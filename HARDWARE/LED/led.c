#include"led.h"

void LED_Init(void)
{
	RCC->AHB1ENR|=1<<5;
	GPIO_Set(GPIOF,PIN10,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	GPIO_Set(GPIOF,PIN9,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	LED0=1;
	LED1=1;
}
