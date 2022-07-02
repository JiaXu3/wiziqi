#ifndef __PID_H
#define __PID_H

#include"sys.h"
/*设置要用作pid调制的引脚，此处以LED（PF9）为例*/
#define LED0 PFout(9)

typedef struct PID
{
	float Sv;         //用户设置的目标值
	float Pv;         //测量值
	
	float Ek;         //本次的偏差值
	float Ek_1;       //上次的偏差值
	float SEk;        //历史偏差值之和
	
	//用户可调参数
	float Kp;         //比例常数
	float Ti;         //积分时间常数
	float T;          //控制时间--采样时间--计算周期
	float Td;         //微分时间常数	
	float Out0;       //调节常数
	
	float Out;        //输出值
	u16 C10ms;        //十毫秒数 定时器中断计数 用于与计算周期比对
	u16 PWMcycle;      //PWM的周期
}PID;
PID pid;
void PID_Init(float Sv,float Kp,float Ti,float T,float Td,float Out0); //PID初始化
void PID_Calc(void); //PID计算
void TIM3_Int_Init(void);  //初始化十毫秒计数定时器（以TIM3为例）
void PWM14_CH1_Init(u32 psc,u32 arr);  //初始化PWM输出（以TIM14_CH1为例）
void PID_Out(void);    //PID输出


#endif