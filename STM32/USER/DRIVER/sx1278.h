#ifndef __SX1278_H
#define __SX1278_H

#include "includes.h"
 
typedef enum
{
	SX1278_GET_WORK_STA,
  SX1278_GET_VERSION,
	SX1278_RESET,
} SX1278_Parse_State_t; 
typedef struct  
{          
	uint16_t address;           
	uint16_t baud;  
	float    speed;  
	uint8_t  channel;
}SX1278_Info_t;   

void SX1278_Init(void);	//LORAģ���ʼ��,�����ù�������
void SX1278_unpackData(uint8_t *buf,u16 len);//
void SX1278_SendData(uint8_t buf[],u16 len);//���ͺ���
void HAL_LORA_To_UART_Receive_IDLE(UART_HandleTypeDef *huart);//LORA���ڽ��պ���
void SX1278_Test_Demo(void);	// Lora���Գ���
#endif