#include "AD5933.h"
#include "delay.h"
#include "IIC.h"
#include "math.h"
#include "stdlib.h"
#define PI    3.141592654


/* AD5933 Instruct*********  ָ��****************************************************************/

uint8_t D_ADDR=0x1A;			//��ַ�Ĵ���
uint8_t SET_POINT=0xB0;   	//0xB0�����ʾд���ַָ��

uint8_t data_clk=0x80;		//��:0x80 ��0x00  ʱ��ѡ��
uint8_t data_gain=0x01;		//����	 1��:0x01  5����0x00
uint8_t data_Vpp=0x04;		//�������ֵ��0.2Vpp:0x02 0.4Vpp:0x04 1Vpp:0x06	 2Vpp:0x00

uint8_t data_AddH=0x00;		//Ƶ��������100Hz=0x000c80,10Hz=0x000140,1Hz=0x20
uint8_t data_AddM=0x0c;
uint8_t data_AddL=0x80;

uint8_t data_StartH=0x0F; 	//��ʼƵ��,10kHz=0x04E214   30kHz=0x 0f 00 00 ;
uint8_t data_StartM=0x00;		//1kHz=0x007d02,50kHz=ox186A64,35kHz=0x111764,10kHz=0x04E214
uint8_t data_StartL=0x00;

uint8_t data_CountFH=0x00;	//ɨ�����
uint8_t data_CountFL=10;

uint8_t data_CountTH=0x00;	//��ʱ������
uint8_t data_CountTL=0x3f;

float FreFlag;

uint8_t ReIm[6];	  			//���ݼĴ��� REH REL IMH IML   ���� ����
uint8_t ReIm10[10];


int Re_Result;
int Im_Result;
float Phase_Result;
float Impedance_Result;


void AD5933_StartTest( uint8_t Add_OK )
{
	if( Add_OK>0 )
	{
		AD5933_WriteByte( 0x80 , 0x30|data_Vpp|data_gain );    /*����Ƶ��ɨ�����*/	
	}
	else
	{
		AD5933_WriteByte( 0x80 , 0x40|data_Vpp|data_gain );    /*�����ٴβ���*/		
	}
}

float AD5933_ReadImpedance( ImpeType *AA )
{
	uint8_t j=0;
	while( (j&0x02)==0 )
	{
	  	AD5933_WriteByte( SET_POINT , 0x8f ); 		//����ָ��ָ��״̬�Ĵ���
		HAL_Delay(5);
	  	j =  AD5933_ReadByte();
	}
	AD5933_WriteByte( SET_POINT , 0x94 );		//ָ�����ݼĴ���

	AA->Re = AD5933_ReadByte();
	AA->Re <<= 8;
	AA->Re |= AD5933_ReadByte();
		
	AA->Im = AD5933_ReadByte();
	AA->Im <<= 8;
	AA->Im |= AD5933_ReadByte();	
	
	AA->Impedance = sqrt( ( (float)(abs(AA->Re) ) ) * ( (float)(abs(AA->Re) ) ) \
					  +   ( (float)( abs(AA->Im ) ) ) * ( (float)(abs(AA->Im) ) )  );

	if( (AA->Re>0)&&(AA->Im>0))	 		//��һ����
	{
		AA->Phase = atan( ((float)(AA->Im)) / ((float)(AA->Re))) ;	
	}
	else
	{
		if( (AA->Re<0)&&(AA->Im>0))		//�ڶ�����
		{
			AA->Phase = atan( ((float)(AA->Im)) / ((float)(AA->Re))) + PI;
		}
		else
		{
			if( (AA->Re<0)&&(AA->Im<0))	 //��������
			{
				AA->Phase = atan( ((float)(AA->Im)) / ((float)(AA->Re))) + PI;
			}
			else
			{
				if( (AA->Re>0)&&(AA->Im<0))	//��������
				{
					AA->Phase = atan( ((float)(AA->Im)) / ((float)(AA->Re)))+ 2*PI;
				}
			}
		}	
	}
	return 0;	
}


