#include "dma.h"


//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,��Χ:0~7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ�����  
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_TypeDef *DMAx;
	u8 Streamx;
	if((u32)Streamx>(u32)DMA2)   //�ж�����DMA1����DMA2
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<21;  //ʹ��ʱ��
	}
	else
	{
		DMAx=DMA1;
		RCC->AHB1ENR|=1<<22;     //ʹ��ʱ��
	}
	while(DMA_Streamx->CR&0x01)
		;                             //�ȴ�DMA������
	//Streamx=(((u32)DMA_Streamx-(u32)DMAx)-0x01)/0x18; 
	Streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;	//ͨ����ַ���������
	/*���жϱ�־λ���*/
	if(Streamx>=6)
		DMAx->HIFCR|=0x3D<<(6*(Streamx-6)+16);
	else if(Streamx>=4)
		DMAx->HIFCR|=0x3D<<6*(Streamx-4);
	else if(Streamx>=2)
		DMAx->LIFCR|=0x3D<<(6*(Streamx-2)+16);
	else
		DMAx->LIFCR|=0x3D<<6*Streamx;
	/*����DMA*/
	DMA_Streamx->PAR=par;  //�����ַ
	DMA_Streamx->M0AR=mar;  //�洢��0��ַ
	DMA_Streamx->NDTR=ndtr;  //�洢��0��ַ
	DMA_Streamx->CR=0;      //��״̬�Ĵ�������
	DMA_Streamx->CR|=1<<6;   //����ģʽ
	DMA_Streamx->CR|=0<<8;  //ѭ��ģʽ
	DMA_Streamx->CR|=0<<9;   //�����ַ������
	DMA_Streamx->CR|=1<<10;   //�洢����ַ����
	DMA_Streamx->CR|=0<<11;    //�������ݴ�С
	DMA_Streamx->CR|=0<<13;    //�洢�����ݴ�С
	DMA_Streamx->CR|=1<<16;     //�е����ȼ�
	DMA_Streamx->CR|=0<<21;     //����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;    //�洢��ͻ�����δ���
	DMA_Streamx->CR|=(u32)chx<<25; //ͨ��ѡ��
	//DMA_Streamx->FCR|=0x21;     //FIFO ���ƼĴ���
}
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0);  //�ر�DMA����
	while(DMA_Streamx->CR&0x01)
		;                             //�ȴ�DMA������
	DMA_Streamx->NDTR=ndtr;     //�洢��0��ַ
	DMA_Streamx->CR|=1<<0;
	
}
