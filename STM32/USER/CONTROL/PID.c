#include "PID.h"
PID_t thruster_heading_pid;  
PID_t thruster_heading_inner_pid;

PID_t Deep_pid;
PID_t Heading_pid;
PID_t Pitch_pid;

NLPID_Typedef YawNLPID; 
extern MPU6050_Raw_Msg_t  mpu6050_raw;

//位置式单环PID                                      //kp,ki,kd,  p_max,  p_min,  i_max,  i_min,  d_max,  d_min,  out_max,  out_min
float thruster_heading_pid_param[12] ={-0.2, 0 , 0,MAX_SPEED,-MAX_SPEED,MAX_SPEED,-MAX_SPEED,MAX_SPEED,-MAX_SPEED,MAX_SPEED,-MAX_SPEED,0};
//float thruster_heading_outerpid_param[12] ={-2, -0.002, 0,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,0};
float thruster_heading_inner_pid_param[12] ={-5, 0, 0,YAW_INNER_MAX_SPEED,-YAW_INNER_MAX_SPEED,YAW_INNER_MAX_SPEED,-YAW_INNER_MAX_SPEED,YAW_INNER_MAX_SPEED,-YAW_INNER_MAX_SPEED,YAW_INNER_MAX_SPEED,-YAW_INNER_MAX_SPEED,0};
float thruster_heading_outerpid_param[12] ={-5, -0.00, 1,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,0};


//增量式PID
//	-15.8    -0.076   电压小于15.6V
//  -13.8    -0.05    电压大于15.6V

//位置式PID
//   -12     -0.35
//                            kp , ki,   kd,  p_max,      p_min,     i_max,      i_min,     d_max,     d_min,      out_max,  out_min   ,死区
float Heading_pid_param[12]={  0 ,  0,    0, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, 0};
float Deep_pid_param[12]   ={  8,  0, 0, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, 0};
//float Pitch_pid_param[12]  ={ 2,0.1,0.5, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, 0};
float Pitch_pid_param[12]  ={ 12,0.05,1, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, 0};
// 定深   9 4 
	
	
//由于步进电机移动缓慢 设成360(设置成俯仰角最大值)
/**
  * @funNm		CalculateYawError
  * @brief	  计算Yaw误差
  * @param		
  * @retval		void 
*/   
float CalculateYawError(float now,float obj)
{
	float err = obj - now;
	float output = 0;
	if (err > 180)
		output = err - 360;
	else if (err < -180)
		output = err + 360;
	else
		output = err;
	return output;
} 

void PID_Init(void) 
{
  PID_Clear(&Deep_pid);
	PID_Clear(&Pitch_pid);
	PID_Clear(&Heading_pid);
	//PID_Param_Init(&Heading_pid,Heading_pid_param);   //定航  PID
	PID_Param_Init(&Pitch_pid,Pitch_pid_param);
	PID_Param_Init(&Deep_pid,Deep_pid_param);         //定深 - 推进器
	
	PID_Param_Init(&thruster_heading_pid,thruster_heading_pid_param); 
	//PID_Param_Init(&thruster_heading_pid,thruster_heading_outerpid_param); //定航 - 推进器推进 PID
	//PID_Param_Init(&thruster_heading_inner_pid,thruster_heading_inner_pid_param); //定航 - 推进器推进 PID
}
 
