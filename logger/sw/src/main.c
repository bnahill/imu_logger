/*!
 @file main.c
 @author Ben Nahill <bnahill@gmail.com>
 @brief The high-level control logic for the logging system
 */
 

#include "stm32f10x.h"
#include "exti.h"
#include "lsm303.h"
#include "lpry.h"
#include "lps001.h"
#include "ff.h"
#include "rtc.h"
#include "logger.h"
#include "button.h"
#include "jitter_buffer.h"
#include "activity_detect.h"
#include "sensor_config.h"

#define LED_GPIO GPIOB
#define LED_PIN  BIT(9)
#define LED_CLK  RCC_APB2Periph_GPIOB

/*!
 @addtogroup application
 @{
 @name Configuration Flags
 @{
 */

//! Flag for debugging to not log any data
#define DO_LOG 1

/*!
 @brief Flag to enable stop mode when not recording (core debug doesn't work
 when stopped)
 */
#define DO_LOPWR 1

/*!
 @brief Flag to enable the internal RC oscillator to drive debug circuitry in
 stop mode
 */
#define DO_LP_DEBUG 1

//! Flag to enable use of accelerometer to wake up
#define DO_LSM_INTERRUPT 0

//! Flag to enable activity detection to put back to sleep
#define DO_ACTIVITY_DETECT 0

//! Flag to enable clean shutdown on low voltage
#define DO_LV_SHUTDOWN 1

//! @} @} Configuration flags

#if !DO_LOPWR
#undef DO_LP_DEBUG
#define DO_LP_DEBUG 0
#endif

static volatile uint32_t tick = 0;
static volatile uint32_t frame_count;

/*!
 @brief Take data from sensors and put it in the jitter buffer
 */
void frame_to_jb(void);

void led_init(void);
void led_set(void);
void led_clr(void);

//! The current operating mode
static enum {
	//! Brief startup routine
	MODE_INIT,
	//! Waiting for action
	MODE_STOPPED,
	//! No activity detected
	MODE_IDLE,
	//! Currently recording data
	MODE_RUNNING,
	//! An error has occurred
	MODE_ERROR,
	//! System has shut down for a critical error
	MODE_SHUTDOWN
} mode;

/*!
 @addtogroup application Application
 @brief The business logic driving the logger application
 
 The application behavior is a cooperative effort between the
 SysTick_Handler() (running in interrupt context) and main(). During active 
 recording, do_run() handles writes to the logger. Since these operations are
 inconsistent in their run time, transfers are asynchronously handled in a
 two-stage process by the SysTick_Handler(). On each cycle, sensors are
 checked to see if they have new data. If they do, a frame is assembled and
 pushed onto the jitter buffer. Then new sensor readings are started according
 to each sensor's schedule. main() then checks (loosely synchronized with
 SysTick) the jitter buffer and writes to the log accordingly.W
 
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

//! @name Run modes
//! @{

/*!
 @brief Provide a quick flashing init sequence to show it is starting up.
 */
static INLINE void do_init(void);

/*!
 @brief Provide feedback feedback for errors
 */
static INLINE void do_error(void);

/*!
 @brief Perform the repeated sensor logging
 */
static INLINE void do_run(void);

//! @}

#if DO_LSM_INTERRUPT
//! A callback for interrupts from the accelerometer
static void lsm_wakeup(void *_);

/*!
 @brief Configure accelerometer interrupt
 
 The configuration is to detect acceration exceeding a threshold value. 
 The high-pass filter is enabled so as to minimize the effects of gravity.
 */
static void config_lsm_interrupts(void);
#endif


#if DO_LV_SHUTDOWN

static void lv_shutdown(void *_);

#endif

//! @}

 
//! @callgraph
int main(void){	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
#if (DO_LOPWR | DO_LV_SHUTDOWN)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
#endif
	
	exti_init();
	button_init();
	
	// Initialize sensors and peripherals
	rtc_init();
	lsm303_init(&magacc);
	lpry_init();
	led_init();
	lps_init();
	
#if DO_LP_DEBUG
	DBGMCU_Config(DBGMCU_STOP, ENABLE);
#endif
	
	// Go to low power and wait for button press
	lpry_power_off();
	lps_set_pm(LPS_PM_OFF);
	while(!button_check_press()){
#if DO_LOPWR
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
#endif
	}
	
	SystemCoreClockUpdate();

	mode = MODE_INIT;

#if DO_LV_SHUTDOWN
	PWR_PVDLevelConfig(PWR_PVDLevel_2V7);
	PWR_PVDCmd(ENABLE);
	exti_config(NULL, 16, EXTI_Trigger_Rising);
	exti_register_handler(16, lv_shutdown, NULL);
#endif

#if DO_LSM_INTERRUPT
	config_lsm_interrupts();
#endif

	
	while(1){
		lpry_power_off();
		lsm303_set_pm(&magacc, LSM_PM_LP_5);
		lps_set_pm(LPS_PM_OFF);
		
		led_set();
		// If there was a button press, wait for release before actually starting
		wait_for_release();
		led_clr();
		
		switch(mode){
		case MODE_INIT:
			do_init();
			break;
		case MODE_ERROR:
			do_error();
			break;
		case MODE_SHUTDOWN:
#if DO_LOG
			if(logger_is_open()){
				logger_close();
			}
#endif
			lsm303_int_disable(&magacc, LSM_INT_1);
			lsm303_set_pm(&magacc, LSM_PM_OFF);
			lpry_power_off();
			while(1){
#if DO_LOPWR
				PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
#endif
			}
			break;
		case MODE_STOPPED:
#if DO_LSM_INTERRUPT
			lsm303_int_enable(&magacc, LSM_INT_1);
#endif
			while(1){
				// Wait for button press or change of modes
#if DO_LOPWR
				if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
					PWR_ClearFlag(PWR_FLAG_WU);
				SystemCoreClockUpdate();
#endif
				if(mode != MODE_STOPPED)
					break;
				if(button_check_press()){
					mode = MODE_RUNNING;
					break;
				}
#if DO_LOPWR
				PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
#endif
			}
			break;
		case MODE_RUNNING:
			do_run();
			break;
		default:
			break;
		}
	}
}

