#ifndef __ADC_H
#define __ADC_H
#define ADC1_CH5 5
#include "sys.h"

void ADC_Init(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);
short Get_Temprate(void);
void ADC3_Init(void);
u16 Get_Adc3_Val(u8 ch);

#endif

