#ifndef __SWITCH_H
#define __SWITCH_H

#include "includes.h"

#define POWER_ON     1 //��
#define POWER_OFF    0 //��
#define POWER_KEY(x)    (x?HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET))
//��������
void PowerON_SwitchSelfHold(void);//��Դ���� 
u8 PowerKeyState(void);	//��Դ����״̬ 
void SW_Test_Demo(void);//���ػ�����
#endif
