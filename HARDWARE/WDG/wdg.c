#include"wdg.h"
#include"led.h"


void IWDG_Init(u8 prer,u16 rlr)
{
	IWDG->KR=0X5555;
	IWDG->PR=prer;
	IWDG->RLR=rlr;
	IWDG->KR=0XAAAA;
	IWDG->KR=0XCCCC;
}

void IWDG_Feed(void)
{
	IWDG->KR=0XAAAA;
}

u8 WWDG_CNT=0x7f;
//tr ��������ֵ            wr w[6:0]����ֵ            fprerԤ��Ƶϵ��
void WWDG_Init(u8 tr,u8 wr,u8 fprer)
{
//	RCC->APB2LPENR|=1<<11;    ���󣺣� ʱ��ʹ�ܴ���  ��ȷ��������
//	WWDG_CNT=tr&WWDG_CNT;
//	WWDG->CFR|=fprer<<7;
//	WWDG->CFR&=0XFF80;  
//	WWDG->CFR|=wr;
//	WWDG->CR=WWDG_CNT;
//	WWDG->CR|=1<<7;
//	MY_NVIC_Init(2,3,WWDG_IRQn,2);
//	WWDG->SR=0x00;
//	WWDG->CFR|=1<<9;
	RCC->APB1ENR|=1<<11; 	//ʹ��wwdgʱ�� 
	WWDG_CNT=tr&WWDG_CNT;   //��ʼ��WWDG_CNT.     
	WWDG->CFR|=fprer<<7;    //PCLK1/4096�ٳ�2^fprer 
	WWDG->CFR&=0XFF80;      
	WWDG->CFR|=wr;     		//�趨����ֵ      
	WWDG->CR|=WWDG_CNT; 	//�趨������ֵ 
	WWDG->CR|=1<<7;  		//�������Ź�      
	MY_NVIC_Init(2,3,WWDG_IRQn,2);//��ռ2�������ȼ�3����2     
	WWDG->SR=0X00; 			//�����ǰ�����жϱ�־λ 
	WWDG->CFR|=1<<9;        //ʹ����ǰ�����ж� 
}

void WWDG_Set_Counter(u8 cnt)
{
	WWDG->CR=(cnt&0x7f);
	
}

void WWDG_IRQHandler(void)
{
	WWDG_Set_Counter(WWDG_CNT);
	WWDG->SR=0x00;
	LED1=!LED1;
	
}
