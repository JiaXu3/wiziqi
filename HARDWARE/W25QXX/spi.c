#include "spi.h"

void SPI1_Init(void)
{
	u16 tempreg=0;
	RCC->AHB1ENR|=1<<0;  //GPIOA使能
	RCC->AHB1ENR|=1<<1;
	RCC->APB2ENR|=1<<12;   //使能SPI1
	GPIO_Set(GPIOB,PIN3|PIN4|PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //复用 上拉
	GPIO_AF_Set(GPIOB,3,5); // 复用为SPI1
	GPIO_AF_Set(GPIOB,4,5); 
	GPIO_AF_Set(GPIOB,5,5);
	
	//针对SPI口初始化
	RCC->APB2RSTR|=1<<12;
	RCC->APB2RSTR&=~(1<<12);
	
	tempreg|=0<<10;  //全双工模式
	tempreg|=1<<9;   //软件NSS
	tempreg|=1<<8;    //内部从器件选择
	tempreg|=1<<2;     //SPI主机
	tempreg|=0<<11;			//8位数据格式
	tempreg|=1<<1;     //CPOL=1 CPHA=1
	tempreg|=1<<0;
	tempreg|=7<<3;     //256分频
	tempreg|=0<<7;     //高有效位开始发送
	tempreg|=1<<6;      //SPI启动
	
	SPI1->CR1=tempreg;
	SPI1->I2SCFGR&=~(0<<11);  //选择SPI模式
	SPI1_ReadWriteByte(0xff);
	
	
}



// SpeedSet 0~7

void SPI1_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0x07;
	SPI1->CR1&=0xFFC7;
	SPI1->CR1|=SpeedSet<<3;   //设置
	SPI1->CR1|=1<<6;       //使能SPI
		
}



//读写一个字节
//TxDate 要发送的字节
//返回接收到的数据
u8 SPI1_ReadWriteByte(u8 TxDate)
{
	while((SPI1->SR&1<<1)==0)       //等待发送缓冲区清空
		;
	SPI1->DR=TxDate;         //发送一个字节的数据
	while((SPI1->SR&1<<0)==0) //等待接收完成
		;
	return SPI1->DR;          //返回接收到的数据
	
}












