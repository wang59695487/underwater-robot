#ifndef __BOARD_H
#define __BOARD_H
#include "includes.h"
//定义串口

#define   SX1278_USART        huart3
#define   lora_USART        huart3
#define   PI_USART          huart2
#define		MPU6050_USART			huart1
#define		MS5803_SPI		   	hspi3	//hspi1

#ifdef FREERTOS_SYS
	#define GET_TICK_COUNT()    xTaskGetTickCount()     
#else
	#define GET_TICK_COUNT()    HAL_GetTick()
#endif
//串口接收长度
#define RX_LEN 256  
//串口接收结构体
typedef struct  
{  
	uint8_t  RX_flag:1;        //IDLE receive flag
	uint16_t RX_Size;          //receive length
	uint8_t  RX_pData[RX_LEN]; //DMA receive buffer
}UsartRec_t;  
extern UsartRec_t	mpu6050_rec;
//重定义
extern QueueHandle_t MPU6050_Rec_Queue;
extern QueueHandle_t MPU6050_Msg_Queue;
extern QueueHandle_t Control_Message_Queue ;  
extern QueueHandle_t Command_Message_Queue ;  
extern QueueHandle_t Pressure_Message_Queue; 
extern QueueHandle_t TempHum_Message_Queue ;
extern QueueHandle_t Battery_Message_Queue ; 
//函数声明
void Task_Queue_Semaphore_Timers_Create(void);//消息队列创建
void delay_ms(int nms);	//ms级延迟函数
void delay_sys_us(uint32_t Delay);  //us延时函数
void BSP_Init(void);	//所有外设的初始化
void USER_UART_IRQHandler(UART_HandleTypeDef *huart); //串口中断函数
#endif
