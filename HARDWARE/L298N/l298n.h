#ifndef __L298N_H
#define __L298N_H
#define A1 PCout(2)  //A   1 ��
#define B1 PCout(3)  //B  2  ��
#define A2 PCout(4)  //~A   3  ��
#define B2 PCout(5)  //~B  4  ��
#include "sys.h"

void Motor_Init(void);
void Motor_Run(u8 modek,u16 step,u16 us);  //1˳ʱ�� 0��ʱ��

#endif