void PID_Param_Init(PID_t *pid, float *pid_param)//PID 参数设置函数
{
	pid->Kp = pid_param[0];
	pid->Ki = pid_param[1];;
	pid->Kd = pid_param[2];;
	pid->p_OutMAX = pid_param[3];
	pid->p_OutMIN = pid_param[4];
	pid->i_OutMAX = pid_param[5];
	pid->i_OutMIN = pid_param[6];
	pid->d_OutMAX = pid_param[7];
	pid->d_OutMIN = pid_param[8];
	pid->OutPutMAX = pid_param[9];
	pid->OutPutMIN = pid_param[10];
	pid->deadband = pid_param[11];
}
void PID_Clear(PID_t *pid)//PID 累计量清空
{
  pid->Bias = 0;
	pid->Last_Bias = 0;
	pid->Last_Last_Bias = 0;
	pid->p_Out = 0;
	pid->i_Out = 0;
	pid->d_Out = 0;
	pid->OutPut = 0;
} 
float Yaw_Inner_Position_PID_Cal(float tar_sp,float temp_sp,PID_t *pid)
{
	pid->Bias =  tar_sp - temp_sp;
	
	pid->p_Out = pid->Kp*pid->Bias;
	pid->p_Out = CONSTRAIN(pid->p_Out, pid->p_OutMAX, pid->p_OutMIN); 
	
	pid->i_Out +=pid->Ki*pid->Bias;
	pid->i_Out = CONSTRAIN(pid->i_Out, pid->i_OutMAX, pid->i_OutMIN);
	
	pid->d_Out = pid->Kd*(pid->Bias - pid->Last_Bias);
	pid->d_Out = CONSTRAIN(pid->d_Out, pid->d_OutMAX, pid->d_OutMIN);
	
	pid->OutPut = pid->p_Out + pid->i_Out + pid->d_Out;
	
	pid->Last_Bias = pid->Bias;
	return pid->OutPut;	
	
}
float Yaw_Outer_Position_PID_Cal(float tar,float input,PID_t *pid)
{
	pid->Bias =  CalculateYawError(input,tar);
	
	pid->p_Out = pid->Kp*pid->Bias;
	pid->p_Out = CONSTRAIN(pid->p_Out, pid->p_OutMAX, pid->p_OutMIN); 
	
	pid->i_Out +=pid->Ki*pid->Bias;
	pid->i_Out = CONSTRAIN(pid->i_Out, pid->i_OutMAX, pid->i_OutMIN);
	
	pid->d_Out = pid->Kd*(pid->Bias - pid->Last_Bias);
	//pid->d_Out = - pid->Kd * mpu6050_raw.angular_velocity[2];  //微分用惯导模块z方向的角速度
	pid->d_Out = CONSTRAIN(pid->d_Out, pid->d_OutMAX, pid->d_OutMIN);
	
	pid->OutPut = pid->p_Out + pid->i_Out + pid->d_Out;
	
	pid->Last_Bias = pid->Bias;
	return pid->OutPut;	
	
}
extern MPU6050Msg_t mpu6050_pos_data;
float Position_PID_Cal(float tar,float input,PID_t *pid)
{
	pid->Bias =  CalculateYawError(input,tar);
	
	pid->p_Out = pid->Kp*pid->Bias;
	
	
	
	pid->p_Out = CONSTRAIN(pid->p_Out, pid->p_OutMAX, pid->p_OutMIN); 
	
	pid->i_Out +=pid->Ki*pid->Bias;
	pid->i_Out = CONSTRAIN(pid->i_Out, pid->i_OutMAX, pid->i_OutMIN);
	
	pid->d_Out = pid->Kd*(pid->Bias - pid->Last_Bias); 
	pid->d_Out = CONSTRAIN(pid->d_Out, pid->d_OutMAX, pid->d_OutMIN);
	
	pid->OutPut = pid->p_Out + pid->i_Out + pid->d_Out;
	pid->OutPut  = CONSTRAIN(pid->OutPut,pid->OutPutMAX,pid->OutPutMIN);
	
	pid->Last_Bias = pid->Bias;
	return pid->OutPut;
}
float Incremental_PID_Cal(float tar,float input,PID_t *pid)//增量式PID计算 
{
			
  float out;
	pid->Bias = CalculateYawError(input,tar);
	//pid->Bias = tar - input;

	pid->p_Out = pid->Kp*(pid->Bias-pid->Last_Bias);                                      
	pid->p_Out = CONSTRAIN(pid->p_Out, pid->p_OutMAX, pid->p_OutMIN);           
	
	pid->i_Out = pid->Ki*pid->Bias;                                                          
	pid->i_Out = CONSTRAIN(pid->i_Out, pid->i_OutMAX, pid->i_OutMIN);
	
	pid->d_Out = pid->Kd*(pid->Bias - 2*pid->Last_Bias + pid->Last_Last_Bias);
  pid->d_Out = CONSTRAIN(pid->d_Out, pid->d_OutMAX, pid->d_OutMIN);
	
	pid->OutPut += pid->p_Out+pid->i_Out+pid->d_Out;
	//pid->p_Out   = pid->p_Out - mpu6050_raw.angular_velocity[0]*1.0;
	pid->OutPut  = CONSTRAIN(pid->OutPut,pid->OutPutMAX,pid->OutPutMIN);
	
	pid->Last_Last_Bias = pid->Last_Bias;
	pid->Last_Bias = pid->Bias;						
  return pid->OutPut;
}



