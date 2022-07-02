#ifndef __HCSR04_H
#define __HCSR04_H

#include "sys.h"
#define Echo PAout(4)

void HCSR04_Init(u32 arr,u16 psc);
u8 HCSR04_Read(void);

#endif