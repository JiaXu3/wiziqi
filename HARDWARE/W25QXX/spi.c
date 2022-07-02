#include "spi.h"

void SPI1_Init(void)
{
	u16 tempreg=0;
	RCC->AHB1ENR|=1<<0;  //GPIOAʹ��
	RCC->AHB1ENR|=1<<1;
	RCC->APB2ENR|=1<<12;   //ʹ��SPI1
	GPIO_Set(GPIOB,PIN3|PIN4|PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //���� ����
	GPIO_AF_Set(GPIOB,3,5); // ����ΪSPI1
	GPIO_AF_Set(GPIOB,4,5); 
	GPIO_AF_Set(GPIOB,5,5);
	
	//���SPI�ڳ�ʼ��
	RCC->APB2RSTR|=1<<12;
	RCC->APB2RSTR&=~(1<<12);
	
	tempreg|=0<<10;  //ȫ˫��ģʽ
	tempreg|=1<<9;   //���NSS
	tempreg|=1<<8;    //�ڲ�������ѡ��
	tempreg|=1<<2;     //SPI����
	tempreg|=0<<11;			//8λ���ݸ�ʽ
	tempreg|=1<<1;     //CPOL=1 CPHA=1
	tempreg|=1<<0;
	tempreg|=7<<3;     //256��Ƶ
	tempreg|=0<<7;     //����Чλ��ʼ����
	tempreg|=1<<6;      //SPI����
	
	SPI1->CR1=tempreg;
	SPI1->I2SCFGR&=~(0<<11);  //ѡ��SPIģʽ
	SPI1_ReadWriteByte(0xff);
	
	
}



// SpeedSet 0~7

void SPI1_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0x07;
	SPI1->CR1&=0xFFC7;
	SPI1->CR1|=SpeedSet<<3;   //����
	SPI1->CR1|=1<<6;       //ʹ��SPI
		
}



//��дһ���ֽ�
//TxDate Ҫ���͵��ֽ�
//���ؽ��յ�������
u8 SPI1_ReadWriteByte(u8 TxDate)
{
	while((SPI1->SR&1<<1)==0)       //�ȴ����ͻ��������
		;
	SPI1->DR=TxDate;         //����һ���ֽڵ�����
	while((SPI1->SR&1<<0)==0) //�ȴ��������
		;
	return SPI1->DR;          //���ؽ��յ�������
	
}












