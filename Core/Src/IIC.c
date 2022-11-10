#include "IIC.h"
#include "delay.h"

//��ʼ��IIC
void IIC_Init(void)
{					     
// 	RCC->APB2ENR|=1<<3;							 
//	GPIOC->MODER&=0XFFF00FFF;
//	GPIOC->MODER|=0X00033000;	   
//	GPIOC->ODR|=3<<11;
GPIO_Set(GPIOC,PIN10|PIN11,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PF9,PF10����	
}
//GPIOͨ������ 
//GPIOx:GPIOA~GPIOI.
//BITx:0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,��������.����0X0101,����ͬʱ����Px0��Px8.
//MODE:0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ������.
//OTYPE:0/1;�������ѡ��,0,�������;1,��©���.
//OSPEED:0~3;����ٶ�����,0,2Mhz;1,25Mhz;2,50Mhz;3,100Mh. 
//PUPD:0~3:����������,0,����������;1,����;2,����;3,����.
//ע��:������ģʽ(��ͨ����/ģ������)��,OTYPE��OSPEED������Ч!!
void GPIO_Set(GPIO_TypeDef* GPIOx,uint32_t BITx,uint32_t MODE,uint32_t OTYPE,uint32_t OSPEED,uint32_t PUPD)
{  
	uint32_t pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//һ����λ��� 
		curpin=BITx&pos;//��������Ƿ�Ҫ����
		if(curpin==pos)	//��Ҫ����
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->MODER|=MODE<<(pinpos*2);	//�����µ�ģʽ 
			if((MODE==0X01)||(MODE==0X02))	//��������ģʽ/���ù���ģʽ
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//���ԭ��������
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//�����µ��ٶ�ֵ  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//���ԭ��������
				GPIOx->OTYPER|=OTYPE<<pinpos;		//�����µ����ģʽ
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//�����µ�������
		}
	}
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  
	DWT_Delay_us(4);	
	IIC_SCL=1;
	DWT_Delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	DWT_Delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	DWT_Delay_us(4);
}	 

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����

	IIC_SDA=0;
 	DWT_Delay_us(6); 
	IIC_SCL=1;//STOP:when CLK is high DATA change form low to high
 	DWT_Delay_us(6); 
	IIC_SDA=1;//����I2C���߽����ź� 		
	DWT_Delay_us(6); 
	IIC_SCL=0;
	DWT_Delay_us(6); 
}


//����ACKӦ��
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

//������ACKӦ��		    
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

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		DWT_Delay_us(6);   //��TEA5767��������ʱ���Ǳ����
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

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



























