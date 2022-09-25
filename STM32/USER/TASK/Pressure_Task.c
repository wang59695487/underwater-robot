#include "Pressure_Task.h"
#include "Sensor_Task.h"

void pressure_Task_Function(void const * argument)
{
    //portTickType tick = xTaskGetTickCount();
    while(1)
    {
#ifdef DEBUG

#else
        Get_Pressure(&SensorData.Pressure_Data);	//获取压力信息
        xQueueOverwrite(Pressure_Message_Queue, &SensorData.Pressure_Data);
#endif
        osDelay(100);
        //vTaskDelayUntil(&tick,200);
    }
}


