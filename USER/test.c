#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "tftlcd.h"
#include "key.h"  
#include "touch.h"
#include "game.h"
#include "text.h"
#include "rng.h"
#include "timer.h"
#include "tpad.h"	
#include "beep.h"
#include "exit.h"


/*PA4����PC0*/
int main(void)
{ 
	int nm;
  static u8 flag1=0;

	u32 timeCount=30;
	static u8 voice=0;
	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz 

	delay_init(168);			//��ʱ��ʼ��  
	uart_init(84,115200);		//��ʼ�����ڲ�����Ϊ115200  	 
	LED_Init();					//��ʼ��LED 
 	LCD_Init();					//LCD��ʼ�� 	
	CalExit_Init();             //У׼�жϳ�ʼ��
	KEY_Init(); 				//������ʼ��  
	TIM3_Int_Init(100-1,8400-1);//��ʼ����ʱ�� 10ms ����ɨ�败����
	tp_dev.init();				//��������ʼ��
	BEEP_Init();
	
	while (font_init());		//�ֿ��ʼ��
	while (RNG_Init());			//�������ʼ��
	
	//��Ϸ��ʼ��
	 GameInit(); 
		while(1)
		{ 
					  //��ʼ��Ϸ
			GameStart();

		}
}
