#ifndef __MPU6050_H
#define __MPU6050_H

#include "includes.h" 

typedef struct
{
	float acceleration[3];		//加速度
	float angular_velocity[3];//角速度
	float Angle[3];						//角度
	uint8_t	state;
	int	freq;	
}MPU6050_Raw_Msg_t;

typedef struct
{
	float	pitch;	//俯仰角
	float	roll;		//横滚角
  float	yaw;		//航向角
  
  float pitch_groy;
  float roll_groy;
  float yaw_groy;
        
  float acc_pitch;
  float acc_yaw;
  float acc_roll;
}MPU6050Msg_t;


//函数声明
void MPU6050_unpack(uint8_t* buf,uint16_t len);//mpu6050 数据包解析
void MPU6050_Init(void);//MPU6050串口DMA 空闲中断初始化
void HAL_MPU_To_UART_Receive_IDLE(UART_HandleTypeDef *huart);//MPU6050对应串口 DMA 空闲中断接收函数
void MPU6050_ReadData(MPU6050Msg_t *mpu6050_data);//mpu6050欧拉角数据读取
void MPU6050_Test_Demo(void);//mpu6050测试demo
#endif
