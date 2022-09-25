/**
  ******************************************************************************
  * @file    led.c
  * @author  LiuYang
  * @brief   led�������
  ******************************************************************************
  * @attention
  *	 
	*  
	* 
  ******************************************************************************
  */
#include "led.h"

/**
 * @funNm : LED_SW
 * @brief : LED �������
 * @param : sta : 0(��) 1(��)
 * @retval: void
 */
void LED_SW(u8 sta)
{
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,(sta?GPIO_PIN_SET:GPIO_PIN_RESET));
}

/**
 * @funNm : LED_Show
 * @brief : LED��˸
 * @param : void
 * @retval: void
 */
void LED_Show(void)
{	

		LED_SW(1);
		delay_ms(500);
		LED_SW(0);
		delay_ms(500);
		LED_SW(1);
		delay_ms(500);
	  LED_SW(0);
}

/**
 * @funNm : LED_Test_Demo
 * @brief : LED����demo��LED��˸
 * @param : void
 * @retval: void
 */
void LED_Test_Demo(void)
{	
	while(1)
	{
		LED_SW(0);
		delay_ms(500);
		LED_SW(1);
		delay_ms(500);
	}

}

