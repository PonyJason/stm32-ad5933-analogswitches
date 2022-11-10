#include "AD5933.h"
#include "delay.h"
#include "IIC.h"
#include "math.h"
#include "stdlib.h"
#define PI    3.141592654


/* AD5933 Instruct*********  指令****************************************************************/

uint8_t D_ADDR=0x1A;			//地址寄存器
uint8_t SET_POINT=0xB0;   	//0xB0命令表示写入地址指令

uint8_t data_clk=0x80;		//外:0x80 内0x00  时钟选择
uint8_t data_gain=0x01;		//增益	 1倍:0x01  5倍：0x00
uint8_t data_Vpp=0x04;		//激励峰峰值：0.2Vpp:0x02 0.4Vpp:0x04 1Vpp:0x06	 2Vpp:0x00

uint8_t data_AddH=0x00;		//频率增量，100Hz=0x000c80,10Hz=0x000140,1Hz=0x20
uint8_t data_AddM=0x0c;
uint8_t data_AddL=0x80;

uint8_t data_StartH=0x0F; 	//起始频率,10kHz=0x04E214   30kHz=0x 0f 00 00 ;
uint8_t data_StartM=0x00;		//1kHz=0x007d02,50kHz=ox186A64,35kHz=0x111764,10kHz=0x04E214
uint8_t data_StartL=0x00;

uint8_t data_CountFH=0x00;	//扫描点数
uint8_t data_CountFL=10;

uint8_t data_CountTH=0x00;	//延时周期数
uint8_t data_CountTL=0x3f;

float FreFlag;

uint8_t ReIm[6];	  			//数据寄存器 REH REL IMH IML   符号 结束
uint8_t ReIm10[10];


int Re_Result;
int Im_Result;
float Phase_Result;
float Impedance_Result;


void AD5933_StartTest( uint8_t Add_OK )
{
	if( Add_OK>0 )
	{
		AD5933_WriteByte( 0x80 , 0x30|data_Vpp|data_gain );    /*启动频率扫描测量*/	
	}
	else
	{
		AD5933_WriteByte( 0x80 , 0x40|data_Vpp|data_gain );    /*启动再次测量*/		
	}
}

float AD5933_ReadImpedance( ImpeType *AA )
{
	uint8_t j=0;
	while( (j&0x02)==0 )
	{
	  	AD5933_WriteByte( SET_POINT , 0x8f ); 		//数据指针指向状态寄存器
		HAL_Delay(5);
	  	j =  AD5933_ReadByte();
	}
	AD5933_WriteByte( SET_POINT , 0x94 );		//指向数据寄存器

	AA->Re = AD5933_ReadByte();
	AA->Re <<= 8;
	AA->Re |= AD5933_ReadByte();
		
	AA->Im = AD5933_ReadByte();
	AA->Im <<= 8;
	AA->Im |= AD5933_ReadByte();	
	
	AA->Impedance = sqrt( ( (float)(abs(AA->Re) ) ) * ( (float)(abs(AA->Re) ) ) \
					  +   ( (float)( abs(AA->Im ) ) ) * ( (float)(abs(AA->Im) ) )  );

	if( (AA->Re>0)&&(AA->Im>0))	 		//第一项限
	{
		AA->Phase = atan( ((float)(AA->Im)) / ((float)(AA->Re))) ;	
	}
	else
	{
		if( (AA->Re<0)&&(AA->Im>0))		//第二项限
		{
			AA->Phase = atan( ((float)(AA->Im)) / ((float)(AA->Re))) + PI;
		}
		else
		{
			if( (AA->Re<0)&&(AA->Im<0))	 //第三项限
			{
				AA->Phase = atan( ((float)(AA->Im)) / ((float)(AA->Re))) + PI;
			}
			else
			{
				if( (AA->Re>0)&&(AA->Im<0))	//第四项限
				{
					AA->Phase = atan( ((float)(AA->Im)) / ((float)(AA->Re)))+ 2*PI;
				}
			}
		}	
	}
	return 0;	
}


