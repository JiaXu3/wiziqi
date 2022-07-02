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
//tr 计数器的值            wr w[6:0]窗口值            fprer预分频系数
void WWDG_Init(u8 tr,u8 wr,u8 fprer)
{
//	RCC->APB2LPENR|=1<<11;    错误：： 时钟使能错了  正确代码如下
//	WWDG_CNT=tr&WWDG_CNT;
//	WWDG->CFR|=fprer<<7;
//	WWDG->CFR&=0XFF80;  
//	WWDG->CFR|=wr;
//	WWDG->CR=WWDG_CNT;
//	WWDG->CR|=1<<7;
//	MY_NVIC_Init(2,3,WWDG_IRQn,2);
//	WWDG->SR=0x00;
//	WWDG->CFR|=1<<9;
	RCC->APB1ENR|=1<<11; 	//使能wwdg时钟 
	WWDG_CNT=tr&WWDG_CNT;   //初始化WWDG_CNT.     
	WWDG->CFR|=fprer<<7;    //PCLK1/4096再除2^fprer 
	WWDG->CFR&=0XFF80;      
	WWDG->CFR|=wr;     		//设定窗口值      
	WWDG->CR|=WWDG_CNT; 	//设定计数器值 
	WWDG->CR|=1<<7;  		//开启看门狗      
	MY_NVIC_Init(2,3,WWDG_IRQn,2);//抢占2，子优先级3，组2     
	WWDG->SR=0X00; 			//清除提前唤醒中断标志位 
	WWDG->CFR|=1<<9;        //使能提前唤醒中断 
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