/*               双环PID                           */
void Rocker_SOTF_Init(SOTF_typeDef* gs)
{
//	//1Hz T=0.02
//	gs->num[0]=0.0035;
//	gs->num[1]=0.0070;
//	gs->num[2]=0.0035;
//	gs->den[0]=1.0000;
//	gs->den[1]=-1.7635;
//	gs->den[2]=0.7775;
//	// 0.7Hz T=0.02
//	gs->num[0]=0.0018;
//	gs->num[1]=0.0035;
//	gs->num[2]=0.0018;
//	gs->den[0]=1.0000;
//	gs->den[1]=-1.8315;
//	gs->den[2]=0.8386;
//	// 0.5Hz  T=0.02 
//	gs->num[0]=0.0009;
//	gs->num[1]=0.0019;
//	gs->num[2]=0.0009;
//	gs->den[0]=1.0000;
//	gs->den[1]=-1.8782;
//	gs->den[2]=0.8819;
	// 0.2Hz  T=0.02
	gs->num[0]=0.0001540;
	gs->num[1]=0.0003080;
	gs->num[2]=0.0001540;
	gs->den[0]=1.0000;
	gs->den[1]=-1.9504;
	gs->den[2]=0.9510;
//	// 0.1Hz  T=0.02
//	gs->num[0]=0.00003899;
//	gs->num[1]=0.00007797;
//	gs->num[2]=0.00003899;
//	gs->den[0]=1.0000;
//	gs->den[1]=-1.9750;
//	gs->den[2]=0.9752;
}
float SOTFOutput(SOTF_typeDef* gs,float data)//二阶滤波
{
	gs->uk=data;
	gs->yk=-(gs->den[1])*(gs->yk_1)-(gs->den[2])*(gs->yk_2) 
					+(gs->num[0])*(gs->uk)+(gs->num[1])*(gs->uk_1)+(gs->num[2])*(gs->uk_2);
	gs->yk_2=gs->yk_1;
	gs->yk_1=gs->yk;
	gs->uk_2=gs->uk_1;
	gs->uk_1=gs->uk;
	return  (gs->yk);
}
/**
  * @funNm  SetValf32
  * @brief  输入参数限幅输出
  * @param	x:输入参数
  * @param	Lval:最小阈值
  * @param	Hval:最大阈值
  * @retval float:返回限幅后的数
  */
float SetValf32(float x,float Lval,float Hval)//输入参数限幅输出
{
	if(x > Hval){
		return Hval;
	}
	else if(x < Lval){
		return Lval;
	}
	return x;
}
/**
  * @funNm	sign
  * @brief  输入值取符号 返回±1
  * @param	x:输入float型数据
  * @retval x > 0 return 1 x < 0 return -1 x = 0 return 0 
  */
