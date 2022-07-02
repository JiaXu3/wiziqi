#include "dac.h"

void DAC1_Init(void)
{
	//enable GPIO PA4
	RCC->AHB1ENR|=1<<0;
	RCC->APB1ENR|=1<<29;
	GPIO_Set(GPIOA,PIN4,GPIO_MODE_AIN,GPIO_OTYPE_PP,0,GPIO_PUPD_PU);
	DAC->CR|=1<<0;
	DAC->CR|=1<<1;
	DAC->CR&=~(1<<2);
	DAC->CR&=~(1<<3);
	DAC->CR&=~(1<<6);
	DAC->CR&=~(1<<8);
	DAC->CR&=~(1<<12);
	DAC->DHR12R1=0;
		
}
//vol 0-3300 ±íÊ¾0-3.3V
void DAC_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC->DHR12R1=temp;	
}



