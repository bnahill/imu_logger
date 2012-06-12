#ifndef __EXTI_H_
#define __EXTI_H_

#include "stm32f10x.h"
#include "sensor_config.h"

typedef void (*exti_callback_t)(uint8_t);

typedef struct _exti_handler{
	int valid;
	exti_callback_t cb;
} exti_handler_t;

void exti_init(void);

int exti_register_handler(GPIO_TypeDef const * gpio,
                          uint8_t pin_num,
                          EXTITrigger_TypeDef direction,
                          exti_callback_t cb);

#endif
