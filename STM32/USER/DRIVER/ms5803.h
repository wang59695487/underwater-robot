#ifndef __MS5803_H
#define	__MS5803_H

#include "includes.h"
//ѹ��
typedef struct
{
    int32_t Temperature;//Deep��λcm Temper ��λ ���϶�
    float depth;
}PressureMsg_t;

#define  MS5803_CS_High  HAL_GPIO_WritePin(MS5803_CS_GPIO_Port, MS5803_CS_Pin, GPIO_PIN_SET); //Ƭѡ�ź�����
#define  MS5803_CS_Low   HAL_GPIO_WritePin(MS5803_CS_GPIO_Port, MS5803_CS_Pin, GPIO_PIN_RESET); //Ƭѡ�ź�����

void SPI_START(void);
void Change_MS5803(void);//ѹ�����ݲ�����ʽ 
void Read_P(void);	//��ȡѹǿ
void Read_T(void);	//��ȡ�¶�
unsigned char crc4(unsigned int n_prom[]);//CRCУ�麯�� 
void SPI_treatment(unsigned long d11,unsigned long d22);//ѹ�����ݳ����������� 
void MS5803aInit(void); //ѹ����ʼ��
void MS5803_Read_Sometimes(void);	//��Ъ�Բɼ�ѹ����ֵ 
void Get_Pressure(PressureMsg_t *ms5803); //��ȡѹ��
#endif 
