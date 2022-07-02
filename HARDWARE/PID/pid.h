#ifndef __PID_H
#define __PID_H

#include"sys.h"
/*����Ҫ����pid���Ƶ����ţ��˴���LED��PF9��Ϊ��*/
#define LED0 PFout(9)

typedef struct PID
{
	float Sv;         //�û����õ�Ŀ��ֵ
	float Pv;         //����ֵ
	
	float Ek;         //���ε�ƫ��ֵ
	float Ek_1;       //�ϴε�ƫ��ֵ
	float SEk;        //��ʷƫ��ֵ֮��
	
	//�û��ɵ�����
	float Kp;         //��������
	float Ti;         //����ʱ�䳣��
	float T;          //����ʱ��--����ʱ��--��������
	float Td;         //΢��ʱ�䳣��	
	float Out0;       //���ڳ���
	
	float Out;        //���ֵ
	u16 C10ms;        //ʮ������ ��ʱ���жϼ��� ������������ڱȶ�
	u16 PWMcycle;      //PWM������
}PID;
PID pid;
void PID_Init(float Sv,float Kp,float Ti,float T,float Td,float Out0); //PID��ʼ��
void PID_Calc(void); //PID����
void TIM3_Int_Init(void);  //��ʼ��ʮ���������ʱ������TIM3Ϊ����
void PWM14_CH1_Init(u32 psc,u32 arr);  //��ʼ��PWM�������TIM14_CH1Ϊ����
void PID_Out(void);    //PID���


#endif