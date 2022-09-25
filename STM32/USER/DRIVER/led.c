/**
  ******************************************************************************
  * @file    led.c
  * @author  LiuYang
  * @brief   led灯组控制
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
 * @brief : LED 灯组控制
 * @param : sta : 0(关) 1(开)
 * @retval: void
 */
void LED_SW(u8 sta)
{
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,(sta?GPIO_PIN_SET:GPIO_PIN_RESET));
}

/**
 * @funNm : LED_Show
 * @brief : LED闪烁
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
 * @brief : LED测试demo，LED闪烁
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