void AD5933_FreInit( float Fre ,float AddFre)	  			//启动一个频率点的扫描
{
	  float StartHz;

	  AD5933_WriteByte( 0x80 , 0xA0|data_Vpp|data_gain );

	  if( Fre != 0 )
	  {
		  StartHz = Fre*4.0*134217728.0/AD5933Fre;
		  data_StartH = (unsigned long)(StartHz)/65536;
		  data_StartM = (unsigned long)(StartHz)%65536/256;
		  data_StartL = (unsigned long)(StartHz)/256;
	  }
	  AD5933_WriteByte( 0x83 , data_StartM );  	//起始频率,Fmclk=16.776MHz					 
	  AD5933_WriteByte( 0x82 , data_StartH );   //start frequency，
	  AD5933_WriteByte( 0x84 , data_StartL );  	//100kHz=0x30D4C8

	  if( AddFre != 0 )
	  {
		  StartHz = AddFre*4.0*134217728.0/AD5933Fre;
		  data_AddH = (unsigned long)(StartHz)/65536;
		  data_AddM = (unsigned long)(StartHz)%65536/256;
		  data_AddL = (unsigned long)(StartHz)/256;
	  }
	  else
	  {
	  		data_AddH = 0;	
			data_AddM = 0;
			data_AddL = 0;
	  }
	  AD5933_WriteByte( 0x85 , data_AddH );   	//频率增量
	  AD5933_WriteByte( 0x86 , data_AddM );   	
	  AD5933_WriteByte( 0x87 , data_AddL );	
	  
	  AD5933_WriteByte( 0x88 , data_CountFH );  //测量点数或频率个数
	  AD5933_WriteByte( 0x89 , data_CountFL );  
	
	  AD5933_WriteByte( 0x8A , data_CountTH );  //等待建立周期数
	  AD5933_WriteByte( 0x8B , data_CountTL );
	  
	  AD5933_WriteByte( 0x80 , 0xB0|data_Vpp|data_gain );
	  AD5933_WriteByte( 0x81 , 0x10|data_clk );		//复位
	  HAL_Delay(400);			//复位延时									   
	  AD5933_WriteByte( 0x81 , 0x00|data_clk );

 	  AD5933_WriteByte( 0x80 , 0x10|data_Vpp|data_gain );	/*以起始频率扫描*/
	  HAL_Delay(100);			
 	  AD5933_WriteByte( 0x80 , 0x20|data_Vpp|data_gain );    /*启动频率扫描*/
}

/*void AD5933_GetData( uint8_t Times )
{
	uint8_t i;
	uint8_t j;
	for( i=0 ; i<Times ; i++ )
	{
	  	AD5933_WriteByte( 0x80 , 0x40|data_Vpp|data_gain );    //启动频率扫描	
	  	do
	  	{
	  		AD5933_WriteByte( SET_POINT , 0x8f ); 		//数据指针指向状态寄存器
	  		i =  AD5933_ReadByte();
	  	}
	  	while( i&0x02==0 );	 						//检测完成标志是否置位

		AD5933_WriteByte( SET_POINT , 0x94 );	//指向数据寄存器

		ReIm[i] = AD5933_ReadByte(); 		//读取数据寄存器			
	}	
}*/ 

/*uint8_t Get_ReIm( void )	 //读取数据寄存器
{
	uint8_t i;

	AD5933_WriteByte( SET_POINT , 0x8f ); 		//数据指针指向状态寄存器
	i =  AD5933_ReadByte();

	if( i&0x02)	 								//检测完成标志是否置位
	{
		AD5933_WriteByte( SET_POINT , 0x94 );	//指向数据寄存器
		for( i=0 ; i<4 ; i++)
		{
			ReIm[i] = AD5933_ReadByte(); 		//读取数据寄存器
		}
	}
	else
	{
		return 0;
	}
	return 1;			//返回有效状态
}*/


void AD5933_WriteByte( uint8_t RegOrIns  , uint8_t DataOrReg )	 //写一个字节到寄存器 或者指令
{
	IIC_Start();
	IIC_Send_Byte( D_ADDR );
	IIC_Send_Byte( RegOrIns );		//寄存器指针设置指令
	IIC_Send_Byte( DataOrReg );
	IIC_Stop();
}

uint8_t AD5933_ReadByte( void )  			//读一个字节		
{
	uint8_t Temp;
	IIC_Start();
	IIC_Send_Byte( D_ADDR|0x01 );		//发送读地址
	Temp = 	IIC_Read_Byte(0);
	IIC_Stop();
	return( Temp );	
}

float Temperature_Test( void )
{
	uint8_t i;
	int  k=0;
	AD5933_WriteByte( 0x80 , 0x90|data_Vpp|data_gain );    //启动频率扫描 |data_Vpp|data_gain

	do
	{
	    AD5933_WriteByte( SET_POINT , 0x8f ); 		//数据指针指向状态寄存器
	  	i =  AD5933_ReadByte();
	 }
	 while( (i&0x01)==0 );	 						//检测完成标志是否置位

	AD5933_WriteByte( SET_POINT , 0x92 );	//指向温度数据寄存器
	

	k = AD5933_ReadByte(); 		//读取数据寄存器
	k <<= 8;
	k += AD5933_ReadByte();
	if( k<8192 )
	{
		return( (float)((float)(k))/32.0);	
	}
	else
	{
		return( ((float)(k-16384))/32.0);	
	}

}
