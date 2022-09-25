/**
  ******************************************************************************
  * @file    switch.c
  * @author  LiuYang
  * @brief   ��Դ���� 
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
 * @brief : ��Դ���� 
 * @param : 0: �ػ�   1: ����
 * @retval: void
 */
void PowerON_SwitchSelfHold(void)
{
	POWER_KEY(1);
	delay_ms(50);
}
	
/**
 * @funNm : PowerKeyState
 * @brief : ��Դ����״̬ 
 * @param : 0: ���ϵ�  1: �Ѷϵ�
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
 * @brief : ���ػ�����
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

