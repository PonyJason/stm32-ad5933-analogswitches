#ifndef __HX711_H
#define __HX711_H

#include "sys.h"
#include "main.h"

#define HX711_SCK LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_5)// 
#define HX711_DOUT LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_4) 
//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数


extern void Init_HX711pin(void);
extern uint32_t HX711_Read(void);
extern void Get_Maopi(void);
extern void Get_Weight(void);

//extern uint32_t HX711_Buffer;
//extern uint32_t Weight_Maopi;
extern int32_t Weight_Shiwu;
//extern uint8_t Flag_Error;

#endif

