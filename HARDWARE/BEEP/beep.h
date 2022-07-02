#ifndef __BEEP_H
#define __BEEP_H
#include"sys.h"
#define BEEP PFout(8)

void BEEP_Init(void);
void BEEP_Run(u8 time);


#endif

