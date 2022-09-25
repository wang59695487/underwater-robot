#include "Control_Task.h"
#include "control.h"

ControlMsg_t control_data;

//float kp_angle = 1.0; //pid????
//float ki_angle = 1.0; //pid????
//float kd_angle = 1.0; //pid????

float front_back_rocker = 0.0;                        //��� - ǰ��ҡ��λ��
float left_right_rocker = 0.0;                        //��� - ����ҡ��λ��
float up_down_rocker = 0.0;                           //�ұ� - ����ҡ��λ��
float left_right_adjust_rock = 0.0;                   //�ұ� - ����ҡ��λ��

int16_t test_motor = 65;
float heading_pid_tar = 0;
float heading_pid_out = 0;
float pitching_pid_out = 0;
float deepth_pid_out = 0;
float deep_tar = 15;   //���ֵ����λ CM
float navi_position  = 0.0;

float move_scale = 0; //����ʱǰ��������ϵ��

//����ʹ��
u8 pitch_en =0;
u8 deep_en = 0;
u8 head_en = 1; //ʹ���˸��飬����Ҫ����Ⱥ͸����ǣ�����ֻ��������ʹ��
//u8 pitch_en =0;
//u8 deep_en = 0;
//u8 head_en =0;


u16 xolotest=0;

SOTF_typeDef RockerYAWGs;

void control_Task_Function(void const * argument)
{

    portTickType tick = xTaskGetTickCount();
    NLPID_Init();
    //������PID��ʼ��
    Rocker_SOTF_Init(&RockerYAWGs);
    while(1)
    {
#ifdef DEBUG

#else
        if(xQueueReceive(Control_Message_Queue,&(control_data),portMAX_DELAY))
        {
//			SetUpDowm(100,100);
//			Horizontal_Control(100,0);

//            //������ PID ����
//            if(pitch_en==1) {
//                pitching_pid_out = Incremental_PID_Cal(0,control_data.pitching,&Pitch_pid);
//                SetUpDowm(-0.5*pitching_pid_out,0.8*pitching_pid_out);
//            } else {
//                SetUpDowm(0,0);
//            }

//            //��� PID ����
//            if(deep_en==1) {
//                //deepth_pid_out = Incremental_PID_Cal(deep_tar,control_data.depth,&Deep_pid);
//                deepth_pid_out = Position_PID_Cal(deep_tar,control_data.depth,&Deep_pid);
//                SetUpDowm(0.8*deepth_pid_out,0.8*deepth_pid_out);
//            } else {
//                SetUpDowm(0,0);
//            }
//
//		//����PID
            if(head_en==1) {
                heading_pid_out = YAWPID_Control(&YawNLPID,control_data.navi_angle,control_data.navi_distance,heading_pid_tar);
                heading_pid_out = CONSTRAIN(heading_pid_out, 12, -12);  				//�޷�

                move_scale = (1 - fabs(heading_pid_out)/10.f)*0.1; 									//����ʱǰ��������ϵ��
                //move_scale = 0;
                Horizontal_Control(2*move_scale*test_motor,0.7*heading_pid_out);	//�����ٶ�����
            } else {
                Horizontal_Control(0,0);
            }

        }
#endif
    }
}

