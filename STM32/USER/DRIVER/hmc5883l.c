/**
  ******************************************************************************
  * @file    HMC5883L.c
  * @author  SunZhiyang
  * @brief   磁力计
  ******************************************************************************
  * @attention
  *	 
	*  
	* 
  ******************************************************************************
  */
	
#include "hmc5883l.h"
#include "myiic.h"

void HMC5883L_Init()  
{  
  IIC_Start();  
  IIC_Send_Byte(HMC5883L_Write_Address);  //写操作
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x00);  //指针指向00，配置寄存器A
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x70);	//测量输出采样平均数为8（缺省），数据输出速率为15Hz，测量模式为默认的正常测量配置
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x01);  //指针指向01，配置寄存器B
  IIC_Wait_Ack();  
  IIC_Send_Byte(0xe0);  //配置增益为230；推荐传感器磁场范围为(-8.1Ga到+8,1Ga)
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x02);  //指针指向02，配置模式寄存器
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x01);  //配置为单一测量模式
  IIC_Stop();  
} 



float Get_Current_Angle()  
{  
  u8 i;  
  u8 a[6];
	int x,y,z,X,Y,Z;  
  float Curent_Angle;  
  HMC5883L_Init();  
  IIC_Start();  
  IIC_Send_Byte(HMC5883L_Write_Address);  
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x03); 
  IIC_Wait_Ack();  
    
  IIC_Start();  
  IIC_Send_Byte(HMC5883L_Read_Address);  //数据读取地址
  IIC_Wait_Ack();  
  for(i=0;i<6;i++)  //连续读取六组数据
  {  
    if(i==5)  
    {  
      a[i] = IIC_Read_Byte(0); //ack=0，发送nACK 
    }  
    else  
    {  
      a[i] = IIC_Read_Byte(1); //ack=1，发送ACK 
    }  
  }  
  IIC_Stop();  
  x=a[0];  //对多组原始数据的处理，先高位再低位，原始数据为补码形式
  x=x<<8;  
  x=x|a[1];  
  z=a[2];  
  z=z<<8;  
  z=z|a[3]; 
  y=a[4];  
  y=y<<8;  
  y=y|a[5];  
  if(x>32768)	//补码到原码的转换
  {  
    x = -(0xFFFF - x + 1);  
  }  
    
  if(y>32768)  
  {  
    y = -(0xFFFF - y + 1);  
  }  
  if(z>32768)  
  {  
    z = -(0xFFFF - z + 1);  
  }  
  X = x; 
  Y = y;  
  Z = z;  
  Curent_Angle = (atan2(Y,X) * (180 / 3.14159265) + 180);  //角度信息水平面上
  return Curent_Angle;  
} 

void HMC5883L_Test_Demo(void)
{
	HMC5883L_Init();
	while(1)
	{
		Get_Current_Angle();
		delay_ms(100);
	}
}