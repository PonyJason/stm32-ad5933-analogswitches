#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
  		   
//IO��������
#define SDA_IN()  {GPIOC->MODER&=~(3<<(10*2));GPIOC->MODER|=0<<10*2;}
#define SDA_OUT() {GPIOC->MODER&=~(3<<(10*2));GPIOC->MODER|=1<<10*2;}

//IO��������	 
#define IIC_SCL    PCout(11) //SCL
#define IIC_SDA    PCout(10) //SDA	 
#define READ_SDA   PCin(10)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�
void GPIO_Set(GPIO_TypeDef* GPIOx,uint32_t BITx,uint32_t MODE,uint32_t OTYPE,uint32_t OSPEED,uint32_t PUPD);
#endif
















