#include "Compass_Task.h"
#include "Sensor_Task.h"
// #define DEBUG
static int MPU6050_Rec_Length = 0;
MPU6050Msg_t mpu6050_raw;
//extern int led_flag;

void imu_Task_Function(void const * argument)
{
//	portTickType tick = xTaskGetTickCount();
    while(1)
    {
#ifdef DEBUG
//				if(led_flag == 1)
//				{
//					osDelay(35000);
//					led_flag = 1;
//					osDelay(1000);
//					led_flag = 1;
//					osDelay(2000);
//					led_flag = 1;
//				}
#else
        if(xQueueReceive(MPU6050_Rec_Queue, &MPU6050_Rec_Length, portMAX_DELAY)) //portMAX_DELAY
        {
            MPU6050_unpack(mpu6050_rec.RX_pData,mpu6050_rec.RX_Size);//mpu6050��̬�ǽ���
            memset(mpu6050_rec.RX_pData,'\0',sizeof(mpu6050_rec.RX_pData));				 //���DMA����
            HAL_UART_Receive_DMA(&MPU6050_USART,mpu6050_rec.RX_pData,sizeof(mpu6050_rec.RX_pData));//����ʹ��DMA����
            MPU6050_ReadData(&SensorData.MPU6050_Data);												 //��̬����
            xQueueOverwrite(MPU6050_Msg_Queue,&SensorData.MPU6050_Data);//���������ݴ�����������
        }
#endif
//    osDelay(100);
//		vTaskDelayUntil(&tick,200);
    }
}
