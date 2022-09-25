#ifndef __SENSOR_TASK_H
#define __SENSOR_TASK_H

#include "includes.h"

typedef struct {
    PressureMsg_t   Pressure_Data; //ѹ��
    MPU6050Msg_t    MPU6050_Data;  //mpu6050
    TempHumiMsg_t   TempHum_Data;  //��ʪ��
    BattMsg_t 			Batt_Data; //��ѹ
} SensorMsg_t;
extern SensorMsg_t SensorData;
void sensor_TaskFunction(void *pvParameters);
#endif
