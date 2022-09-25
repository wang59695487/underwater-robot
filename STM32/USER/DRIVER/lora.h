#ifndef __LORA_H
#define __LORA_H
#include "includes.h"

//lora使能引脚配置
#define M0SET(x)   (x?HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(LORA_M0_GPIO_Port, LORA_M0_Pin, GPIO_PIN_RESET))
#define M1SET(x)   (x?HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(LORA_M1_GPIO_Port, LORA_M1_Pin, GPIO_PIN_RESET))

//lora结构体
typedef struct {
    u8 frame;
    u8 addr;
    u8 lenth;
    u8 data;
} LORA_SET;

//函数声明
void lora_init(u16 id, u8 chain);
void Lora_SendData(u8* pbData,u16 pnDataLen);
void HAL_LORA_To_UART_Receive_IDLE(UART_HandleTypeDef *huart);
#endif
