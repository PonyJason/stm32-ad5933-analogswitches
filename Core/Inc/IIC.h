#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
  		   
//IO方向设置
#define SDA_IN()  {GPIOC->MODER&=~(3<<(10*2));GPIOC->MODER|=0<<10*2;}
#define SDA_OUT() {GPIOC->MODER&=~(3<<(10*2));GPIOC->MODER|=1<<10*2;}

//IO操作函数	 
#define IIC_SCL    PCout(11) //SCL
#define IIC_SDA    PCout(10) //SDA	 
#define READ_SDA   PCin(10)  //输入SDA 

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号
void GPIO_Set(GPIO_TypeDef* GPIOx,uint32_t BITx,uint32_t MODE,uint32_t OTYPE,uint32_t OSPEED,uint32_t PUPD);
#endif
















