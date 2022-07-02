#include"pid.h"

void PID_Init(float Sv,float Kp,float Ti,float T,float Td,float Out0)
{
	/*对pid参数进行初始化*/
	pid.Sv=Sv;
	pid.Kp=Kp;
	pid.Ti=Ti;
	pid.T=T;
	pid.Td=Td;
	pid.Out0=Out0;
}

void TIM3_Int_Init(void)  //10ms定时器初始化
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能
    
 	TIM3->ARR=100-1;  	//设定计数器自动重装值  计数100次为 10ms
	TIM3->PSC=8400-1;  	//预分频器	  10KHz的频率 
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//抢占1，子优先级3，组2		
}

void TIM3_IRQHandler(void) //10ms计数中断
{ 		    		  			    
	if(TIM3->SR&0X0001)      //溢出中断
		pid.C10ms++;	
	TIM3->SR&=~(1<<0);//清除中断标志位 	
}

void TIM5_Int_Init(u32 arr,u16 psc)  //PWM输出定时器初始化
{
	RCC->APB1ENR|=1<<3;   	//TIM5 时钟使能 
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//复用功能,下拉
	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF  
 	TIM5->ARR=10-1;  		//设定计数器自动重装值    1ms调节一次占空比
	TIM5->PSC=8400-1;  		//预分频器 				10KHz的频率
	TIM5->DIER|=1<<0;   	//允许更新中断	
	TIM5->CR1|=0x01;    	//使能定时器2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//抢占2，子优先级0，组2
}

void TIM5_IRQHandler(void) //1msPWM中断
{ 		    		  			    
	if(TIM3->SR&0X0001)      //溢出中断
		PID_Out(); 	
	TIM3->SR&=~(1<<0);//清除中断标志位 	
}

void PID_Calc(void)
{
	/*位置式PID*/
	float ti,Kii,Di,tDi,Kid,Eki;   //定义中间值
	float out,POut,IOut,DOut;			//定义各部分的输出
	if(pid.C10ms<(pid.T/10))        //若不到计算周期 跳出
	{
		return ;
	}
	pid.Ek=pid.Sv-pid.Pv;          //计算本次偏差值
	Eki=pid.Ek-pid.Ek_1;			//计算本次偏差值和上一次偏差值的差值
	pid.SEk+=pid.Ek;				//历史偏差值累加
	ti=pid.T/pid.Ti;				//T/Ti 计算周期与积分时间常数的积
	Kii=ti*pid.Kp;					//乘固定比例
	IOut=Kii*pid.SEk;				//IOut
	POut=pid.Kp*pid.Ek;				//POut
	tDi=pid.Td/pid.T;				//Td/T 计算微分时间常数与计算周期的积
	Kid=pid.Kp*tDi;                 //乘固定比例
	DOut=Kid*Eki;					//DOut
	out=POut+IOut+DOut+pid.Out0;	//out
	if(out>pid.PWMcycle)    		//归并
		pid.Out=pid.PWMcycle;
	if(out<0)
		pid.Out=pid.Out0;
	pid.Out=out;
	pid.C10ms=0;					//将10ms计数器归零
	pid.Ek_1=pid.Ek;				//将偏差值置为上一次的偏差 准备接收下一个数据	
}


void PID_Out(void)      //输出pid到负载  每1ms加一次
{
	static u16 pw;
	pw++;
	if(pw>pid.PWMcycle)     //当达到最大值时清零
		pw=0;
	//0->pid.PWMcycle-1
	if(pw>pid.Out)         //输出
		LED0=1;
	else
		LED0=0;
		
}