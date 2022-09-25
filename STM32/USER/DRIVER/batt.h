#ifndef __BATT_H
#define __BATT_H

#include "includes.h"
 //定义滤波大小
#define BAT_V_FILTER_N		( 100 )
typedef struct
{
	float  voltage;		//温度
}BattMsg_t;
//函数声明
void Batt_Init(void);	//电压采集初始化
void Get_Batt_Data(BattMsg_t *Batt);	//传递电压值，低电压警报
void Batt_Test_Demo(void);//电压采集测试Demo

#endif
