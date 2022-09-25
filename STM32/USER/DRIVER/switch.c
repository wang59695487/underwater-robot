/**
  ******************************************************************************
  * @file    switch.c
  * @author  LiuYang
  * @brief   电源开关 
  ******************************************************************************
  * @attention
  *	 
	*  
	* 
  ******************************************************************************
  */
#include "switch.h"

/**
 * @funNm : PowerON_SwitchSelfHold
 * @brief : 电源开关 
 * @param : 0: 关机   1: 开机
 * @retval: void
 */
void PowerON_SwitchSelfHold(void)
{
	POWER_KEY(1);
	delay_ms(50);
}
	
/**
 * @funNm : PowerKeyState
 * @brief : 电源开关状态 
 * @param : 0: 已上电  1: 已断电
 * @retval: void
 */
u8 PowerKeyState(void)
{
		u8 res = 0;
		if(HAL_GPIO_ReadPin(POWER_KEY_STATE_GPIO_Port,POWER_KEY_STATE_Pin) == GPIO_PIN_SET)
		{
			res = 1;
		}
		return res;
}

/**
 * @funNm : SW_Test_Demo
 * @brief : 开关机测试
 * @param : void
 * @retval: void
 */
u8 power_key_sta;
void SW_Test_Demo(void)
{
		//extern u8 power_key_sta;
		while(1)
		{
			power_key_sta = PowerKeyState();
			POWER_KEY(POWER_ON);
			delay_ms(100);
		}
}

