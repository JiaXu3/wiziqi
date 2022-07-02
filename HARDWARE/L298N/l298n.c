#include "l298n.h"
#include "led.h"
#include "delay.h"

void Motor_Init(void)
{
	RCC->AHB1ENR|=1<<2;
	GPIO_Set(GPIOC,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	GPIO_Set(GPIOC,PIN3,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	GPIO_Set(GPIOC,PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	GPIO_Set(GPIOC,PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);
	A1=1;
	A2=0;
	B1=0;
	B2=0;
}

void Motor_Run(u8 mode,u16 step,u16 us)
{
	u16 Nowstep=0;
	u8 i=1;
	if(mode==1)
	{
		while(Nowstep<step)
		{
		switch(i)
		{
			case 1: //A
			{
				GPIOC->ODR|=1<<2;
				GPIOC->ODR&=~(1<<3);
				GPIOC->ODR&=~(1<<4);
				GPIOC->ODR&=~(1<<5);
				Nowstep++;
				break;			
			}
			case 2: //B
			{	
				GPIOC->ODR&=~(1<<2);
				GPIOC->ODR|=1<<3;
				GPIOC->ODR&=~(1<<4);
				GPIOC->ODR&=~(1<<5);
				Nowstep++;
				break;
			}
			case 3:  //~A
			{
				GPIOC->ODR&=~(1<<2);
				GPIOC->ODR&=~(1<<3);
				GPIOC->ODR|=1<<4;
				GPIOC->ODR&=~(1<<5);
				Nowstep++;
				break;			
			}
			case 4:  //~B
			{
				GPIOC->ODR&=~(1<<2);
				GPIOC->ODR&=~(1<<3);
				GPIOC->ODR&=~(1<<4);
				GPIOC->ODR|=1<<5;
				Nowstep++;
				break;
			}									
		}
		delay_us(us);
		i++;
		if(i>3)
	{
		i=1;
	}
	}
		Nowstep=0;
}
	else if(mode==0)
	{
		while(Nowstep<step)
		{
		switch(i)
		{ 
			case 1:   //A
			{
				GPIOC->ODR|=1<<2;
				GPIOC->ODR&=~(1<<3);
				GPIOC->ODR&=~(1<<4);
				GPIOC->ODR&=~(1<<5);
				Nowstep++;
				break;			
			}
			case 2: //~B
			{	
				GPIOC->ODR&=~(1<<2);
				GPIOC->ODR&=~(1<<3);
				GPIOC->ODR&=~(1<<4);
				GPIOC->ODR|=1<<5;
				Nowstep++;
				break;
			}
			case 3:  //~A
			{
				GPIOC->ODR&=~(1<<2);
				GPIOC->ODR&=~(1<<3);
				GPIOC->ODR|=1<<4;
				GPIOC->ODR&=~(1<<5);
				Nowstep++;
				break;			
			}
			case 4:  //B
			{
				GPIOC->ODR&=~(1<<2);
				GPIOC->ODR|=1<<3;
				GPIOC->ODR&=~(1<<4);
				GPIOC->ODR&=~(1<<5);
				Nowstep++;
				break;
			}	
								
		}
		delay_us(us);
		i++;
		if(i>3)
	{
		i=1;
	}
	}
		Nowstep=0;
}
}

