/**
  ******************************************************************************
  * @file    myiic.c
  * @author  LiuYang
  * @brief   iic
  ******************************************************************************
  * @attention
  *	 
	*  
	* 
  ******************************************************************************
  */
#include "myiic.h"

/**
 * @funNm : SDA_Out
 * @brief : ���ģʽ
 * @param : void
 * @retval: void
 */
void SDA_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = QMC5883L_IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QMC5883L_IIC_PORT, &GPIO_InitStruct);	
}

/**
 * @funNm : SDA_In
 * @brief : ����ģʽ
 * @param : void
 * @retval: void
 */
void SDA_In(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = QMC5883L_IIC_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QMC5883L_IIC_PORT, &GPIO_InitStruct);
}

/**
  * @funNm  IIC_Start
  * @brief  ����IIC��ʼ�ź�
  * @param	void
  * @retval void
*/
void IIC_Start(void)
{
	SDA_Out();  //sda�����
	SDA(1);	  	  
	SCL(1);
	delay_sys_us(4);
 	SDA(0);  //START:when CLK is high,DATA change form high to low 
	delay_sys_us(4);
	SCL(0);  //ǯסI2C���ߣ�׼�����ͻ�������� 
}

/**
  * @funNm  IIC_Stop
  * @brief  ����IICֹͣ�ź�
  * @param	void
  * @retval void
*/
void IIC_Stop(void)
{
	SDA_Out();//sda�����
	SCL(0);
	SDA(0);  //STOP:when CLK is high DATA change form low to high
 	delay_sys_us(4);
	SCL(1); 
	SDA(1);  //����I2C���߽����ź�
	delay_sys_us(4);							   	
}

/**
  * @funNm  IIC_Wait_Ack
  * @brief  �ȴ�Ӧ���źŵ���
  * @param  void
  * @retval 1������Ӧ��ʧ��
            0������Ӧ��ɹ�
*/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_In();  //SDA����Ϊ����  
	SDA(1);delay_sys_us(1);	   
	SCL(1);delay_sys_us(1);	 
	while(READ_SDA())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	SCL(0);  //ʱ�����0 	   
	return 0;  
} 

/**
  * @funNm  IIC_Ack
  * @brief  ����ACKӦ��
  * @param	void
  * @retval void
*/
void IIC_Ack(void)
{
	SCL(0);
	SDA_Out();
	SDA(0);
	delay_sys_us(2);
	SCL(1);
	delay_sys_us(2);
	SCL(0);
}

/**
  * @funNm  IIC_AIIC_NAckck
  * @brief  ������ACKӦ��
  * @param	void
  * @retval void
*/
void IIC_NAck(void)
{
	SCL(0);
	SDA_Out();
	SDA(1);
	delay_sys_us(2);
	SCL(1);
	delay_sys_us(2);
	SCL(0);
}					 				     

/**
  * @funNm   IIC_AIIC_NAckck
  * @brief   IIC����һ���ֽ�
  * @param	 �ӻ�����Ӧ��
  * @retval  1����Ӧ��
             0����Ӧ��
*/
void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	SDA_Out(); 	    
	SCL(0);//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{              
		SDA((txd&0x80)>>7);
		txd<<=1; 	  
		delay_sys_us(2);   //��TEA5767��������ʱ���Ǳ����
		SCL(1);
		delay_sys_us(2); 
		SCL(0);	
		delay_sys_us(2);
	}	 
} 	    

/**
  * @funNm  IIC_Read_Byte
  * @brief  IIC��1���ֽ�
  * @param  ack=1ʱ������ACK��ack=0������nACK
  * @retval  receive
*/ 
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_In();//SDA����Ϊ����
	for(i=0;i<8;i++ )
	{
		SCL(0); 
		delay_sys_us(2);
		SCL(1);
		receive<<=1;
		if(READ_SDA())receive++;   
		delay_sys_us(1); 
  }					 
	if (!ack)
			IIC_NAck();//����nACK
	else
			IIC_Ack(); //����ACK   
	return receive;
}


