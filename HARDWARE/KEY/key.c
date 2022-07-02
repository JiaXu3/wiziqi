#include"key.h"
#include"delay.h"

void KEY_Init(void)
{
	RCC->AHB1ENR|=1<<4;
	RCC->AHB1ENR|=1<<0;
	GPIO_Set(GPIOE,PIN2|PIN3|PIN4,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);
	//GPIO_Set(GPIOE,PIN2|PIN3|PIN4,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);	//PE2~4������������
	//GPIO_Set(GPIOA,PIN0,GPIO_MODE_IN,0,0,GPIO_PUPD_PD); 
	KEY2 =1;
	KEY1=1;
	KEY0=1;
	KEY3=0;
}
//u8 KEY_Scan(void)
//{
//	//key_upλ1δ���£�0����
//	static u8 key_up=1;
//	//���ȼ� 3>2>1>0  ��ֵΪ��ż�һ
//	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==1))
//	{
//		delay_ms(10);
//		key_up=0;
//		if(KEY3==1)
//			return 4;
//		else if(KEY2==0)
//			return 3;
//		else if(KEY1==0)
//			return 2;
//		else if(KEY0==0)
//			return 5;
//		
//	}
//	else if(KEY0==1||KEY1==1||KEY2==1||KEY3==0)
//	{
//		key_up=1;
//		return 0;
//	}
//}
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return 5;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
		else if(KEY3==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==0)key_up=1; 	    
 	return 0;// �ް�������
}