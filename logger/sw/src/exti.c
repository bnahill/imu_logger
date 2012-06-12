#include "exti.h"

#define NUM_HANDLERS 16

static exti_handler_t handlers[NUM_HANDLERS];

void exti_init(void){
	NVIC_InitTypeDef nvic_init_s;
	int i;
	
	for(i = 0; i < NUM_HANDLERS; i++){
		handlers[i].valid = 0;
	}
	
	nvic_init_s.NVIC_IRQChannelPreemptionPriority = 0x0F;
	nvic_init_s.NVIC_IRQChannelSubPriority = 0x0F;
	nvic_init_s.NVIC_IRQChannelCmd = ENABLE;
	
	nvic_init_s.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&nvic_init_s);
	nvic_init_s.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&nvic_init_s);
	nvic_init_s.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&nvic_init_s);
	nvic_init_s.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&nvic_init_s);
	nvic_init_s.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_Init(&nvic_init_s);
	nvic_init_s.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&nvic_init_s);
	nvic_init_s.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Init(&nvic_init_s);
}

int exti_register_handler(GPIO_TypeDef const * gpio,
                          uint8_t pin_num,
                          EXTITrigger_TypeDef direction,
                          exti_callback_t cb){
	EXTI_InitTypeDef exti_init_s;
	uint32_t mask = BIT(pin_num);
	__disable_irq();
	
	// Check if there's already a handler
	if((pin_num > 15) || handlers[pin_num].valid ||
	   (EXTI->RTSR & mask) || (EXTI->FTSR & mask)){
		__enable_irq();
		return 0;
	}
	
	GPIO_EXTILineConfig(gpio - GPIOA, pin_num);
	
	handlers[pin_num].valid = 1;
	handlers[pin_num].cb = cb;
	
	// Configure EXTI
	exti_init_s.EXTI_Line = BIT(pin_num);
	exti_init_s.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init_s.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_init_s.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init_s);
	// Now configure the interrupts
	
	__enable_irq();
	return 1;
}

/*

static INLINE void exti_run_isr(uint32_t pin_num){
	EXTI->PR = BIT(pin_num);
	if(handlers[pin_num].valid){
		handlers[pin_num].cb(pin_num);
	}
}

void EXTI15_10_IRQHandler(void){
	uint32_t pr = EXTI->PR;
	if(pr & BIT(10)){exti_run_isr(10);}
	if(pr & BIT(11)){exti_run_isr(11);}
	if(pr & BIT(12)){exti_run_isr(12);}
	if(pr & BIT(13)){exti_run_isr(13);}
	if(pr & BIT(14)){exti_run_isr(14);}
	if(pr & BIT(15)){exti_run_isr(15);}
}

void EXTI9_5_IRQHandler(void){
	uint32_t pr = EXTI->PR;
	if(pr & BIT(5)){exti_run_isr(5);}
	if(pr & BIT(6)){exti_run_isr(6);}
	if(pr & BIT(7)){exti_run_isr(7);}
	if(pr & BIT(8)){exti_run_isr(8);}
	if(pr & BIT(9)){exti_run_isr(9);}
}

void EXTI0_IRQHandler(void){exti_run_isr(0);}
void EXTI1_IRQHandler(void){exti_run_isr(1);}
void EXTI2_IRQHandler(void){exti_run_isr(2);}
void EXTI3_IRQHandler(void){exti_run_isr(3);}
void EXTI4_IRQHandler(void){exti_run_isr(4);}

*/