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


/*PA4连接PC0*/
int main(void)
{ 
	int nm;
  static u8 flag1=0;

	u32 timeCount=30;
	static u8 voice=0;
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 

	delay_init(168);			//延时初始化  
	uart_init(84,115200);		//初始化串口波特率为115200  	 
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化 	
	CalExit_Init();             //校准中断初始化
	KEY_Init(); 				//按键初始化  
	TIM3_Int_Init(100-1,8400-1);//初始化定时器 10ms 用于扫描触摸屏
	tp_dev.init();				//触摸屏初始化
	BEEP_Init();
	
	while (font_init());		//字库初始化
	while (RNG_Init());			//随机数初始化
	
	//游戏初始化
	 GameInit(); 
		while(1)
		{ 
					  //开始游戏
			GameStart();

		}
}
