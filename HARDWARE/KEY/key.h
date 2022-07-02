#ifndef __KEY_h
#define __KEY_h
#include"sys.h"
#define KEY0 PEin(4)
#define KEY1 PEin(3)
#define KEY2 PEin(2)
#define KEY3 PAin(0)
void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
