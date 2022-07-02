#include"beep.h"
#include "delay.h"


void BEEP_Init(void)
{
	RCC->AHB1ENR|=1<<5;
	GPIO_Set(GPIOF,PIN8,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PD);
	BEEP=0;
}


void BEEP_Run(u8 time)
{
	BEEP=1;
	delay_ms(time);
	BEEP=0;
	
}

