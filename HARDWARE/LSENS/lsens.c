#include "lsens.h"
#include "delay.h"

void Lsens_Init(void)
{
	RCC->AHB1ENR|=1<<5;
	GPIO_Set(GPIOF,PIN7,GPIO_MODE_AIN,GPIO_OTYPE_PP,0,GPIO_PUPD_PU);
	ADC3_Init();
	
}
//读取光强 0-100  光强增大

u8 Get_Lsens_Value(void)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_Adc3_Val(5);
		delay_ms(5);
	}
	temp_val/=LSENS_READ_TIMES;
	if(temp_val>=4000)
		temp_val=4000;
	return (u8)(100-(temp_val/40));
	
}