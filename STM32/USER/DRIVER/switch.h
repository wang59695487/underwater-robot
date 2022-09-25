#ifndef __SWITCH_H
#define __SWITCH_H

#include "includes.h"

#define POWER_ON     1 //开
#define POWER_OFF    0 //关
#define POWER_KEY(x)    (x?HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET))
//函数声明
void PowerON_SwitchSelfHold(void);//电源开关 
u8 PowerKeyState(void);	//电源开关状态 
void SW_Test_Demo(void);//开关机测试
#endif
