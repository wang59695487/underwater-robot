#ifndef __MPU6050_H
#define __MPU6050_H

#include "includes.h" 

typedef struct
{
	float acceleration[3];		//���ٶ�
	float angular_velocity[3];//���ٶ�
	float Angle[3];						//�Ƕ�
	uint8_t	state;
	int	freq;	
}MPU6050_Raw_Msg_t;

typedef struct
{
	float	pitch;	//������
	float	roll;		//�����
  float	yaw;		//�����
  
  float pitch_groy;
  float roll_groy;
  float yaw_groy;
        
  float acc_pitch;
  float acc_yaw;
  float acc_roll;
}MPU6050Msg_t;


//��������
void MPU6050_unpack(uint8_t* buf,uint16_t len);//mpu6050 ���ݰ�����
void MPU6050_Init(void);//MPU6050����DMA �����жϳ�ʼ��
void HAL_MPU_To_UART_Receive_IDLE(UART_HandleTypeDef *huart);//MPU6050��Ӧ���� DMA �����жϽ��պ���
void MPU6050_ReadData(MPU6050Msg_t *mpu6050_data);//mpu6050ŷ�������ݶ�ȡ
void MPU6050_Test_Demo(void);//mpu6050����demo
#endif
