#include "led.h"

void TRAILING_Init(void)
{
	RCC->AHB1ENR|=1<<1; //ʹ��PB0ʱ��
	RCC->AHB1ENR|=1<<5;
	GPIO_Set(GPIOB,PIN0,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//ʹ������
	GPIO_Set(GPIOF,PIN9,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//ʹ��LED DS0
	
}

u8 TRAILING_Read(void)
{
	if(GPIOB->IDR&1<<0)
		return 1;
	else
		return 0;
}
