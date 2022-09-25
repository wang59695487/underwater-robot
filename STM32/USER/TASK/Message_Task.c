#include "Message_Task.h"
#include "Sensor_Task.h"

static ControlMsg_t control_data;

float navi_center_to_line_angle = 0.0;   //中心到色带角度
float navi_center_to_line_distance = 0.0;//中心到色带距离
int16_t front_speed = 0;
char temp_buf[50]= {0};

char temp_buf1[100]= {0};
char temp_buf3[100]= {0};
char temp_buf4[50]= {0};
char temp_buf5[50]= {0};
char temp_buf6[50]= {0};
char temp_buf7[50]= {0};

int led_flag;

char receive_letter[4]= {'E','B','A','C'}; //接收到的字符

u32 pack_num = 0;
//消息任务功能
void message_Task_Function(void const * argument)
{
    BaseType_t err;
    portTickType tick = xTaskGetTickCount();
    /* Infinite loop */
    for(;;)
    {
#ifdef DEBUG
        //HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin);
#else

        xQueuePeek(TempHum_Message_Queue, &SensorData.TempHum_Data,1); //温湿度
        xQueuePeek(MPU6050_Rec_Queue, &SensorData.MPU6050_Data,1); //电子罗盘
        xQueuePeek(Pressure_Message_Queue, &SensorData.Pressure_Data,1); //压传
        xQueuePeek(Battery_Message_Queue, &SensorData.Batt_Data,1); //电池

        control_data.yawing = SensorData.MPU6050_Data.yaw;
        control_data.pitching = SensorData.MPU6050_Data.pitch+1.90;//+1.50(1号);
        control_data.rolling = SensorData.MPU6050_Data.roll-3.53;
        control_data.pitch_groy = SensorData.MPU6050_Data.pitch_groy;
        control_data.roll_groy = SensorData.MPU6050_Data.roll_groy;
        control_data.yaw_groy = SensorData.MPU6050_Data.yaw_groy;

        control_data.depth = SensorData.Pressure_Data.depth;
        control_data.temperture = SensorData.Pressure_Data.Temperature;

        Adder_Control_Disable_Counter();
        control_data.Control_Disable = Get_Control_Disable_Counter_State();

        if(Control_Message_Queue!=NULL)//发给控制任务
        {
            err=xQueueSend(Control_Message_Queue,&control_data,1);	// 手柄控制
            if(err==errQUEUE_FULL)
            {
                ;//printf("队列Key_Queue已满，数据发送失败!\r\n");
            }
        }

        static int time = 0;
        if(time>10) {
            time = 0;
            sprintf(temp_buf,"PI-dis=%.2f,ang = %.2f\r\n",control_data.navi_distance,control_data.navi_angle);

           // sprintf(temp_buf1,"PRESS-tem=%.2f,dep = %.2f \r\n",control_data.temperture,control_data.depth);
           // sprintf(temp_buf3,"JY901-head=%.2f,pitch = %.2f,roll=%.2f \r\n",control_data.yawing,control_data.pitching,control_data.rolling);
           // sprintf(temp_buf5,"SHT35-tem=%.2f,hum = %.2f\r\n",SensorData.TempHum_Data.temp,SensorData.TempHum_Data.humi);
            sprintf(temp_buf6,"BAT-vol=%.2f\r\n\n", SensorData.Batt_Data.voltage);
					  sprintf(temp_buf7,"letters=%s\r\n\n", receive_letter);

            SX1278_SendData((uint8_t*)temp_buf,strlen(temp_buf));

           // SX1278_SendData((uint8_t*)temp_buf1,strlen(temp_buf1));
           // SX1278_SendData((uint8_t*)temp_buf3,strlen(temp_buf3));
            SX1278_SendData((uint8_t*)temp_buf4,strlen(temp_buf4));
          //  SX1278_SendData((uint8_t*)temp_buf5,strlen(temp_buf5));
            SX1278_SendData((uint8_t*)temp_buf6,strlen(temp_buf6));

            //Added by Liwenwen
            SX1278_SendData((uint8_t*)temp_buf7,strlen(temp_buf7));

            //LED_Show();     		//led灯传输数据过程中闪烁

        }
        else {
            time ++;
        }
#endif
        vTaskDelayUntil(&tick,100);
    }
}
float p_val = 0.0;
float i_val = 0.0;
float d_val = 0.0;


void SX1278_unpackData(uint8_t *buf,u16 len)
{
    if(strstr((char*)buf,"P:")) {
//		p_val = atof(buf);
    }
    if(strstr((char*)buf,"I:")) {

    }
    if(strstr((char*)buf,"D:")) {

    }

}
//树莓派和stm32之间协议解析
void Pi_Stm32_unpackData(uint8_t *buf,u16 len)
{
    u8 step = 0;
    for(int i =0; i<len; i++)
    {
        switch(step)
        {
        case 0:                      //检测包头
            if(buf[i]==0xFE) {
                step = 1;
            }
            break;
        case 1:
            if(buf[i]<=(len-i+1)) {
                if(buf[buf[i]-1+(i-1)] == 0x0a) { //检测包长
                    step = 2;
                }
            } else {
                step = 0;
            }
            break;
        case 2:
            i++;
            switch(buf[i])
            {
            case MSG_NAVI_ID:
                memcpy((void*)&navi_center_to_line_angle,(void*)&(buf[i+1]),4);
                memcpy((void*)&navi_center_to_line_distance,(void*)&(buf[i+5]),4);
                control_data.navi_distance = navi_center_to_line_distance;
                control_data.navi_angle    = navi_center_to_line_angle;
                Clear_Control_Disable_Counter();
                break;
                // Added by Liwenwen
            case MSG_DAKA_ID:
                led_flag=1;
                break;
						case MSG_LETTER_ID:
							  memcpy((void*)&receive_letter[0],(void*)&(buf[i+1]),1);
						    memcpy((void*)&receive_letter[1],(void*)&(buf[i+2]),1);
						    memcpy((void*)&receive_letter[2],(void*)&(buf[i+3]),1);
						    memcpy((void*)&receive_letter[3],(void*)&(buf[i+4]),1);
							  break;

            default:
                break;
            }
            step = 0;
            break;
        }
    }
}
