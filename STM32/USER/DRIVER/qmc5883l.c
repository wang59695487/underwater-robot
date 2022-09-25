#include "qmc5883l.h"
#include "myiic.h"

/**
  * @funNm   qmc_write_reg
  * @brief   qmc5883l寄存器写入函数
  * @param	 reg 寄存器地址；data寄存器数据
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
  * @brief   qmc5883l寄存器读取函数
  * @param	 reg 寄存器地址
  * @retval  data 寄存器内数据
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
  * @brief   连续读取XYZ三轴磁场数据
  * @param	 *data 数据存入指针
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
  * @brief   磁力计初始化函数
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
  * @brief		MS5803测试demo
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
