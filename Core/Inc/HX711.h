#ifndef __HX711_H
#define __HX711_H

#include "sys.h"
#include "main.h"

#define HX711_SCK LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_5)// 
#define HX711_DOUT LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_4) 
//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��


extern void Init_HX711pin(void);
extern uint32_t HX711_Read(void);
extern void Get_Maopi(void);
extern void Get_Weight(void);

//extern uint32_t HX711_Buffer;
//extern uint32_t Weight_Maopi;
extern int32_t Weight_Shiwu;
//extern uint8_t Flag_Error;

#endif

