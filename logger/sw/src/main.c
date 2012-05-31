/*!
 @file main.c
 @author Ben Nahill <bnahill@gmail.com>
 @brief The high-level control logic for the logging system
 */
 

#include "stm32f10x.h"
#include "lsm303.h"
#include "lpry.h"
#include "lps001.h"
#include "ff.h"
#include "rtc.h"
#include "logger.h"
#include "button.h"
#include "jitter_buffer.h"

#define LED_GPIO GPIOB
#define LED_PIN  BIT(9)
#define LED_CLK  RCC_APB2Periph_GPIOB

//! Flag for debugging to not log any data
#define DO_LOG 1

static volatile uint32_t tick = 0;
static volatile uint32_t frame_count;

/*!
 @brief Take data from sensors and put it in the jitter buffer
 */
void frame_to_jb(void);

void led_init(void);
void led_set(void);
void led_clr(void);

static enum {
	MODE_STOPPED,
	MODE_IDLE,
	MODE_RUNNING
} mode;

/*!
 @addtogroup application Application
 @brief The business logic driving the logger application
 
 The application behavior is a cooperative effort between the
 SysTick_Handler() (running in interrupt context) and main(). During active 
 recording, main() handles writes to the logger. Since these operations are
 inconsistent in their run time, transfers are asynchronously handled in a
 two-stage process by the SysTick_Handler(). On each cycle, sensors are
 checked to see if they have new data. If they do, a frame is assembled and
 pushed onto the jitter buffer. Then new sensor readings are started according
 to each sensor's schedule. main() then checks (loosely synchronized with
 SysTick) the jitter buffer and writes to the log accordingly.
 
 @{
 */

//! A jitter buffer to accomodate the inconsistent timing of the SD card
static jitter_buffer_t jitter_buffer;

/*!
 @brief The application entry point
 */
int main(void);

/*!
 @brief The handler of the SysTick device. Handles sensor reads and data collection.
 */
void SysTick_Handler(void);

//! @}


//! @callgraph
int main(void){
	int i;
	jb_frame_t frame;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	
	// Initialize sensors and peripherals
	rtc_init();
	lsm303_init();
	lpry_init();
	led_init();
	button_init();
	lps_init();
	
	while(1){
		mode = MODE_STOPPED;
		
		// Wait for button press
		while(!button_check_press());
	
		mode = MODE_RUNNING;
		
		SysTick_Config(SystemCoreClock / 50);
		
		frame_count = 0;
		
#if DO_LOG
		if(logger_init("test") == NULL)
			while(1);
#endif
			
		led_set();
		
		i = 0;
		
		jb_init(&jitter_buffer);
		
		while(1){
			// Wait for a tick
			while(!tick);
			// Only decrement it so that we may run multiple times if we
			// miss a deadline
			tick -= 1;
			if(++i == 2){
				led_clr();
			} else if(i == 50){
				i = 0;
				led_set();
			}
			
			//////////////////////////////////////
			// Check for new data frames
			//////////////////////////////////////
			
			if(jb_pop(&jitter_buffer, &frame)){
#if DO_LOG	
				// And commit them to the log
				logger_update(&frame);
#endif
			}


			if(button_check_press())
				break;
		}
		
		led_clr();
		
		// Disable SysTick
		SysTick_Config(0xFFFFFFFF);
#if DO_LOG	
		logger_sync();
		logger_close();
#endif
	}
}

void led_init(void){
	GPIO_InitTypeDef gpio_init_s;
	
	// Initialize GPIO
	RCC_APB2PeriphClockCmd(LED_CLK, ENABLE);
	
	led_clr();
	
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_s.GPIO_Pin = LED_PIN;
	GPIO_Init(LED_GPIO, &gpio_init_s);
}

void led_set(void){
	LED_GPIO->BSRR = LED_PIN;
}

void led_clr(void){
	LED_GPIO->BSRR = LED_PIN << 16;
}

void frame_to_jb(void){
	jb_frame_t frame;
	frame.acc.x = magacc.acc.x;
	frame.acc.y = magacc.acc.y;
	frame.acc.z = magacc.acc.z;
	
	frame.mag.x = magacc.mag.x;
	frame.mag.y = magacc.mag.y;
	frame.mag.z = magacc.mag.z;
	
	frame.gyro.pitch = gyro.reading.pitch;
	frame.gyro.yaw = gyro.reading.yaw;
	frame.gyro.roll = gyro.reading.roll;
	
	frame.pressure = pressure.pressure;
	frame.temperature = pressure.temperature;
	
	if(!jb_push(&jitter_buffer, &frame))
		while(1);
}

//! @callgraph
void SysTick_Handler(void){
	int do_update;
	switch(mode){
	case MODE_RUNNING:
		do_update = 0;
		
		//////////////////////////////////////
		// First check if any sensors are done
		//////////////////////////////////////
		
		if(lsm303_acc_xfer_complete()){
			lsm303_update_acc();
			do_update = 1;
		}
		
		if(lsm303_mag_xfer_complete()){
			lsm303_update_mag();
			do_update = 1;
		}
		
		if(lpry_xfer_complete()){
			lpry_update();
			do_update = 1;
		}
		
		if(lps_xfer_complete()){
			lps_update();
			do_update = 1;
		}
		
		//////////////////////////////////////
		// If any were, post a new frame
		//////////////////////////////////////
		
		if(do_update)
			frame_to_jb();
		
		//////////////////////////////////////
		// Start new measurements
		//////////////////////////////////////
		
		// Start mag/acc readings
		lsm303_read_acc();
		if(!(frame_count & 1))
			lsm303_read_mag();
		// Start pressure reading
		if(!(frame_count & 3))	
			lps_read();
		// Start gyro reading
		lpry_read();
		
		frame_count += 1;
		break;
	default:
		break;
	}
	tick += 1;
}
