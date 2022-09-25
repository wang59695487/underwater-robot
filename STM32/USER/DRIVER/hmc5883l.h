#ifndef __HMC5883L_H
#define __HMC5883L_H 
#include "includes.h" 

#define			HMC5883L_Write_Address		0x3c
#define			HMC5883L_Read_Address		0x3d

typedef struct
{
    int32_t Temperature;//Deep单位cm Temper 单位 摄氏度
    float depth;
}MagneticMsg_t;

void HMC5883L_Init();
float Get_Current_Angle();
void HMC5883L_Test_Demo(void);
#endif