#ifndef __CONTROL_H
#define __CONTROL_H

#include "includes.h"

#define  CONSTRAIN(x,max,min) (x>max?max:(x<min?min:x))

//#define  MAX_SPEED            100
#define  MAX_SPEED            110
#define  MIDValue             1500
// 左电机   -27    73         MIDValue + (73-27)/2   23  
// 右电机   -13    87         MIDValue + (87-13)/2   37

//#define  front_mid            1500
//#define  back_mid             1500 
//#define  right_mid            1500
//#define  left_mid             1500
//#define  max_speed_delta      5    
//#define  max_speed_value     (MIDValue + MAX_SPEED)
//#define  min_speed_value     (MIDValue - MAX_SPEED)

#define DIREC_SERVO_MID        	( 1500 )
#define MOVE_SERVO_MID    		 	( 1000 )
//#define MAX_SPEED              	( 80 )
#define YAW_INNER_MAX_SPEED		 	( 150 )
#define YAW_OUTER_MAX_SPEED		 	( 120 )
#define CONSTRAIN(x,max,min)   	(x>max?max:(x<min?min:x))
#define LONLAT_ADDR  ((uint32_t)0x08040000)


typedef enum 
{
    MANUAL = 0,
    AUTO
}ControlState_t;

typedef struct{
//	int16_t channel[4];
	float temperture;
	float depth;
	float yawing;       //航向
	float pitching;   	//前后偏角
	float rolling;    	//左右偏角
	
	//添加如下内容
	float pitch_groy;		
	float roll_groy;
	float yaw_groy;
	
	ControlState_t depth_state;
	ControlState_t heading_state;
	bool  Control_Disable;
	float navi_angle;
	float navi_distance;
}ControlMsg_t;

//函数定义
void SetUpDowm(float front_val,float back_val); //垂直运动
void SetHorizontal(float f_b,float rotate);     //水平运动
void Clear_Control_Disable_Counter(void);				//清除控制计数器
bool Get_Control_Disable_Counter_State(void);		//获取控制计数器
void Adder_Control_Disable_Counter(void);				//控制计数器
//后添加
float Left_Conver(float in_speed);							//设置电机PWM 去死区 线性化速度	 
float Right_Conver(float in_speed);							//设置电机PWM 去死区 线性化速度	 
void Horizontal_Control(int16_t front_back,int16_t left_right);//水平运动控制，去除线性化后
#endif

