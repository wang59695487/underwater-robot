/**
  ******************************************************************************
  * @file    HMC5883L.c
  * @author  SunZhiyang
  * @brief   ������
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
  IIC_Send_Byte(HMC5883L_Write_Address);  //д����
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x00);  //ָ��ָ��00�����üĴ���A
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x70);	//�����������ƽ����Ϊ8��ȱʡ���������������Ϊ15Hz������ģʽΪĬ�ϵ�������������
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x01);  //ָ��ָ��01�����üĴ���B
  IIC_Wait_Ack();  
  IIC_Send_Byte(0xe0);  //��������Ϊ230���Ƽ��������ų���ΧΪ(-8.1Ga��+8,1Ga)
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x02);  //ָ��ָ��02������ģʽ�Ĵ���
  IIC_Wait_Ack();  
  IIC_Send_Byte(0x01);  //����Ϊ��һ����ģʽ
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
  IIC_Send_Byte(HMC5883L_Read_Address);  //���ݶ�ȡ��ַ
  IIC_Wait_Ack();  
  for(i=0;i<6;i++)  //������ȡ��������
  {  
    if(i==5)  
    {  
      a[i] = IIC_Read_Byte(0); //ack=0������nACK 
    }  
    else  
    {  
      a[i] = IIC_Read_Byte(1); //ack=1������ACK 
    }  
  }  
  IIC_Stop();  
  x=a[0];  //�Զ���ԭʼ���ݵĴ����ȸ�λ�ٵ�λ��ԭʼ����Ϊ������ʽ
  x=x<<8;  
  x=x|a[1];  
  z=a[2];  
  z=z<<8;  
  z=z|a[3]; 
  y=a[4];  
  y=y<<8;  
  y=y|a[5];  
  if(x>32768)	//���뵽ԭ���ת��
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
  Curent_Angle = (atan2(Y,X) * (180 / 3.14159265) + 180);  //�Ƕ���Ϣˮƽ����
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