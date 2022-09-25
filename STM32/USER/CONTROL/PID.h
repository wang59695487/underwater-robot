#ifndef __PID_H
#define __PID_H

#include "includes.h"
#include "stdbool.h"
//˫��PID
typedef struct
{
	float yk;
	float yk_1;
	float yk_2;
	float uk;
	float uk_1;
	float uk_2;
	float num[3];
	float den[3];
}SOTF_typeDef;

typedef struct
{
	bool OuterLoopEN;//true:�����⻷ 
	SOTF_typeDef OIntf;//�⻷����
	SOTF_typeDef Ofbtf;//�⻷����
	float Oe;
	float Oe1;
	float OeI;
	float OeImax;
	float OP;
	float OI;
	float OD;
	float Ooutput;
	float OoutMax;
	float OoutMin;
	SOTF_typeDef Oouttf;//�⻷���
	SOTF_typeDef Ifbtf;//�ڻ�����
	//fal��������
	float INVal;//��������
	float falnum;//0.5-1֮��
	float Ie;
	float Ie1;
	float IeI;
	float IeImax;
	float IP;
	float II;
	float ID;
	float ZVal;
	float Ioutput;
	float T;
	
}NLPID_Typedef;

typedef struct{
	float Kp;/*����ϵ��*/
	float Ki;/*����ϵ��*/
	float Kd;/*΢��ϵ��*/

	float p_Out;
	float i_Out;
	float d_Out;
	
	float p_OutMAX;
	float p_OutMIN;
	
	float i_OutMAX;
	float i_OutMIN;
	
	float d_OutMAX;
	float d_OutMIN;
  
	float Bias;
	float Last_Bias;			/*ǰһ��ƫ��*/
	float Last_Last_Bias;/*ǰ����ƫ��*/
	
	float OutPut;  		/*���ֵ*/
	float OutPutMAX; 	/*���ֵ����*/
	float OutPutMIN;	/*���ֵ����*/
	
	float deadband;/*����*/
//  float result;                 /*PID������������*/
//  float errorabsmax;            /*ƫ�����ֵ���ֵ*/
//  float errorabsmin;            /*ƫ�����ֵ��Сֵ*/
//  float alpha;                  /*����ȫ΢��ϵ��*/
//  float deltadiff;              /*΢������*/
}PID_t;

#define CONSTRAIN(x,max,min) (x>max?max:(x<min?min:x))

extern PID_t Deep_pid;
extern PID_t Heading_pid;
extern PID_t Pitch_pid;

extern PID_t thruster_heading_pid;
extern PID_t thruster_heading_inner_pid; 

extern void PID_Init(void);                                        //PID��ʼ��
void PID_Param_Init(PID_t *pid, float *pid_param);                 //PID��������
extern float Incremental_PID_Cal(float tar,float input,PID_t *pid);//����ʽPID���� 
float Position_PID_Cal(float tar,float input,PID_t *pid);					 //λ��ʽPID����
float Yaw_Inner_Position_PID_Cal(float tar_sp,float temp_sp,PID_t *pid);//˫��λ��ʽPID�ڻ�
float Yaw_Outer_Position_PID_Cal(float tar,float input,PID_t *pid);//˫��λ��ʽPID�⻷
extern void PID_Clear(PID_t *pid);							                   //PID �ۼ������ 

//�������
float CalculateYawError(float now,float obj);	//����Yaw���

//˫��������PID
extern NLPID_Typedef YawNLPID; 
void Rocker_SOTF_Init(SOTF_typeDef* gs); //˫��PID������ʼ��
float NLPID_Init(void);	//������PID��ʼ��
extern float YAWPID_Control(NLPID_Typedef *nl,float Onow,float Inow,float obj);//PID���ƺ���
#endif
