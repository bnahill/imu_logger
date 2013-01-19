/*!
 @file platform.cpp
 
 @brief Platform-specific instantiations
 
 @author Ben Nahill <bnahill@gmail.com>
 */
#define __PLATFORM_CPP_

#include "platform.h"
#include "platform_usbserial.h"


I2C Platform::i2c1(I2CD1, OPMODE_I2C, FAST_DUTY_CYCLE_2, 100000);
MMA8452Q Platform::acc1(Platform::i2c1, 0x1C);
MAG3110 Platform::mag1(Platform::i2c1, 0x0E);

///////////////////////////////////////////
// SPI Platform
///////////////////////////////////////////

SPI Platform::spi1(SPID1);

SPI Platform::spi2(SPID2);

///////////////////////////////////////////
// L3GD20 Platform
///////////////////////////////////////////

L3GD20_SPI Platform::gyro1(Platform::spi1, {NULL, GPIOC, 4, SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA});


///////////////////////////////////////////
// Button platform configuration
///////////////////////////////////////////


EXTDriver &Button::driver = EXTD1;

//Button Platform::button1(GPIOC, 12, Button::ACTIVE_LOW);
//Button Platform::button2(GPIOC, 10, Button::ACTIVE_LOW);
//Button Platform::button3(GPIOC, 11, Button::ACTIVE_LOW);
Button Platform::button[3] = {
	{GPIOC, 12, Button::ACTIVE_LOW},
	{GPIOC, 10, Button::ACTIVE_LOW},
	{GPIOC, 11, Button::ACTIVE_LOW}
};

///////////////////////////////////////////
// Edge-trigger interrupt configuration
///////////////////////////////////////////

const EXTConfig Platform::extcfg = {
	{
		{EXT_CH_MODE_DISABLED, NULL}, // 0
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL}, // 5
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, Button::callback}, // 10
		{EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, Button::callback},
		{EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, Button::callback},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL}, // 15
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL}, // 20
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL}
	}
};


void Platform::EXTInit(){
	extStart(&EXTD1, &extcfg);
}

///////////////////////////////////////////
// LED platform configuration
///////////////////////////////////////////

gpio_pin_t Platform::led1(GPIOC, 3);


LTC3559 Platform::reg1({GPIOC, 2}, {GPIOC, 1}, {GPIOA, 4}, {GPIOA, 1});

///////////////////////////////////////////
// OLED display platform configuration
///////////////////////////////////////////

LY091WG15 Platform::oled(spi2,
                        {NULL, GPIOB, 11, SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA},
                        {GPIOB, 10}, {GPIOB, 2});


// USBSerial &Platform::get_usb_serial(USBDriver *usbp){
// 	return usbserial1;
// }
// 
// USBSerial Platform::usbserial1(&USBD2);

usbserial1_t Platform::usbserial1({GPIOA, 9});

extern "C" {
void NMIVector(void){while(1);}
void HardFaultVector(void){while(1);}
void MemManageVector(void){while(1);}
void BusFaultVector(void){while(1);}
void UsageFaultVector(void){while(1);}
}
