/**
  ******************************************************************************
  * @file    mpu6050.c
  * @author  LiuYang
  * @brief   mpu6050������
  ******************************************************************************
  * @attention
  *	 
  ******************************************************************************
  */
#include "mpu6050.h"

UsartRec_t  mpu6050_rec =	{0};//���ڽ��� DMA��������
static MPU6050_Raw_Msg_t  mpu6050_raw =	{0};//mpu6050 ԭʼ����
/**
  * @funNm		MPU6050_Init
  * @brief		����DMA �����жϳ�ʼ��
  * @param		void
  * @retval		void
*/
void MPU6050_Init(void)
{
    __HAL_UART_ENABLE_IT(&MPU6050_USART, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&MPU6050_USART, mpu6050_rec.RX_pData, sizeof(mpu6050_rec.RX_pData));
    __HAL_UART_CLEAR_IDLEFLAG(&MPU6050_USART);
}

/**
  * @funNm		MPU6050_unpack
  * @brief		mpu6050 ���ݰ�����
  * @param		buf	�����ݵ�ַ; len	�����ݳ���
  * @retval		void
*/
void MPU6050_unpack(uint8_t* buf, uint16_t len)
{
    static uint8_t  Buf1[11], Buf2[11], Buf3[11];

    memcpy(Buf1, buf, 11);
    memcpy(Buf2, buf + 11, 11);
    memcpy(Buf3, buf + 22, 11);
    if(Buf1[0] == 0x55 &&  Buf2[0] == 0x55 &&   Buf3[0] == 0x55)
    {
        uint8_t Sum1 = 0, Sum2 = 0, Sum3 = 0;
        for(uint8_t i = 0; i < 10 ; i++)
        {
            Sum1 += Buf1[i];
            Sum2 += Buf2[i];
            Sum3 += Buf3[i];
        }
        if(Sum1 == Buf1[10]  && Sum2 == Buf2[10] && Sum3 == Buf3[10])
        {
						//���ٶ�
            mpu6050_raw.acceleration[0] = (short)(Buf1[3] << 8 | Buf1[2]) / 32768.0 * 16;
            mpu6050_raw.acceleration[1] = (short)(Buf1[5] << 8 | Buf1[4]) / 32768.0 * 16;
            mpu6050_raw.acceleration[2] = (short)(Buf1[7] << 8 | Buf1[6]) / 32768.0 * 16;
						//���ٶ�
            mpu6050_raw.angular_velocity[0] = (short)(Buf2[3] << 8 | Buf2[2]) / 32768.0 * 2000;
            mpu6050_raw.angular_velocity[1] = (short)(Buf2[5] << 8 | Buf2[4]) / 32768.0 * 2000;
            mpu6050_raw.angular_velocity[2] = (short)(Buf2[7] << 8 | Buf2[6]) / 32768.0 * 2000;
						//�Ƕ�
            mpu6050_raw.Angle[0] = (short)(Buf3[3] << 8 | Buf3[2]) / 32768.0 * 180;
            mpu6050_raw.Angle[1] = (short)(Buf3[5] << 8 | Buf3[4]) / 32768.0 * 180;
            mpu6050_raw.Angle[2] = (short)(Buf3[7] << 8 | Buf3[6]) / 32768.0 * 180;
        }
    }
}

/**
  * @funNm		HAL_MPU_To_UART_Receive_IDLE
  * @brief		����3 DMA �����жϽ��պ���
  * @param		void
  * @retval		void
*/
void HAL_MPU_To_UART_Receive_IDLE(UART_HandleTypeDef *huart)
{
		static uint32_t temp;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if((__HAL_UART_GET_FLAG(&MPU6050_USART, UART_FLAG_IDLE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&MPU6050_USART);
        HAL_UART_DMAStop(&MPU6050_USART);
				temp = huart->hdmarx->Instance->CNDTR; 
        mpu6050_rec.RX_Size = RX_LEN - temp;
				mpu6050_rec.RX_flag = 1; 
			
        //�������з���
        if(MPU6050_Rec_Queue !=NULL){
					xQueueSendFromISR(MPU6050_Rec_Queue, &mpu6050_rec.RX_Size, &xHigherPriorityTaskWoken);
			 }
		}
}

/**
  * @funNm		MPU6050_ReadData
  * @brief		mpu6050ŷ�������ݶ�ȡ
  * @param		mpu6050_data	����������ָ��
  * @retval		void
*/
void MPU6050_ReadData(MPU6050Msg_t *mpu6050_data)
{
	 if(mpu6050_rec.RX_flag == 1)
	 {
			//mpu6050��̬�ǽ���
			MPU6050_unpack(mpu6050_rec.RX_pData, mpu6050_rec.RX_Size);
		
			//���DMA����
			memset(mpu6050_rec.RX_pData, '\0', RX_LEN);
		
			//��̬����	
			mpu6050_data->yaw = mpu6050_raw.Angle[2];
			mpu6050_data->roll = mpu6050_raw.Angle[1];
			mpu6050_data->pitch = mpu6050_raw.Angle[0];

			mpu6050_data->pitch_groy = mpu6050_raw.angular_velocity[0];
			mpu6050_data->roll_groy = mpu6050_raw.angular_velocity[1];
			mpu6050_data->yaw_groy = mpu6050_raw.angular_velocity[2];

			mpu6050_data->acc_pitch = mpu6050_raw.acceleration[0];
			mpu6050_data->acc_roll = mpu6050_raw.acceleration[1];
			mpu6050_data->acc_yaw = mpu6050_raw.acceleration[2];
		
			//����DMA����
			HAL_UART_Receive_DMA(&MPU6050_USART, mpu6050_rec.RX_pData, RX_LEN);
			mpu6050_rec.RX_flag = 0;
		}
}

/**
  * @funNm		MPU6050_Test_Demo
  * @brief		mpu6050����demo
  * @param		void
  * @retval		void
*/
MPU6050Msg_t  mpu6050_data;
void MPU6050_Test_Demo(void)
{
		//extern MPU6050Msg_t  mpu6050_data; //ʹ�õ���̬��
		//static MPU6050Msg_t  mpu6050_data =	{0}; //ʹ�õ���̬��
    MPU6050_Init();
    while(1)
    {
				MPU6050_ReadData(&mpu6050_data);
       // delay_ms(20);
    }
}
