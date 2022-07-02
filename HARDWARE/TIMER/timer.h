#ifndef __TIMER_H
#define __TIMER_H
#include"sys.h"
#define LED0_PWM_VALUE TIM14->CCR1
#define LED0_PWM_ARR TIM14->ARR
#define PWM_DAC_Value TIM9->CCR2

void TIM3_Int_Init(u16 psc,u16 arr);
void PWM14_CH1_Init(u32 psc,u32 arr);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
void TIM9_CH2_PWM_Init(u16 arr,u16 psc);

#endif

