#ifndef __PWM_H
#define __PWM_H
#include "sys.h"
#define LED0_PWM_VALUE TIM14->CCR1
#define LED0_PWM_ARR TIM14->ARR

void PWM14_CH1_Init(u32 psc,u32 arr);
void TIM2_CH1_Cap_Init(u32 arr,u16 psc);


#endif 