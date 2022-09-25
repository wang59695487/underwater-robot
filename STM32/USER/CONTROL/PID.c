#include "PID.h"
PID_t thruster_heading_pid;  
PID_t thruster_heading_inner_pid;

PID_t Deep_pid;
PID_t Heading_pid;
PID_t Pitch_pid;

NLPID_Typedef YawNLPID; 
extern MPU6050_Raw_Msg_t  mpu6050_raw;

//λ��ʽ����PID                                      //kp,ki,kd,  p_max,  p_min,  i_max,  i_min,  d_max,  d_min,  out_max,  out_min
float thruster_heading_pid_param[12] ={-0.2, 0 , 0,MAX_SPEED,-MAX_SPEED,MAX_SPEED,-MAX_SPEED,MAX_SPEED,-MAX_SPEED,MAX_SPEED,-MAX_SPEED,0};
//float thruster_heading_outerpid_param[12] ={-2, -0.002, 0,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,0};
float thruster_heading_inner_pid_param[12] ={-5, 0, 0,YAW_INNER_MAX_SPEED,-YAW_INNER_MAX_SPEED,YAW_INNER_MAX_SPEED,-YAW_INNER_MAX_SPEED,YAW_INNER_MAX_SPEED,-YAW_INNER_MAX_SPEED,YAW_INNER_MAX_SPEED,-YAW_INNER_MAX_SPEED,0};
float thruster_heading_outerpid_param[12] ={-5, -0.00, 1,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,YAW_OUTER_MAX_SPEED,-YAW_OUTER_MAX_SPEED,0};


//����ʽPID
//	-15.8    -0.076   ��ѹС��15.6V
//  -13.8    -0.05    ��ѹ����15.6V

//λ��ʽPID
//   -12     -0.35
//                            kp , ki,   kd,  p_max,      p_min,     i_max,      i_min,     d_max,     d_min,      out_max,  out_min   ,����
float Heading_pid_param[12]={  0 ,  0,    0, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, 0};
float Deep_pid_param[12]   ={  8,  0, 0, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, 0};
//float Pitch_pid_param[12]  ={ 2,0.1,0.5, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, 0};
float Pitch_pid_param[12]  ={ 12,0.05,1, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, MAX_SPEED, -MAX_SPEED, 0};
// ����   9 4 
	
	
//���ڲ�������ƶ����� ���360(���óɸ��������ֵ)
/**
  * @funNm		CalculateYawError
  * @brief	  ����Yaw���
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
	//PID_Param_Init(&Heading_pid,Heading_pid_param);   //����  PID
	PID_Param_Init(&Pitch_pid,Pitch_pid_param);
	PID_Param_Init(&Deep_pid,Deep_pid_param);         //���� - �ƽ���
	
	PID_Param_Init(&thruster_heading_pid,thruster_heading_pid_param); 
	//PID_Param_Init(&thruster_heading_pid,thruster_heading_outerpid_param); //���� - �ƽ����ƽ� PID
	//PID_Param_Init(&thruster_heading_inner_pid,thruster_heading_inner_pid_param); //���� - �ƽ����ƽ� PID
}
 
void PID_Param_Init(PID_t *pid, float *pid_param)//PID �������ú���
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
void PID_Clear(PID_t *pid)//PID �ۼ������
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
	//pid->d_Out = - pid->Kd * mpu6050_raw.angular_velocity[2];  //΢���ùߵ�ģ��z����Ľ��ٶ�
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
float Incremental_PID_Cal(float tar,float input,PID_t *pid)//����ʽPID���� 
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



/*               ˫��PID                           */
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
float SOTFOutput(SOTF_typeDef* gs,float data)//�����˲�
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
  * @brief  ��������޷����
  * @param	x:�������
  * @param	Lval:��С��ֵ
  * @param	Hval:�����ֵ
  * @retval float:�����޷������
  */
float SetValf32(float x,float Lval,float Hval)//��������޷����
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
  * @brief  ����ֵȡ���� ���ء�1
  * @param	x:����float������
  * @retval x > 0 return 1 x < 0 return -1 x = 0 return 0 
  */
