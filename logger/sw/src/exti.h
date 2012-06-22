#ifndef __EXTI_H_
#define __EXTI_H_

#include "stm32f10x.h"
#include "sensor_config.h"

typedef void (*exti_callback_t)(void *);

typedef struct _exti_handler{
	int valid;
	GPIO_TypeDef const * gpio;
	exti_callback_t cb;
	void *arg;
} exti_handler_t;

void exti_init(void);

int exti_register_handler(GPIO_TypeDef const * gpio,
                          uint8_t pin_num,
                          EXTITrigger_TypeDef direction,
                          exti_callback_t cb,
                          void *arg);

int exti_unregister_handler(GPIO_TypeDef const * gpio,
                            uint8_t pin_num);

static INLINE void exti_mask(uint32_t mask){
	EXTI->IMR &= ~mask;
}

static INLINE void exti_unmask(uint32_t mask){
	EXTI->IMR |= mask;	
}

static INLINE void exti_clear(uint32_t mask){
	EXTI->PR = mask;
}

#endif
