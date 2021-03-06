#include "exti.h"

#define NUM_HANDLERS 19

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
	//nvic_init_s.NVIC_IRQChannel = RTC_IRQn;
	//NVIC_Init(&nvic_init_s);
	nvic_init_s.NVIC_IRQChannel = PVD_IRQn;
	NVIC_Init(&nvic_init_s);
	nvic_init_s.NVIC_IRQChannel = USBWakeUp_IRQn;
	NVIC_Init(&nvic_init_s);
}

int exti_config(GPIO_TypeDef const * gpio,
                uint8_t pin_num,
                EXTITrigger_TypeDef direction){
	EXTI_InitTypeDef exti_init_s;
	uint8_t portsrc;
	exti_handler_t * const handler = &handlers[pin_num];
	__disable_irq();
	
	if(handler->valid){
		// This one's already taken
		__enable_irq();
		return 0;
	}
	
	if(gpio){
		if(pin_num > 15){
			__enable_irq();
			return 0;
		}
		switch((uint32_t)gpio){
		case (uint32_t)GPIOA:
			portsrc = GPIO_PortSourceGPIOA;
			break;
		case (uint32_t)GPIOB:
			portsrc = GPIO_PortSourceGPIOB;
			break;
		case (uint32_t)GPIOC:
			portsrc = GPIO_PortSourceGPIOC;
			break;
		case (uint32_t)GPIOD:
			portsrc = GPIO_PortSourceGPIOD;
			break;
		case (uint32_t)GPIOE:
			portsrc = GPIO_PortSourceGPIOE;
			break;
		default:
			__enable_irq();
			return 0;
		}
		GPIO_EXTILineConfig(portsrc, pin_num);
	}
	
	handler->gpio = gpio;
	handler->valid = 1;
	handler->cb = NULL;
	
	// Configure EXTI
	exti_init_s.EXTI_Line = BIT(pin_num);
	exti_init_s.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init_s.EXTI_Trigger = direction;
	exti_init_s.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init_s);
	// Now configure the interrupts
	
	__enable_irq();
	return 1;
	
}

void exti_register_handler(uint8_t pin_num,
                           exti_callback_t cb,
                           void *arg){
	exti_handler_t * const handler = &handlers[pin_num];
	if(!handler->valid)
		return;
	__disable_irq();
	handler->cb = cb;
	handler->arg = arg;
	__enable_irq();
}

#if 0
int exti_register_handler(GPIO_TypeDef const * gpio,
                          uint8_t pin_num,
                          EXTITrigger_TypeDef direction,
                          exti_callback_t cb,
                          void *arg){
	
	uint8_t portsrc;
	uint32_t mask = BIT(pin_num);
	exti_handler_t * const handler = &handlers[pin_num];
	__disable_irq();
	
	// Check if there's already a handler
	if((pin_num > 15) || (handler->valid && handler->gpio != gpio) ||
	   (EXTI->RTSR & mask) || (EXTI->FTSR & mask)){
		__enable_irq();
		return 0;
	}
	
	switch((uint32_t)gpio){
	case (uint32_t)GPIOA:
		portsrc = GPIO_PortSourceGPIOA;
		break;
	case (uint32_t)GPIOB:
		portsrc = GPIO_PortSourceGPIOB;
		break;
	case (uint32_t)GPIOC:
		portsrc = GPIO_PortSourceGPIOC;
		break;
	case (uint32_t)GPIOD:
		portsrc = GPIO_PortSourceGPIOD;
		break;
	case (uint32_t)GPIOE:
		portsrc = GPIO_PortSourceGPIOE;
		break;
	default:
		__enable_irq();
		return 0;
	}
	
	GPIO_EXTILineConfig(portsrc, pin_num);
	
	handler->gpio = gpio;
	handler->valid = 1;
	handler->cb = cb;
	handler->arg = arg;
	
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
#endif

static INLINE void exti_run_isr(uint32_t pin_num){
	EXTI->PR = BIT(pin_num);
	if(handlers[pin_num].valid && handlers[pin_num].cb){
		handlers[pin_num].cb(handlers[pin_num].arg);
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
void PVD_IRQHandler(void){exti_run_isr(16);}
void RTC_IRQHandler(void){exti_run_isr(17);}
void USBWakeUp_IRQHandler(void){exti_run_isr(18);}
