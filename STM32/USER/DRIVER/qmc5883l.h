#ifndef __QMC5883L_H
#define __QMC5883L_H

//#include "stm32f10x.h"
#include "includes.h" 

#define WRITE_ADDRESS   0x1a
#define READ_ADDRESS    0x1b

#define CONFIGA         0x09
#define CONFIGB         0x09
#define MODE            0x09

#define DATAX_L         0x00
#define DATAX_M         0x01
#define DATAY_L         0x02
#define DATAY_M         0x03
#define DATAZ_L         0x04
#define DATAZ_M         0x05

void qmc_write_reg(u8 reg,u8 data);//qmc5883l寄存器写入函数
u8 qmc_read_reg(u8 reg);					 //qmc5883l寄存器读取函数
void qmc_read_XYZ(short int *data);//连续读取XYZ三轴磁场数据
void qmc_init(void);							 //磁力计初始化函数
void QMC5883L_Test_Demo(void);		 //MS5803测试demo
#endif