float sign(float x)// ����ֵȡ���� ���ء�1
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
//������PID��ʼ��
float NLPID_Init(void)
{
	YawNLPID.OuterLoopEN=true;
	YawNLPID.T=0.02;
	YawNLPID.ZVal=0;
	/***�⻷����*5Hz***/
	YawNLPID.OIntf.num[0]=0.0571;
	YawNLPID.OIntf.num[1]=0.1143;
	YawNLPID.OIntf.num[2]=0.0571;
	YawNLPID.OIntf.den[0]=1.0000;
	YawNLPID.OIntf.den[1]=-1.0438;
	YawNLPID.OIntf.den[2]=0.2724;
	/*********/
	/***�⻷����*15Hz***/
	YawNLPID.Ofbtf.num[0]=0.2354;
	YawNLPID.Ofbtf.num[1]=0.4708;
	YawNLPID.Ofbtf.num[2]=0.2354;
	YawNLPID.Ofbtf.den[0]=1.0000;
	YawNLPID.Ofbtf.den[1]=-0.0592;
	YawNLPID.Ofbtf.den[2]=0.0009;
	/****�����⻷PID*****/
	YawNLPID.OP=1.9;//7.4
	YawNLPID.OI=0.01;//0.02;//
	YawNLPID.OD=0.001;

//  YawNLPID.OP=-3;//7.4
//	YawNLPID.OI=0;//0.02;//
//  YawNLPID.OD=1;
	
	YawNLPID.OeImax=100;
	YawNLPID.OoutMax=2000;
	YawNLPID.OoutMin=-2000;
	/****�����ڻ�PID******/
	YawNLPID.falnum=1.2;
	YawNLPID.INVal=25;
  YawNLPID.IP=0.25;
	YawNLPID.II=0.02;
	YawNLPID.ID=0.001;
//  YawNLPID.IP=5;
//	YawNLPID.II=0;
//	YawNLPID.ID=0;
	
	YawNLPID.IeImax=100;//300
	/***�ڻ�����****/
	YawNLPID.Oouttf.num[0]=1;
	YawNLPID.Oouttf.num[1]=0;
	YawNLPID.Oouttf.num[2]=0;
	YawNLPID.Oouttf.den[0]=1;
	YawNLPID.Oouttf.den[1]=0;
	YawNLPID.Oouttf.den[2]=0;
	/***�ڻ�����15Hz****/
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
	float CLHeading=YawErrorDispose(nl->Ofbtf.yk,nl->OIntf.yk);   //�ж�ת�������ݽ������������ж�
	if(nl->OuterLoopEN == true)//�⻷ʹ��
	{
		//�⻷
		nl->Oe=nl->OIntf.yk - CLHeading;    												//�⻷��� 
		if(fabs(nl->Oe)< nl->ZVal) return 0;												//��������
		
		nl->Oe1= ((nl->OIntf.yk - nl->OIntf.yk_2)-(nl->Ofbtf.yk-nl->Ofbtf.yk_2))/nl->T;//΢��
		nl->OeI= nl->OeI+nl->Oe*nl->T;															//����
		nl->OeI= SetValf32(nl->OeI,-1*nl->OeImax,nl->OeImax);				//�޷�	
		nl->Ooutput=nl->OP * nl->Oe + nl->OI*nl->OeI+nl->OD*nl->Oe1;//���
		nl->Ooutput=SetValf32(nl->Ooutput,nl->OoutMin,nl->OoutMax);	//�޷�

		//�ڻ�
		SOTFOutput(&nl->Oouttf,nl->Ooutput);												//�����˲�
		nl->Ie= nl->Oouttf.yk - nl->Ifbtf.yk;    										//�ڻ�������
		nl->Ie1=((nl->Oouttf.yk - nl->Oouttf.yk_2) - (nl->Ifbtf.yk - nl->Ifbtf.yk_2))/nl->T; 	//΢��
		nl->IeI=nl->IeI+nl->Ie*nl->T;																//����
		nl->IeI= SetValf32(nl->IeI,-1*nl->IeImax,nl->IeImax);				//�޷�
//		nl->Ioutput=nl->IP*nl->Ie+nl->II*nl->IeI+nl->ID*nl->Ie1;
		nl->Ioutput= nl->IP*NLfal(nl->Ie,nl->falnum,nl->INVal)			
		+nl->II*nl->IeI
		+nl->ID*NLfal(nl->Ie1,nl->falnum/2,nl->INVal);
	}
	else{
		SOTFOutput(&nl->Oouttf,obj);																//�����˲�
		//�ڻ�
		nl->Ie= nl->Oouttf.yk - nl->Ifbtf.yk;												//�ڻ�������
		nl->Ie1=((nl->Oouttf.yk - nl->Oouttf.yk_2) - (nl->Ifbtf.yk - nl->Ifbtf.yk_2))/nl->T;	//΢��
		nl->IeI=nl->IeI+nl->Ie*nl->T;																//����
		nl->IeI= SetValf32(nl->IeI,-1*nl->IeImax,nl->IeImax);				//�޷�
//	nl->Ioutput=nl->IP*nl->Ie+nl->II*nl->IeI+nl->ID*nl->Ie1;
		nl->Ioutput= nl->IP*NLfal(nl->Ie,nl->falnum,nl->INVal)
		+nl->II*nl->IeI
		+nl->ID*NLfal(nl->Ie1,nl->falnum/2,nl->INVal);
	}
	return nl->Ioutput;
}




