#include"timer.h"
#include"led.h"
#include "touch.h"
#include "tpad.h"
#include "game.h"
#include "delay.h"


void TIM3_IRQHandler(void)
{ 	
	if(TIM3->SR&0X0001)//����ж�
	{
		tp_dev.scan(0);	
//		if(TPAD_Scan(0))
//		{
//			delay_ms(1000);
//		}
		
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 psc,u16 arr)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//��ռ1�������ȼ�3����2									 
}
//arr:�Զ���װ��ֵ psc:ʱ��Ԥ��Ƶϵ����
void PWM14_CH1_Init(u32 psc,u32 arr)
{
	RCC->APB1ENR|=1<<8;
	RCC->AHB1ENR|=1<<5;
	GPIO_Set(GPIOF,PIN9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_AF_Set(GPIOF,9,9);
	TIM14->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM14->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM14->CCMR1|=6<<4;
	TIM14->CCMR1|=1<<3;
	TIM14->CCER|=1<<0;
	TIM14->CCER|=1<<1;
	TIM14->CR1|=1<<7;
	TIM14->CR1|=1<<0;
	
}

void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{		 
	RCC->APB1ENR|=1<<3;   	//TIM5 ʱ��ʹ�� 
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//���ù���,����
	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF2
	  
 	TIM5->ARR=arr;  		//�趨�������Զ���װֵ   
	TIM5->PSC=psc;  		//Ԥ��Ƶ�� 

	TIM5->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
 	TIM5->CCMR1|=0<<4; 		//IC1F=0000 ���������˲��� ���˲�
 	TIM5->CCMR1|=0<<10; 	//IC1PS=00 	���������Ƶ,����Ƶ 

	TIM5->CCER|=0<<1; 		//CC1P=0	�����ز���
	TIM5->CCER|=1<<0; 		//CC1E=1 	�������������ֵ������Ĵ�����

	TIM5->EGR=1<<0;			//������Ʋ��������¼�,ʹд��PSC��ֵ������Ч,���򽫻�Ҫ�ȵ���ʱ������Ż���Ч!
	TIM5->DIER|=1<<1;   	//������1�ж�				
	TIM5->DIER|=1<<0;   	//��������ж�	
	TIM5->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//��ռ2�������ȼ�0����2	   
}
//[7] 0��ʾû�гɹ����� 1��ʾ�ɹ�����һ��
//[6] 0��ʾ��û���񵽵͵�ƽ 1��ʾ���񵽸ߵ�ƽ
//[5:0]  ����ߵ�ƽ������Ĵ���
u8 TIM5CH1_CAPTURE_STA=0;  //���벶��״̬
u32 TIM5CH1_CAPTURE_VAL;   //���벶��ֵ


void TIM5_IRQHandler(void)
{ 		    
	u16 tsr;
	tsr=TIM5->SR;
 	if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{
		if(tsr&0X01)//���
		{	     
			if(TIM5CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�������һ��
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM5CH1_CAPTURE_STA++;
			}	 
		}
		if(tsr&0x02)//����1���������¼�
		{	
			if(TIM5CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
			    TIM5CH1_CAPTURE_VAL=TIM5->CCR1;	//��ȡ��ǰ�Ĳ���ֵ.
	 			TIM5->CCER&=~(1<<1);			//CC1P=0 ����Ϊ�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIM5CH1_CAPTURE_STA=0;			//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
				TIM5->CR1&=~(1<<0)		;    	//ʹ�ܶ�ʱ��2
	 			TIM5->CNT=0;					//���������
	 			TIM5->CCER|=1<<1; 				//CC1P=1 ����Ϊ�½��ز���
				TIM5->CR1|=0x01;    			//ʹ�ܶ�ʱ��2
			}		    
		}			     	    					   
 	}
	TIM5->SR=0;//����жϱ�־λ   
}


//arr:�Զ���װ��ֵ psc:ʱ��Ԥ��Ƶϵ����
void TIM9_CH2_PWM_Init(u16 arr,u16 psc)
{
	RCC->APB2ENR|=1<<16;
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOA,PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_AF_Set(GPIOA,3,3);
	
	TIM9->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM9->PSC=psc;  	//Ԥ��Ƶ��	  
	
	TIM9->CCMR1|=6<<12;
	TIM9->CCMR1|=1<<11;
	TIM9->CCER|=1<<4;
	TIM9->CR1|=1<<7;
	TIM9->CR1|=1<<0;
	
}


