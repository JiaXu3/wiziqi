#include "dma.h"


//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,范围:0~7
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量  
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_TypeDef *DMAx;
	u8 Streamx;
	if((u32)Streamx>(u32)DMA2)   //判断属于DMA1还是DMA2
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<21;  //使能时钟
	}
	else
	{
		DMAx=DMA1;
		RCC->AHB1ENR|=1<<22;     //使能时钟
	}
	while(DMA_Streamx->CR&0x01)
		;                             //等待DMA可配置
	//Streamx=(((u32)DMA_Streamx-(u32)DMAx)-0x01)/0x18; 
	Streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;	//通过地址计算流编号
	/*将中断标志位清除*/
	if(Streamx>=6)
		DMAx->HIFCR|=0x3D<<(6*(Streamx-6)+16);
	else if(Streamx>=4)
		DMAx->HIFCR|=0x3D<<6*(Streamx-4);
	else if(Streamx>=2)
		DMAx->LIFCR|=0x3D<<(6*(Streamx-2)+16);
	else
		DMAx->LIFCR|=0x3D<<6*Streamx;
	/*配置DMA*/
	DMA_Streamx->PAR=par;  //外设地址
	DMA_Streamx->M0AR=mar;  //存储器0地址
	DMA_Streamx->NDTR=ndtr;  //存储器0地址
	DMA_Streamx->CR=0;      //将状态寄存器清零
	DMA_Streamx->CR|=1<<6;   //传输模式
	DMA_Streamx->CR|=0<<8;  //循环模式
	DMA_Streamx->CR|=0<<9;   //外设地址非增量
	DMA_Streamx->CR|=1<<10;   //存储器地址增量
	DMA_Streamx->CR|=0<<11;    //外设数据大小
	DMA_Streamx->CR|=0<<13;    //存储器数据大小
	DMA_Streamx->CR|=1<<16;     //中等优先级
	DMA_Streamx->CR|=0<<21;     //外设突发单次传输
	DMA_Streamx->CR|=0<<23;    //存储器突发单次传输
	DMA_Streamx->CR|=(u32)chx<<25; //通道选择
	//DMA_Streamx->FCR|=0x21;     //FIFO 控制寄存器
}
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0);  //关闭DMA传输
	while(DMA_Streamx->CR&0x01)
		;                             //等待DMA可配置
	DMA_Streamx->NDTR=ndtr;     //存储器0地址
	DMA_Streamx->CR|=1<<0;
	
}
