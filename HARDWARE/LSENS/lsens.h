#ifndef __LSENS_H
#define __LSENS_H

#define LSENS_READ_TIMES 10
#define LSENS_ADC_CHX 5
#include "sys.h"
#include "adc.h"

void Lsens_Init(void);
u8 Get_Lsens_Value(void);

#endif