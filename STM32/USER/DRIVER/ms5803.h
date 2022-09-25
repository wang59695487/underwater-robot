#ifndef __MS5803_H
#define	__MS5803_H

#include "includes.h"
//压传
typedef struct
{
    int32_t Temperature;//Deep单位cm Temper 单位 摄氏度
    float depth;
}PressureMsg_t;

#define  MS5803_CS_High  HAL_GPIO_WritePin(MS5803_CS_GPIO_Port, MS5803_CS_Pin, GPIO_PIN_SET); //片选信号拉高
#define  MS5803_CS_Low   HAL_GPIO_WritePin(MS5803_CS_GPIO_Port, MS5803_CS_Pin, GPIO_PIN_RESET); //片选信号拉低

void SPI_START(void);
void Change_MS5803(void);//压传数据补偿公式 
void Read_P(void);	//读取压强
void Read_T(void);	//读取温度
unsigned char crc4(unsigned int n_prom[]);//CRC校验函数 
void SPI_treatment(unsigned long d11,unsigned long d22);//压传数据出错舍弃函数 
void MS5803aInit(void); //压传初始化
void MS5803_Read_Sometimes(void);	//间歇性采集压传数值 
void Get_Pressure(PressureMsg_t *ms5803); //获取压力
#endif 
