#ifndef __PID_H
#define __PID_H

#include "includes.h"
#include "stdbool.h"
//双环PID
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
	bool OuterLoopEN;//true:开启外环 
	SOTF_typeDef OIntf;//外环输入
	SOTF_typeDef Ofbtf;//外环反馈
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
	SOTF_typeDef Oouttf;//外环输出
	SOTF_typeDef Ifbtf;//内环反馈
	//fal函数参数
	float INVal;//线性区间
	float falnum;//0.5-1之间
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
	float Kp;/*比例系数*/
	float Ki;/*积分系数*/
	float Kd;/*微分系数*/

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
	float Last_Bias;			/*前一拍偏差*/
	float Last_Last_Bias;/*前两拍偏差*/
	
	float OutPut;  		/*输出值*/
	float OutPutMAX; 	/*输出值上限*/
	float OutPutMIN;	/*输出值下限*/
	
	float deadband;/*死区*/
//  float result;                 /*PID控制器计算结果*/
//  float errorabsmax;            /*偏差绝对值最大值*/
//  float errorabsmin;            /*偏差绝对值最小值*/
//  float alpha;                  /*不完全微分系数*/
//  float deltadiff;              /*微分增量*/
}PID_t;

#define CONSTRAIN(x,max,min) (x>max?max:(x<min?min:x))

extern PID_t Deep_pid;
extern PID_t Heading_pid;
extern PID_t Pitch_pid;

extern PID_t thruster_heading_pid;
extern PID_t thruster_heading_inner_pid; 

extern void PID_Init(void);                                        //PID初始化
void PID_Param_Init(PID_t *pid, float *pid_param);                 //PID参数设置
extern float Incremental_PID_Cal(float tar,float input,PID_t *pid);//增量式PID计算 
float Position_PID_Cal(float tar,float input,PID_t *pid);					 //位置式PID计算
float Yaw_Inner_Position_PID_Cal(float tar_sp,float temp_sp,PID_t *pid);//双环位置式PID内环
float Yaw_Outer_Position_PID_Cal(float tar,float input,PID_t *pid);//双环位置式PID外环
extern void PID_Clear(PID_t *pid);							                   //PID 累计量清空 

//添加内容
float CalculateYawError(float now,float obj);	//计算Yaw误差

//双环非线性PID
extern NLPID_Typedef YawNLPID; 
void Rocker_SOTF_Init(SOTF_typeDef* gs); //双环PID参数初始化
float NLPID_Init(void);	//非线性PID初始化
extern float YAWPID_Control(NLPID_Typedef *nl,float Onow,float Inow,float obj);//PID控制函数
#endif
