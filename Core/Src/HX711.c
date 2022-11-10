/************************************************************************************
						
*************************************************************************************/
#include "HX711.h"
#include "delay.h"

uint32_t HX711_Buffer;
uint32_t Weight_Maopi;
int32_t Weight_Shiwu;
uint8_t Flag_Error = 0;

//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
#define GapValue 475




//****************************************************
//��ȡHX711
//****************************************************
uint32_t HX711_Read(void)	//����128
{
	unsigned long count; 
	unsigned char i; 
  LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_4); 
	DWT_Delay_us(1);
  LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_5);
	DWT_Delay_us(1);	
  count=0; 
  while(HX711_DOUT); 
  for(i=0;i<24;i++)
	{ 
			DWT_Delay_us(1);
	  	HX711_SCK; 
	  	count=count<<1; 
			DWT_Delay_us(1);
			HX711_SCK; 
	  	if(HX711_DOUT)
			count++; 
		DWT_Delay_us(1);
	} 
 	HX711_SCK; 
    count=count^0x800000;//��25�������½�����ʱ��ת������
	DWT_Delay_us(1);
	HX711_SCK;  
	return(count);
}

//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//����
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = HX711_Read();
	if(HX711_Buffer > Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//��ȡʵ���AD������ֵ��
	
		Weight_Shiwu = (int32_t)((float)Weight_Shiwu/GapValue); 	//����ʵ���ʵ������																//��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������GapValue���������
																		//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
																		//������Գ���������ƫСʱ����С����ֵ��
	}

	
}
