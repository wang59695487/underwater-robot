/**
  ******************************************************************************
  * @file    pwm.c
  * @author  LiuYang
  * @brief   PWM
  ******************************************************************************
  * @attention
  *	 
	*  
	* 
  ******************************************************************************
  */
#include "pwm.h"

////摄像头使用
//void PWM_ServoInit(void)
//{
////	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
////	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,SERVO_INIT);
//}
//int GetAngle = 0;
//void SetCameraAngle(int value)
//{
////	GetAngle = value;
////	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,(GetAngle-50)*6+SERVO_INIT);
//}

/**
 * @funNm : PWM_Init
 * @brief : PWM初始化
 * @param : void
 * @retval: void
 */
void PWM_Init(void)
{
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,MIDValue);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,MIDValue);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,MIDValue);//  水平  左推进器
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,MIDValue);
		
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
}

/**
 * @funNm : PWM_Test_Demo
 * @brief : PWM测试demo
 * @param : void
 * @retval: void
 */
void PWM_Test_Demo(void)
{
		static uint16_t test_pwm = 1600;//MIDValue;
		PWM_Init();
		while(1)
		{
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,test_pwm);//3   上下
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,test_pwm);//4   前后
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,test_pwm);
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,test_pwm);
			
			HAL_Delay(500);
		}
}


