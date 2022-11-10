#ifndef AD5933_H
#define AD5933_H

#include "sys.h"
#define ErrorIns 0x55
#define AD5933Fre 16000000


typedef  struct ImpedanceType
{
	short 	 Re;
	short    Im;
	float    Impedance;
	float    Phase;
}ImpeType;

extern uint8_t D_ADDR;
extern uint8_t SET_POINT;   //0xB0命令表示写入地址
extern uint8_t data_clk;		//默认为内部时钟
extern uint8_t data_gain;		//默认为X1增益
extern uint8_t data_Vpp;		//默认0.2V Vpp
extern uint8_t data_AddH;		//频率增量，100Hz=0x000c80,
extern uint8_t data_AddM;
extern uint8_t data_AddL;
extern uint8_t data_StartH; 	//起始频率,10kHz=0x04E214
extern uint8_t data_StartM;
extern uint8_t data_StartL;
extern uint8_t data_CountFH;	//扫描点数
extern uint8_t data_CountFL;
extern uint8_t data_CountTH;	//延时周期数
extern uint8_t data_CountTL;

extern uint8_t ReIm[6];	 	//数据寄存器 REH REL IMH IML

extern int Re_Result;
extern int Im_Result;
extern float Phase_Result;
extern float Impedance_Result;

extern void  AD5933_WriteByte( uint8_t RegOrIns  , uint8_t DataOrReg );
extern uint8_t AD5933_ReadByte( void );    
extern void  AD5933_FreInit( float Fre ,float AddFre);	  	

void AD5933_StartTest( uint8_t Add_OK );
float AD5933_ReadImpedance( ImpeType *AA );


float Temperature_Test( void );//测温度


#endif