void AD5933_FreInit( float Fre ,float AddFre)	  			//����һ��Ƶ�ʵ��ɨ��
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
	  AD5933_WriteByte( 0x83 , data_StartM );  	//��ʼƵ��,Fmclk=16.776MHz					 
	  AD5933_WriteByte( 0x82 , data_StartH );   //start frequency��
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
	  AD5933_WriteByte( 0x85 , data_AddH );   	//Ƶ������
	  AD5933_WriteByte( 0x86 , data_AddM );   	
	  AD5933_WriteByte( 0x87 , data_AddL );	
	  
	  AD5933_WriteByte( 0x88 , data_CountFH );  //����������Ƶ�ʸ���
	  AD5933_WriteByte( 0x89 , data_CountFL );  
	
	  AD5933_WriteByte( 0x8A , data_CountTH );  //�ȴ�����������
	  AD5933_WriteByte( 0x8B , data_CountTL );
	  
	  AD5933_WriteByte( 0x80 , 0xB0|data_Vpp|data_gain );
	  AD5933_WriteByte( 0x81 , 0x10|data_clk );		//��λ
	  HAL_Delay(400);			//��λ��ʱ									   
	  AD5933_WriteByte( 0x81 , 0x00|data_clk );

 	  AD5933_WriteByte( 0x80 , 0x10|data_Vpp|data_gain );	/*����ʼƵ��ɨ��*/
	  HAL_Delay(100);			
 	  AD5933_WriteByte( 0x80 , 0x20|data_Vpp|data_gain );    /*����Ƶ��ɨ��*/
}

/*void AD5933_GetData( uint8_t Times )
{
	uint8_t i;
	uint8_t j;
	for( i=0 ; i<Times ; i++ )
	{
	  	AD5933_WriteByte( 0x80 , 0x40|data_Vpp|data_gain );    //����Ƶ��ɨ��	
	  	do
	  	{
	  		AD5933_WriteByte( SET_POINT , 0x8f ); 		//����ָ��ָ��״̬�Ĵ���
	  		i =  AD5933_ReadByte();
	  	}
	  	while( i&0x02==0 );	 						//�����ɱ�־�Ƿ���λ

		AD5933_WriteByte( SET_POINT , 0x94 );	//ָ�����ݼĴ���

		ReIm[i] = AD5933_ReadByte(); 		//��ȡ���ݼĴ���			
	}	
}*/ 

/*uint8_t Get_ReIm( void )	 //��ȡ���ݼĴ���
{
	uint8_t i;

	AD5933_WriteByte( SET_POINT , 0x8f ); 		//����ָ��ָ��״̬�Ĵ���
	i =  AD5933_ReadByte();

	if( i&0x02)	 								//�����ɱ�־�Ƿ���λ
	{
		AD5933_WriteByte( SET_POINT , 0x94 );	//ָ�����ݼĴ���
		for( i=0 ; i<4 ; i++)
		{
			ReIm[i] = AD5933_ReadByte(); 		//��ȡ���ݼĴ���
		}
	}
	else
	{
		return 0;
	}
	return 1;			//������Ч״̬
}*/


void AD5933_WriteByte( uint8_t RegOrIns  , uint8_t DataOrReg )	 //дһ���ֽڵ��Ĵ��� ����ָ��
{
	IIC_Start();
	IIC_Send_Byte( D_ADDR );
	IIC_Send_Byte( RegOrIns );		//�Ĵ���ָ������ָ��
	IIC_Send_Byte( DataOrReg );
	IIC_Stop();
}

uint8_t AD5933_ReadByte( void )  			//��һ���ֽ�		
{
	uint8_t Temp;
	IIC_Start();
	IIC_Send_Byte( D_ADDR|0x01 );		//���Ͷ���ַ
	Temp = 	IIC_Read_Byte(0);
	IIC_Stop();
	return( Temp );	
}

float Temperature_Test( void )
{
	uint8_t i;
	int  k=0;
	AD5933_WriteByte( 0x80 , 0x90|data_Vpp|data_gain );    //����Ƶ��ɨ�� |data_Vpp|data_gain

	do
	{
	    AD5933_WriteByte( SET_POINT , 0x8f ); 		//����ָ��ָ��״̬�Ĵ���
	  	i =  AD5933_ReadByte();
	 }
	 while( (i&0x01)==0 );	 						//�����ɱ�־�Ƿ���λ

	AD5933_WriteByte( SET_POINT , 0x92 );	//ָ���¶����ݼĴ���
	

	k = AD5933_ReadByte(); 		//��ȡ���ݼĴ���
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
