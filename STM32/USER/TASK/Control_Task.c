#include "Control_Task.h"
#include "control.h"

ControlMsg_t control_data;

//float kp_angle = 1.0; //pid????
//float ki_angle = 1.0; //pid????
//float kd_angle = 1.0; //pid????

float front_back_rocker = 0.0;                        //左边 - 前后摇杆位置
float left_right_rocker = 0.0;                        //左边 - 左右摇杆位置
float up_down_rocker = 0.0;                           //右边 - 上下摇杆位置
float left_right_adjust_rock = 0.0;                   //右边 - 左右摇杆位置

int16_t test_motor = 65;
float heading_pid_tar = 0;
float heading_pid_out = 0;
float pitching_pid_out = 0;
float deepth_pid_out = 0;
float deep_tar = 15;   //深度值，单位 CM
float navi_position  = 0.0;

float move_scale = 0; //定航时前进方向动力系数

//俯仰使能
u8 pitch_en =0;
u8 deep_en = 0;
u8 head_en = 1; //使用了浮块，不需要定深度和俯仰角，所以只开启朝向使能
//u8 pitch_en =0;
//u8 deep_en = 0;
//u8 head_en =0;


u16 xolotest=0;

SOTF_typeDef RockerYAWGs;

void control_Task_Function(void const * argument)
{

    portTickType tick = xTaskGetTickCount();
    NLPID_Init();
    //非线性PID初始化
    Rocker_SOTF_Init(&RockerYAWGs);
    while(1)
    {
#ifdef DEBUG

#else
        if(xQueueReceive(Control_Message_Queue,&(control_data),portMAX_DELAY))
        {
//			SetUpDowm(100,100);
//			Horizontal_Control(100,0);

//            //俯仰角 PID 调节
//            if(pitch_en==1) {
//                pitching_pid_out = Incremental_PID_Cal(0,control_data.pitching,&Pitch_pid);
//                SetUpDowm(-0.5*pitching_pid_out,0.8*pitching_pid_out);
//            } else {
//                SetUpDowm(0,0);
//            }

//            //深度 PID 调节
//            if(deep_en==1) {
//                //deepth_pid_out = Incremental_PID_Cal(deep_tar,control_data.depth,&Deep_pid);
//                deepth_pid_out = Position_PID_Cal(deep_tar,control_data.depth,&Deep_pid);
//                SetUpDowm(0.8*deepth_pid_out,0.8*deepth_pid_out);
//            } else {
//                SetUpDowm(0,0);
//            }
//
//		//定航PID
            if(head_en==1) {
                heading_pid_out = YAWPID_Control(&YawNLPID,control_data.navi_angle,control_data.navi_distance,heading_pid_tar);
                heading_pid_out = CONSTRAIN(heading_pid_out, 12, -12);  				//限幅

                move_scale = (1 - fabs(heading_pid_out)/10.f)*0.1; 									//定航时前进方向动力系数
                //move_scale = 0;
                Horizontal_Control(2*move_scale*test_motor,0.7*heading_pid_out);	//导航速度设置
            } else {
                Horizontal_Control(0,0);
            }

        }
#endif
    }
}

