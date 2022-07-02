#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"

#define SDA_IN() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<(9*2);}
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<(9*2);}

#define IIC_SCL  PBout(8)
#define IIC_SDA  PBout(9)
#define READ_SDA  PBin(9)

void IIC_Init(void);					//初始化IIC的IO口
void IIC_Start(void);					//起始信号
void IIC_Stop(void);					//终止信号
void IIC_Send_Byte(u8 txd);				//发送一个字节
u8 IIC_Read_Byte(unsigned char ack);	//读取一个字节
u8 IIC_Wait_Ack(void);				//等待ACK信号
void IIC_Ack(void);						//发送ACK信号
void IIC_NAck(void);					//发送NACK信号


#endif
