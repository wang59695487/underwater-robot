#ifndef __CONTROL_H
#define __CONTROL_H

#include "includes.h"

#define  CONSTRAIN(x,max,min) (x>max?max:(x<min?min:x))

//#define  MAX_SPEED            100
#define  MAX_SPEED            110
#define  MIDValue             1500
// ����   -27    73         MIDValue + (73-27)/2   23  
// �ҵ��   -13    87         MIDValue + (87-13)/2   37

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
	float yawing;       //����
	float pitching;   	//ǰ��ƫ��
	float rolling;    	//����ƫ��
	
	//�����������
	float pitch_groy;		
	float roll_groy;
	float yaw_groy;
	
	ControlState_t depth_state;
	ControlState_t heading_state;
	bool  Control_Disable;
	float navi_angle;
	float navi_distance;
}ControlMsg_t;

//��������
void SetUpDowm(float front_val,float back_val); //��ֱ�˶�
void SetHorizontal(float f_b,float rotate);     //ˮƽ�˶�
void Clear_Control_Disable_Counter(void);				//������Ƽ�����
bool Get_Control_Disable_Counter_State(void);		//��ȡ���Ƽ�����
void Adder_Control_Disable_Counter(void);				//���Ƽ�����
//�����
float Left_Conver(float in_speed);							//���õ��PWM ȥ���� ���Ի��ٶ�	 
float Right_Conver(float in_speed);							//���õ��PWM ȥ���� ���Ի��ٶ�	 
void Horizontal_Control(int16_t front_back,int16_t left_right);//ˮƽ�˶����ƣ�ȥ�����Ի���
#endif

