#include "adc.h"
#include "delay.h"


void ADC_Init(void)
{
	RCC->AHB1ENR|=1<<0;
	RCC->APB2ENR|=1<<8;
	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AIN,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PD);
	RCC->APB2RSTR|=1<<8;  //复位ADC接口
	RCC->APB2RSTR&=~(1<<8);  //复位结束
	ADC->CCR|=3<<16;  //四分频
	ADC->CCR|=1<<23;  //使能温度传感器
	ADC1->CR1=0;
	ADC1->CR2=0;
	ADC1->CR1|=0<<24;  //分辨率12位
	ADC1->CR1|=0<<8;   //非扫描模式
	ADC1->CR2&=~(1<<1);  //单次转换模式
	ADC1->CR2&=~(1<<11);  //右对齐
	ADC1->CR2|=0<<28;  //软件触发
	ADC1->SQR1&=~(0xF<<20);
	ADC1->SQR1|=0<<20;     //一个转换在规则序列中
	
	//设置通道16转换时间
	ADC1->SMPR2&=~(7<<(3*(16-10)));  //采样时间清空
	ADC1->SMPR2|=7<<(3*(16-10));    //设置为480个周期
	ADC1->CR2|=1<<0;      //ADON
	
}

//获取ADC的值
//ch  通道
//返回值 转换结果
u16 Get_Adc(u8 ch)
{
	ADC1->SQR3&=0XFFFFFFE0;
	ADC1->SQR3|=ch;
	ADC1->CR2|=1<<30;
	while(!(ADC1->SR&1<<1))
		; //等待结束
	return ADC1->DR;
}
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	temp_val/=times;
	return temp_val;
	
	
}
//读取温度
short Get_Temprate(void)
{
	u32 adcx;
	short result;
	double temprate;
	adcx=Get_Adc_Average(16,20);
	temprate=(float)adcx*(3.3/4096);
	temprate=(temprate-0.76)/0.0025+25;
	result=temprate*=100;
	return result;
		
}


void ADC3_Init(void)
{
	RCC->APB2ENR|=1<<10;
	RCC->APB2RSTR|=1<<8;  //复位ADC接口
	RCC->APB2RSTR&=~(1<<8);  //复位结束
	ADC->CCR|=3<<16;  //四分频
	//ADC->CCR|=1<<23;  //使能温度传感器
	ADC3->CR1=0;
	ADC3->CR2=0;
	ADC3->CR1|=0<<24;  //分辨率12位
	ADC3->CR1|=0<<8;   //非扫描模式
	ADC3->CR2&=~(1<<1);  //单次转换模式
	ADC3->CR2&=~(1<<11);  //右对齐
	ADC3->CR2|=0<<28;  //软件触发
	ADC3->SQR1&=~(0xF<<20);
	ADC3->SQR1|=0<<20;     //一个转换在规则序列中
	
	//设置通道5转换时间
	ADC3->SMPR2&=~(7<<(3*5));  //采样时间清空
	ADC3->SMPR2|=7<<(3*5);    //设置为480个周期
	ADC3->CR2|=1<<0;      //ADON
	
}
u16 Get_Adc3_Val(u8 ch)
{
	ADC3->SQR3&=0XFFFFFFE0;
	ADC3->SQR3|=ch;
	ADC3->CR2|=1<<30;
	while(!(ADC3->SR&1<<1))
		; //等待结束
	return ADC3->DR;
}







