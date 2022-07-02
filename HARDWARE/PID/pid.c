#include"pid.h"

void PID_Init(float Sv,float Kp,float Ti,float T,float Td,float Out0)
{
	/*��pid�������г�ʼ��*/
	pid.Sv=Sv;
	pid.Kp=Kp;
	pid.Ti=Ti;
	pid.T=T;
	pid.Td=Td;
	pid.Out0=Out0;
}

void TIM3_Int_Init(void)  //10ms��ʱ����ʼ��
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��
    
 	TIM3->ARR=100-1;  	//�趨�������Զ���װֵ  ����100��Ϊ 10ms
	TIM3->PSC=8400-1;  	//Ԥ��Ƶ��	  10KHz��Ƶ�� 
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//��ռ1�������ȼ�3����2		
}

void TIM3_IRQHandler(void) //10ms�����ж�
{ 		    		  			    
	if(TIM3->SR&0X0001)      //����ж�
		pid.C10ms++;	
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	
}

void TIM5_Int_Init(u32 arr,u16 psc)  //PWM�����ʱ����ʼ��
{
	RCC->APB1ENR|=1<<3;   	//TIM5 ʱ��ʹ�� 
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//���ù���,����
	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF  
 	TIM5->ARR=10-1;  		//�趨�������Զ���װֵ    1ms����һ��ռ�ձ�
	TIM5->PSC=8400-1;  		//Ԥ��Ƶ�� 				10KHz��Ƶ��
	TIM5->DIER|=1<<0;   	//��������ж�	
	TIM5->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//��ռ2�������ȼ�0����2
}

void TIM5_IRQHandler(void) //1msPWM�ж�
{ 		    		  			    
	if(TIM3->SR&0X0001)      //����ж�
		PID_Out(); 	
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	
}

void PID_Calc(void)
{
	/*λ��ʽPID*/
	float ti,Kii,Di,tDi,Kid,Eki;   //�����м�ֵ
	float out,POut,IOut,DOut;			//��������ֵ����
	if(pid.C10ms<(pid.T/10))        //�������������� ����
	{
		return ;
	}
	pid.Ek=pid.Sv-pid.Pv;          //���㱾��ƫ��ֵ
	Eki=pid.Ek-pid.Ek_1;			//���㱾��ƫ��ֵ����һ��ƫ��ֵ�Ĳ�ֵ
	pid.SEk+=pid.Ek;				//��ʷƫ��ֵ�ۼ�
	ti=pid.T/pid.Ti;				//T/Ti �������������ʱ�䳣���Ļ�
	Kii=ti*pid.Kp;					//�˹̶�����
	IOut=Kii*pid.SEk;				//IOut
	POut=pid.Kp*pid.Ek;				//POut
	tDi=pid.Td/pid.T;				//Td/T ����΢��ʱ�䳣����������ڵĻ�
	Kid=pid.Kp*tDi;                 //�˹̶�����
	DOut=Kid*Eki;					//DOut
	out=POut+IOut+DOut+pid.Out0;	//out
	if(out>pid.PWMcycle)    		//�鲢
		pid.Out=pid.PWMcycle;
	if(out<0)
		pid.Out=pid.Out0;
	pid.Out=out;
	pid.C10ms=0;					//��10ms����������
	pid.Ek_1=pid.Ek;				//��ƫ��ֵ��Ϊ��һ�ε�ƫ�� ׼��������һ������	
}


void PID_Out(void)      //���pid������  ÿ1ms��һ��
{
	static u16 pw;
	pw++;
	if(pw>pid.PWMcycle)     //���ﵽ���ֵʱ����
		pw=0;
	//0->pid.PWMcycle-1
	if(pw>pid.Out)         //���
		LED0=1;
	else
		LED0=0;
		
}