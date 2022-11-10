/************************************************************************************
						
*************************************************************************************/
#include "HX711.h"
#include "delay.h"

uint32_t HX711_Buffer;
uint32_t Weight_Maopi;
int32_t Weight_Shiwu;
uint8_t Flag_Error = 0;

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 475




//****************************************************
//读取HX711
//****************************************************
uint32_t HX711_Read(void)	//增益128
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
    count=count^0x800000;//第25个脉冲下降沿来时，转换数据
	DWT_Delay_us(1);
	HX711_SCK;  
	return(count);
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//称重
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = HX711_Read();
	if(HX711_Buffer > Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//获取实物的AD采样数值。
	
		Weight_Shiwu = (int32_t)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量																//因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。
																		//当发现测试出来的重量偏大时，增加该数值。
																		//如果测试出来的重量偏小时，减小改数值。
	}

	
}
