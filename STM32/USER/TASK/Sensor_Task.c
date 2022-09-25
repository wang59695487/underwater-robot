#include "Sensor_Task.h"

SensorMsg_t SensorData;
void sensor_Task_Function(void *pvParameters)
{
    //portTickType tick = xTaskGetTickCount();
    while(1)
    {
#ifdef DEBUG

#else
        DHT11_Read_Data(&SensorData.TempHum_Data);  //读取温湿度值
        Get_Batt_Data(&SensorData.Batt_Data);    //读取电压值
        xQueueOverwrite(TempHum_Message_Queue, &SensorData.TempHum_Data);
        xQueueOverwrite(Battery_Message_Queue, &SensorData.Batt_Data);

#endif
        osDelay(1000);
        //vTaskDelayUntil(&tick,1000);
    }
}

