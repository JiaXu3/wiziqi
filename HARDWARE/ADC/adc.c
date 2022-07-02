#include "adc.h"
#include "delay.h"


void ADC_Init(void)
{
	RCC->AHB1ENR|=1<<0;
	RCC->APB2ENR|=1<<8;
	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AIN,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PD);
	RCC->APB2RSTR|=1<<8;  //��λADC�ӿ�
	RCC->APB2RSTR&=~(1<<8);  //��λ����
	ADC->CCR|=3<<16;  //�ķ�Ƶ
	ADC->CCR|=1<<23;  //ʹ���¶ȴ�����
	ADC1->CR1=0;
	ADC1->CR2=0;
	ADC1->CR1|=0<<24;  //�ֱ���12λ
	ADC1->CR1|=0<<8;   //��ɨ��ģʽ
	ADC1->CR2&=~(1<<1);  //����ת��ģʽ
	ADC1->CR2&=~(1<<11);  //�Ҷ���
	ADC1->CR2|=0<<28;  //�������
	ADC1->SQR1&=~(0xF<<20);
	ADC1->SQR1|=0<<20;     //һ��ת���ڹ���������
	
	//����ͨ��16ת��ʱ��
	ADC1->SMPR2&=~(7<<(3*(16-10)));  //����ʱ�����
	ADC1->SMPR2|=7<<(3*(16-10));    //����Ϊ480������
	ADC1->CR2|=1<<0;      //ADON
	
}

//��ȡADC��ֵ
//ch  ͨ��
//����ֵ ת�����
u16 Get_Adc(u8 ch)
{
	ADC1->SQR3&=0XFFFFFFE0;
	ADC1->SQR3|=ch;
	ADC1->CR2|=1<<30;
	while(!(ADC1->SR&1<<1))
		; //�ȴ�����
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
//��ȡ�¶�
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
	RCC->APB2RSTR|=1<<8;  //��λADC�ӿ�
	RCC->APB2RSTR&=~(1<<8);  //��λ����
	ADC->CCR|=3<<16;  //�ķ�Ƶ
	//ADC->CCR|=1<<23;  //ʹ���¶ȴ�����
	ADC3->CR1=0;
	ADC3->CR2=0;
	ADC3->CR1|=0<<24;  //�ֱ���12λ
	ADC3->CR1|=0<<8;   //��ɨ��ģʽ
	ADC3->CR2&=~(1<<1);  //����ת��ģʽ
	ADC3->CR2&=~(1<<11);  //�Ҷ���
	ADC3->CR2|=0<<28;  //�������
	ADC3->SQR1&=~(0xF<<20);
	ADC3->SQR1|=0<<20;     //һ��ת���ڹ���������
	
	//����ͨ��5ת��ʱ��
	ADC3->SMPR2&=~(7<<(3*5));  //����ʱ�����
	ADC3->SMPR2|=7<<(3*5);    //����Ϊ480������
	ADC3->CR2|=1<<0;      //ADON
	
}
u16 Get_Adc3_Val(u8 ch)
{
	ADC3->SQR3&=0XFFFFFFE0;
	ADC3->SQR3|=ch;
	ADC3->CR2|=1<<30;
	while(!(ADC3->SR&1<<1))
		; //�ȴ�����
	return ADC3->DR;
}







