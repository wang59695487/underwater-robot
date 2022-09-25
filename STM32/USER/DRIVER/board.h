#ifndef __BOARD_H
#define __BOARD_H
#include "includes.h"
//���崮��

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
//���ڽ��ճ���
#define RX_LEN 256  
//���ڽ��սṹ��
typedef struct  
{  
	uint8_t  RX_flag:1;        //IDLE receive flag
	uint16_t RX_Size;          //receive length
	uint8_t  RX_pData[RX_LEN]; //DMA receive buffer
}UsartRec_t;  
extern UsartRec_t	mpu6050_rec;
//�ض���
extern QueueHandle_t MPU6050_Rec_Queue;
extern QueueHandle_t MPU6050_Msg_Queue;
extern QueueHandle_t Control_Message_Queue ;  
extern QueueHandle_t Command_Message_Queue ;  
extern QueueHandle_t Pressure_Message_Queue; 
extern QueueHandle_t TempHum_Message_Queue ;
extern QueueHandle_t Battery_Message_Queue ; 
//��������
void Task_Queue_Semaphore_Timers_Create(void);//��Ϣ���д���
void delay_ms(int nms);	//ms���ӳٺ���
void delay_sys_us(uint32_t Delay);  //us��ʱ����
void BSP_Init(void);	//��������ĳ�ʼ��
void USER_UART_IRQHandler(UART_HandleTypeDef *huart); //�����жϺ���
#endif