float sign(float x)// 输入值取符号 返回±1
{
	float output=0;
	if(x > 0){
		output=1;
	}
	else if(x < 0){
		output=-1;
	}
	return output;
}
float NLfal(float x,float a,float val)
{
	float out;
	float Absx=fabs(x);
	float signx=sign(x);
	if(Absx > 10000){
		out=x;
	}
	else{
		out=signx*val*pow(Absx/val,1.0/a);
	}
	return out;
}
float YawErrorDispose(float now,float obj)
{
	float err=obj-now;
	if(err > 180){
		return (180 - fabs(now)+(180-obj)+obj);
	}
	else if(err < -180){
		return (-1*((180 - fabs(now))+(180+obj))+obj);
	}
	return now;
}
//非线性PID初始化
float NLPID_Init(void)
{
	YawNLPID.OuterLoopEN=true;
	YawNLPID.T=0.02;
	YawNLPID.ZVal=0;
	/***外环输入*5Hz***/
	YawNLPID.OIntf.num[0]=0.0571;
	YawNLPID.OIntf.num[1]=0.1143;
	YawNLPID.OIntf.num[2]=0.0571;
	YawNLPID.OIntf.den[0]=1.0000;
	YawNLPID.OIntf.den[1]=-1.0438;
	YawNLPID.OIntf.den[2]=0.2724;
	/*********/
	/***外环反馈*15Hz***/
	YawNLPID.Ofbtf.num[0]=0.2354;
	YawNLPID.Ofbtf.num[1]=0.4708;
	YawNLPID.Ofbtf.num[2]=0.2354;
	YawNLPID.Ofbtf.den[0]=1.0000;
	YawNLPID.Ofbtf.den[1]=-0.0592;
	YawNLPID.Ofbtf.den[2]=0.0009;
	/****航向外环PID*****/
	YawNLPID.OP=1.9;//7.4
	YawNLPID.OI=0.01;//0.02;//
	YawNLPID.OD=0.001;

//  YawNLPID.OP=-3;//7.4
//	YawNLPID.OI=0;//0.02;//
//  YawNLPID.OD=1;
	
	YawNLPID.OeImax=100;
	YawNLPID.OoutMax=2000;
	YawNLPID.OoutMin=-2000;
	/****航向内环PID******/
	YawNLPID.falnum=1.2;
	YawNLPID.INVal=25;
  YawNLPID.IP=0.25;
	YawNLPID.II=0.02;
	YawNLPID.ID=0.001;
//  YawNLPID.IP=5;
//	YawNLPID.II=0;
//	YawNLPID.ID=0;
	
	YawNLPID.IeImax=100;//300
	/***内环输入****/
	YawNLPID.Oouttf.num[0]=1;
	YawNLPID.Oouttf.num[1]=0;
	YawNLPID.Oouttf.num[2]=0;
	YawNLPID.Oouttf.den[0]=1;
	YawNLPID.Oouttf.den[1]=0;
	YawNLPID.Oouttf.den[2]=0;
	/***内环反馈15Hz****/
	YawNLPID.Ifbtf.num[0]=0.2354;
	YawNLPID.Ifbtf.num[1]=0.4708;
	YawNLPID.Ifbtf.num[2]=0.2354;
	YawNLPID.Ifbtf.den[0]=1.0000;
	YawNLPID.Ifbtf.den[1]=-0.0592;
	YawNLPID.Ifbtf.den[2]=0.0009;

}
float YAWPID_Control(NLPID_Typedef *nl,float Onow,float Inow,float obj)
{
	SOTFOutput(&nl->OIntf,obj);
	SOTFOutput(&nl->Ofbtf,Onow);
	SOTFOutput(&nl->Ifbtf,Inow);
	float CLHeading=YawErrorDispose(nl->Ofbtf.yk,nl->OIntf.yk);   //判断转向后对数据进行收敛方向判断
	if(nl->OuterLoopEN == true)//外环使能
	{
		//外环
		nl->Oe=nl->OIntf.yk - CLHeading;    												//外环误差 
		if(fabs(nl->Oe)< nl->ZVal) return 0;												//死区控制
		
		nl->Oe1= ((nl->OIntf.yk - nl->OIntf.yk_2)-(nl->Ofbtf.yk-nl->Ofbtf.yk_2))/nl->T;//微分
		nl->OeI= nl->OeI+nl->Oe*nl->T;															//积分
		nl->OeI= SetValf32(nl->OeI,-1*nl->OeImax,nl->OeImax);				//限幅	
		nl->Ooutput=nl->OP * nl->Oe + nl->OI*nl->OeI+nl->OD*nl->Oe1;//输出
		nl->Ooutput=SetValf32(nl->Ooutput,nl->OoutMin,nl->OoutMax);	//限幅

		//内环
		SOTFOutput(&nl->Oouttf,nl->Ooutput);												//二阶滤波
		nl->Ie= nl->Oouttf.yk - nl->Ifbtf.yk;    										//内环误差计算
		nl->Ie1=((nl->Oouttf.yk - nl->Oouttf.yk_2) - (nl->Ifbtf.yk - nl->Ifbtf.yk_2))/nl->T; 	//微分
		nl->IeI=nl->IeI+nl->Ie*nl->T;																//积分
		nl->IeI= SetValf32(nl->IeI,-1*nl->IeImax,nl->IeImax);				//限幅
//		nl->Ioutput=nl->IP*nl->Ie+nl->II*nl->IeI+nl->ID*nl->Ie1;
		nl->Ioutput= nl->IP*NLfal(nl->Ie,nl->falnum,nl->INVal)			
		+nl->II*nl->IeI
		+nl->ID*NLfal(nl->Ie1,nl->falnum/2,nl->INVal);
	}
	else{
		SOTFOutput(&nl->Oouttf,obj);																//二阶滤波
		//内环
		nl->Ie= nl->Oouttf.yk - nl->Ifbtf.yk;												//内环误差计算
		nl->Ie1=((nl->Oouttf.yk - nl->Oouttf.yk_2) - (nl->Ifbtf.yk - nl->Ifbtf.yk_2))/nl->T;	//微分
		nl->IeI=nl->IeI+nl->Ie*nl->T;																//积分
		nl->IeI= SetValf32(nl->IeI,-1*nl->IeImax,nl->IeImax);				//限幅
//	nl->Ioutput=nl->IP*nl->Ie+nl->II*nl->IeI+nl->ID*nl->Ie1;
		nl->Ioutput= nl->IP*NLfal(nl->Ie,nl->falnum,nl->INVal)
		+nl->II*nl->IeI
		+nl->ID*NLfal(nl->Ie1,nl->falnum/2,nl->INVal);
	}
	return nl->Ioutput;
}




