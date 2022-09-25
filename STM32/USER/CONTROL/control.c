/**
  ******************************************************************************
  * @file    control.c
  * @author  LiuYang
  * @brief   control
  ******************************************************************************
  * @attention
	* 
	*
  ******************************************************************************
  */
#include "control.h"

/**
 * @funNm : SetUpDowm
 * @brief : 垂直方向电机 速度设置
 * @param : front_val：前方电机油门值	  back_val：后方油门值值
 * @retval: void
 */
void SetUpDowm(float front_val,float back_val)//min--max 升降运动
{
		int16_t testupdown_motor = 0;	//60
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,  testupdown_motor + MIDValue  + front_val);//2	前 
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,  testupdown_motor + MIDValue  - back_val);//4 	后
	
	
//    // Added by Li Wenwen, 2021,4,12
//	  //前后电机的死区临界值
//	  int16_t deadzone_front_up = 1550;
//	  int16_t deadzone_front_down = 1450;
//	  int16_t deadzone_back_up = 1550;
//	  int16_t deadzone_back_down = 1450;
//    if(front_val>0)
//		{
//		  testupdown_motor = deadzone_front_up - MIDValue;
//		}
//		else
//		{
//			testupdown_motor = deadzone_front_down - MIDValue;
//		}
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,  testupdown_motor + MIDValue  + front_val);//前
//		if(back_val>0)
//		{
//			testupdown_motor = deadzone_back_down - MIDValue;
//		}
//		else
//		{
//			testupdown_motor = deadzone_back_up - MIDValue;
//		}
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,  testupdown_motor + MIDValue  - back_val);//4 	后

} 

/**
 * @funNm : SetHorizontal
 * @brief : 水平方向电机 速度设置
* @param : f_b:前进后退参数  rotate：旋转参数
 * @retval: void
 */
void SetHorizontal(float f_b, float rotate)//水平运动	
{
	float temp_l=  f_b + MIDValue  + rotate;
	float temp_r= -f_b + MIDValue  + rotate ;
		
	//控制方案一
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,temp_l);//1
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,temp_r);//3
}

/**
 * @funNm : Clear_Control_Disable_Counter
 * @brief : 清除控制计数器
 * @param : void
 * @retval: void
 */
u16 xCounterPi_To_Stm32 = 5;
void Clear_Control_Disable_Counter(void)
{
	xCounterPi_To_Stm32 = 0;
}

/**
 * @funNm : Get_Control_Disable_Counter_State
 * @brief : 获取控制计数器
 * @param : void
 * @retval:boolvoid
 */
bool Get_Control_Disable_Counter_State(void)
{
	bool ret;
	if(xCounterPi_To_Stm32 >= 5){
		ret = false;
	}else{
		ret = true;
	}
	return ret;
}

/**
 * @funNm : Adder_Control_Disable_Counter
 * @brief : 控制计数器
 * @param : void
 * @retval: void
 */
void Adder_Control_Disable_Counter(void)
{
	if(xCounterPi_To_Stm32>=5){
		xCounterPi_To_Stm32 = 5;
	}else{
		xCounterPi_To_Stm32++;
	}
}

//后添加内容
/**
  * @funNm	Left_Conver	 
  * @brief	设置电机PWM 去死区 线性化速度	 
	* @param	in_speed (-100 : 100) 
  * @retval	对应速度的pwm  
*/ 
float Left_Conver(float in_speed)
{
	static float out_pwm = 1500;
	float speed = CONSTRAIN(in_speed, 30,-30);
	if(speed == 0){
	  out_pwm = 1500;
	}else if(speed>0){
		out_pwm = speed*2 + 1560;//(4B)  //1560;//(3B)//1550;
		//out_pwm = speed*2 + 1550;//(4B)  //1560;//(3B)//1550;
	}else if(speed<0){
		out_pwm = speed*2 + 1440;//(4B)//1450;//(3B)
		//out_pwm = speed*2 + 1440;//(4B)//1450;//(3B)
	}
	return out_pwm;
}

/**
  * @funN		Right_Conver	 
  * @brief	设置电机PWM 去死区 线性化速度	 	 
	* @param	in_speed (-100 : 100) 
  * @retval	对应速度的pwm  
*/ 
float Right_Conver(float in_speed)
{
	static float out_pwm = 1500;
	float speed = CONSTRAIN(in_speed, 30,-30);
	if(speed == 0){
		out_pwm = 1500;
	}else if(speed>0){
		//out_pwm = speed*1.5 + 1560;//(34B)//1530;
		out_pwm = speed*2 + 1560;//(34B)//1530;
	}else if(speed<0){
		//out_pwm = speed*1.5 + 1450;//(34B)//1470;
		out_pwm = speed*2 + 1440;//(34B)//1470;  //修改死区，正向转动
	}
	return out_pwm;
}

/**
  * @funN		Horizontal_Control	 
  * @brief	设置电机速度	 
	* @param	front_back 前后速度  left_right左右转速度
  * @retval	
*/ 
int left_pwm;
int right_pwm;
void Horizontal_Control(int16_t front_back,int16_t left_right)
{
		static int left_motor;
		static int right_motor;
	

		left_pwm  = Left_Conver ( front_back - left_right);
		right_pwm = Right_Conver(- front_back - left_right);

	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,left_pwm);//1
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,right_pwm);//3
	
	//测试左右电机死区
//	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,1500);//1
// 	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,1446);//3
//	
}