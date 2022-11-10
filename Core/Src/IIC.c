#include "IIC.h"
#include "delay.h"

//初始化IIC
void IIC_Init(void)
{					     
// 	RCC->APB2ENR|=1<<3;							 
//	GPIOC->MODER&=0XFFF00FFF;
//	GPIOC->MODER|=0X00033000;	   
//	GPIOC->ODR|=3<<11;
GPIO_Set(GPIOC,PIN10|PIN11,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PF9,PF10设置	
}
//GPIO通用设置 
//GPIOx:GPIOA~GPIOI.
//BITx:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.比如0X0101,代表同时设置Px0和Px8.
//MODE:0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入.
//OTYPE:0/1;输出类型选择,0,推挽输出;1,开漏输出.
//OSPEED:0~3;输出速度设置,0,2Mhz;1,25Mhz;2,50Mhz;3,100Mh. 
//PUPD:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
//注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效!!
void GPIO_Set(GPIO_TypeDef* GPIOx,uint32_t BITx,uint32_t MODE,uint32_t OTYPE,uint32_t OSPEED,uint32_t PUPD)
{  
	uint32_t pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//一个个位检查 
		curpin=BITx&pos;//检查引脚是否要设置
		if(curpin==pos)	//需要设置
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->MODER|=MODE<<(pinpos*2);	//设置新的模式 
			if((MODE==0X01)||(MODE==0X02))	//如果是输出模式/复用功能模式
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//清除原来的设置
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//设置新的速度值  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//清除原来的设置
				GPIOx->OTYPER|=OTYPE<<pinpos;		//设置新的输出模式
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//设置新的上下拉
		}
	}
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  
	DWT_Delay_us(4);	
	IIC_SCL=1;
	DWT_Delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	DWT_Delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
	DWT_Delay_us(4);
}	 

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出

	IIC_SDA=0;
 	DWT_Delay_us(6); 
	IIC_SCL=1;//STOP:when CLK is high DATA change form low to high
 	DWT_Delay_us(6); 
	IIC_SDA=1;//发送I2C总线结束信号 		
	DWT_Delay_us(6); 
	IIC_SCL=0;
	DWT_Delay_us(6); 
}


//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	DWT_Delay_us(6);
	IIC_SCL=1;
	DWT_Delay_us(6);
	IIC_SCL=0;
}

//不产生ACK应答		    
void IIC_NAck(void)
{
	SDA_OUT();
	IIC_SDA=1;
	DWT_Delay_us(6);
	IIC_SCL=1;
	DWT_Delay_us(6);
	IIC_SCL=0;
	DWT_Delay_us(6);
}	

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		DWT_Delay_us(6);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		DWT_Delay_us(6); 
		IIC_SCL=0;	
		DWT_Delay_us(6);
    }	
	IIC_SDA=1;
	DWT_Delay_us(6);
	IIC_SCL=1;
	DWT_Delay_us(6);
	IIC_SCL=0;
	DWT_Delay_us(6);	
} 	    

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
		receive<<=1;
        
		IIC_SCL=1;
        
        if(READ_SDA)receive++;   
		DWT_Delay_us(6); 
		IIC_SCL=0; 
		DWT_Delay_us(6);
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



