static INLINE void do_init(void){
	int i;
	SysTick_Config(SystemCoreClock / 50);
	for(i = 0; i < 5; i++){
		tick = 0;
		led_set();
		while(tick < 2);
		led_clr();
		while(tick < 10);
	}
	SysTick_Config(0);
	tick = 0;
	mode = MODE_STOPPED;
}

static INLINE void do_error(void){
	SysTick_Config(SystemCoreClock / 50);
	while(1){
		tick = 0;
		led_clr();
		while(tick < 5);
		led_set();
		while(tick < 10);
		if(button_check_press()){
			SysTick_Config(0);
			tick = 0;
			mode = MODE_STOPPED;
			led_clr();
			break;
		}
	}
}

static INLINE void do_run(void){
	int i;
	jb_frame_t frame;
	uint32_t calib_count;
#if DO_ACTIVITY_DETECT
	ac_result_t ac_result;
	int since_active = -1;
#endif

	frame_count = 0;
	
	lps_set_pm(LPS_PM_NORMAL);
	lsm303_set_pm(&magacc, LSM_PM_NORM);
	lpry_power_on();
#if DO_ACTIVITY_DETECT
	ac_init();
#endif
	
#if DO_LOG
	calib_count = 0;
	
	if(magacc.calib)
		calib_count |= 1;
	
	if(logger_init("test", calib_count) == NULL){
		mode = MODE_ERROR;
		return;
	}
	
		
#endif
	
	led_set();
	
	i = 0;
	tick = 0;
	
	jb_init(&jitter_buffer);
	
	SysTick_Config(SystemCoreClock / 50);
	
	while(1){
		// Wait for a tick
		while(!tick);
		// Only decrement it so that we may run multiple times if we
		// miss a deadline
		tick -= 1;
		if(++i == 2){
			led_clr();
		} else if(i == 100){
			i = 0;
			led_set();
		}
		
		//////////////////////////////////////
		// Check for new data frames
		//////////////////////////////////////
		if(jb_pop(&jitter_buffer, &frame)){
#if DO_ACTIVITY_DETECT
			ac_result = ac_add_frame(&frame);
			switch(ac_result){
				case AC_RES_INCOMPLETE:
					break;
				case AC_RES_ACTIVE:
					// Just another active frame
					since_active = 0;
					ac_init();
					break;
				case AC_RES_INACTIVE:
					if(since_active < 0){
						// This was the first period after waking up.
						// Go back to sleep.
						mode = MODE_INIT;
						break;
					}
					since_active += 1;
					if(since_active == activity_num_frames){
						// Been idle too long
						mode = MODE_INIT;
					}
					break;
			}
#endif
#if DO_LOG	
			// And commit them to the log
			logger_update(&frame);
#endif
		}

		// Check for escape triggers
		if(button_check_press()){
			mode = MODE_STOPPED;
			break;
		} else if(mode != MODE_RUNNING){
			// Escape if something changes the mode
			break;
		}
	}
	
	led_clr();
	
	// Delay here to allow all sensor reads to finish
	tick = 0;
	while(tick < 2);

	// Disable SysTick
	SysTick_Config(0);
#if DO_LOG	
	logger_sync();
	logger_close();
#endif
}

#if DO_LV_SHUTDOWN

static void lv_shutdown(void *_){
	mode = MODE_SHUTDOWN;
}

#endif

#if DO_LSM_INTERRUPT

static void lsm_wakeup(void *_){
	lsm303_int_disable(&magacc, LSM_INT_1);
	if(mode == MODE_STOPPED){
		mode = MODE_RUNNING;
	}
}

static void config_lsm_interrupts(void){
	lsm_int_config_t conf;
	conf.arg = NULL;
	conf.cb = lsm_wakeup;
	conf.threshold = 7;
	conf.duration = 4;
	conf.highpass = LSM_ENABLE;
	conf.index = LSM_INT_1;
	conf.mode = LSM_INTMODE_OR;
	conf.src = LSM_INTSRC_XHI | LSM_INTSRC_YHI | LSM_INTSRC_ZHI; //| LSM_INTSRC_XLO | LSM_INTSRC_YLO | LSM_INTSRC_ZLO;
	
	if(!lsm303_int_config(&magacc, &conf))
		while(1);
}

#endif

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
	
	frame.gyro.yaw = gyro.reading.yaw;
	frame.gyro.pitch = gyro.reading.pitch;
	frame.gyro.roll = gyro.reading.roll;
	
	frame.pressure = pressure.pressure;
	frame.temperature = pressure.temperature;
	
	if(!jb_push(&jitter_buffer, &frame))
		mode = MODE_ERROR;
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
		
		if(lsm303_acc_xfer_complete(&magacc)){
			lsm303_update_acc(&magacc);
			do_update = 1;
		}
		
		if(lsm303_mag_xfer_complete(&magacc)){
			lsm303_update_mag(&magacc);
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
		lsm303_read_acc(&magacc);
		if(!(frame_count & 1))
			lsm303_read_mag(&magacc);
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
