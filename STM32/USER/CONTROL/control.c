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
 * @brief : ��ֱ������ �ٶ�����
 * @param : front_val��ǰ���������ֵ	  back_val��������ֵֵ
 * @retval: void
 */
void SetUpDowm(float front_val,float back_val)//min--max �����˶�
{
		int16_t testupdown_motor = 0;	//60
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,  testupdown_motor + MIDValue  + front_val);//2	ǰ 
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,  testupdown_motor + MIDValue  - back_val);//4 	��
	
	
//    // Added by Li Wenwen, 2021,4,12
//	  //ǰ�����������ٽ�ֵ
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
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,  testupdown_motor + MIDValue  + front_val);//ǰ
//		if(back_val>0)
//		{
//			testupdown_motor = deadzone_back_down - MIDValue;
//		}
//		else
//		{
//			testupdown_motor = deadzone_back_up - MIDValue;
//		}
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,  testupdown_motor + MIDValue  - back_val);//4 	��

} 

/**
 * @funNm : SetHorizontal
 * @brief : ˮƽ������ �ٶ�����
* @param : f_b:ǰ�����˲���  rotate����ת����
 * @retval: void
 */
void SetHorizontal(float f_b, float rotate)//ˮƽ�˶�	
{
	float temp_l=  f_b + MIDValue  + rotate;
	float temp_r= -f_b + MIDValue  + rotate ;
		
	//���Ʒ���һ
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,temp_l);//1
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,temp_r);//3
}

/**
 * @funNm : Clear_Control_Disable_Counter
 * @brief : ������Ƽ�����
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
 * @brief : ��ȡ���Ƽ�����
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
 * @brief : ���Ƽ�����
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

//���������
/**
  * @funNm	Left_Conver	 
  * @brief	���õ��PWM ȥ���� ���Ի��ٶ�	 
	* @param	in_speed (-100 : 100) 
  * @retval	��Ӧ�ٶȵ�pwm  
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
  * @brief	���õ��PWM ȥ���� ���Ի��ٶ�	 	 
	* @param	in_speed (-100 : 100) 
  * @retval	��Ӧ�ٶȵ�pwm  
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
		out_pwm = speed*2 + 1440;//(34B)//1470;  //�޸�����������ת��
	}
	return out_pwm;
}

/**
  * @funN		Horizontal_Control	 
  * @brief	���õ���ٶ�	 
	* @param	front_back ǰ���ٶ�  left_right����ת�ٶ�
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
	
	//�������ҵ������
//	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,1500);//1
// 	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,1446);//3
//	
}