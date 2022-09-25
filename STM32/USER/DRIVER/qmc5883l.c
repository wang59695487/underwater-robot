#include "qmc5883l.h"
#include "myiic.h"

/**
  * @funNm   qmc_write_reg
  * @brief   qmc5883l�Ĵ���д�뺯��
  * @param	 reg �Ĵ�����ַ��data�Ĵ�������
  * @retval  void
*/
void qmc_write_reg(u8 reg,u8 data)
{
	IIC_Start();
	IIC_Send_Byte(WRITE_ADDRESS);
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	IIC_Send_Byte(data);
	IIC_Wait_Ack();
	IIC_Stop();
	//delay_ms(5);
}
/**
  * @funNm   qmc_read_reg
  * @brief   qmc5883l�Ĵ�����ȡ����
  * @param	 reg �Ĵ�����ַ
  * @retval  data �Ĵ���������
*/
u8 qmc_read_reg(u8 reg)
{
	u8 data;
	IIC_Start();
	IIC_Send_Byte(WRITE_ADDRESS);
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	IIC_Stop();
	IIC_Start();
	IIC_Send_Byte(READ_ADDRESS);
	IIC_Wait_Ack();
//	data=IIC_Read_Byte();
//	IIC_NAck();
	data=IIC_Read_Byte(0);
	IIC_Stop();
	return data;
}

/**
  * @funNm   qmc_read_XYZ
  * @brief   ������ȡXYZ����ų�����
  * @param	 *data ���ݴ���ָ��
  * @retval  void
*/
void qmc_read_XYZ(short int *data)
{
	*data++=((int16_t)qmc_read_reg(DATAX_M)<<8)|qmc_read_reg(DATAX_L);
	*data++=((int16_t)qmc_read_reg(DATAY_M)<<8)|qmc_read_reg(DATAY_L);
	*data++=((int16_t)qmc_read_reg(DATAZ_M)<<8)|qmc_read_reg(DATAZ_L);
}

/**
  * @funNm   qmc_init
  * @brief   �����Ƴ�ʼ������
  * @param	 void
  * @retval  void
*/
void qmc_init(void)
{
	qmc_write_reg(0X0B,0x01);
	qmc_write_reg(0X20,0x40);
	qmc_write_reg(0X21,0x01);
	qmc_write_reg(CONFIGA,0x1D);
	delay_ms(10);
}

/**
  * @funNm		MS5803_Test_Demo
  * @brief		MS5803����demo
  * @param		void
  * @retval		void
*/
short int data[3];
double angle,x,y,z;
void QMC5883L_Test_Demo(void)
{
	qmc_init();
	while(1)
	{
		qmc_read_XYZ(data);
		//angle=atan2((double)data[0],(double)data[2])*(180 / 3.14159265)+180;
		x=(double)data[0];
		y=(double)data[1];
		z=(double)data[2];
		delay_ms(100);
	}
}
