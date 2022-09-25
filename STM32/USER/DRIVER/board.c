/**
  ******************************************************************************
  * @file    board.c
  * @author  LiuYang
  * @brief   bsp
  ******************************************************************************
  * @attention
  *	UART1 --> lora
	* UART2 --> 树莓派
	* UART3 --> mpu6050
	* SPI1  --> MS5803
  ******************************************************************************
  */
#include "board.h"

//定义消息队列
QueueHandle_t MPU6050_Rec_Queue = NULL;
QueueHandle_t MPU6050_Msg_Queue = NULL;
QueueHandle_t Control_Message_Queue = NULL;  
QueueHandle_t Command_Message_Queue = NULL;  
QueueHandle_t Pressure_Message_Queue = NULL; 
QueueHandle_t TempHum_Message_Queue = NULL;
QueueHandle_t Battery_Message_Queue = NULL; 

/**
 * @funNm : delay_ms
 * @brief : 延时函数
 * @param : nms
 * @retval: void
 */
void delay_ms(int nms)   
{
		if(xTaskGetSchedulerState()==taskSCHEDULER_NOT_STARTED)
		{
			HAL_Delay(nms);
		}else
		{
			osDelay(nms);
		}
}

/**
 * @funNm : delay_sys_us
 * @brief : 延时nus
 * @param : nus:要延时的us数.	0~204522252(最大值即2^32/fac_us@fac_us=168)	 
 * @retval: void
 */
void delay_sys_us(uint32_t Delay)//1个delay，大概1.5us
{
		uint32_t cnt = Delay * 8;
    //uint32_t cnt = Delay * 35;
    uint32_t i = 0;
    for(i = 0; i < cnt; i++)__NOP();
}

/**
 * @funNm : Task_Queue_Semaphore_Timers_Create
 * @brief : 任务消息队列、信号量、定时器创建
 * @param : void
 * @retval: 1.在各个任务执行前创建  2.注意防止中断打断创建
 */
void Task_Queue_Semaphore_Timers_Create(void)
{
		do{Pressure_Message_Queue = xQueueCreate(1, sizeof(PressureMsg_t));
		}while(Pressure_Message_Queue==NULL);
		
		do{
			MPU6050_Rec_Queue = xQueueCreate(1, sizeof(uint32_t));
		}while(MPU6050_Rec_Queue==NULL);
			
		do{
			MPU6050_Msg_Queue = xQueueCreate(1,sizeof(MPU6050Msg_t));
		}while(MPU6050_Msg_Queue == NULL);
		
		do{
			Control_Message_Queue = xQueueCreate(3, sizeof(ControlMsg_t));
		}while(Control_Message_Queue==NULL);
		
		do{
			TempHum_Message_Queue = xQueueCreate(1, sizeof(TempHumiMsg_t));
		}while(TempHum_Message_Queue==NULL);
		
		do{
				Battery_Message_Queue = xQueueCreate(1, sizeof(uint32_t));	
		}while(Battery_Message_Queue==NULL);
	
}

/**
 * @funNm : USER_UART_IRQHandler
 * @brief : 串口中断函数
 * @param : huart 某串口
 * @retval: void
 */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
		if(huart->Instance == USART1)//mpu6050
		{
			HAL_MPU_To_UART_Receive_IDLE(huart);
		}
		if(huart->Instance == USART2)//树莓派
		{
			HAL_PI_To_UART_Receive_IDLE(huart);
		}
		if(huart->Instance == USART3)//Lora模块
		{
			HAL_LORA_To_UART_Receive_IDLE(huart);
		}
		if(huart->Instance == UART4)
		{
			
		}
}

/**
  * @funNm   BSP_Init
  * @brief   所有外设初始化
  * @param	 void
  * @retval  void
*/
void BSP_Init(void)
{	
		PowerON_SwitchSelfHold();
		PWM_Init();
		Batt_Init();
		SX1278_Init();//		lora_init(0,0); //id:0; chain: 0.
		MPU6050_Init();
		MS5803aInit(); //压传初始化
		DHT11_init();
		qmc_init();			//磁力计初始化
		PID_Init();
		Stm32_Pi_Init();

		
	/*********Test Demo 只支持单一模块测试*********/
//	LED_Test_Demo();     		//led 灯demo
//	MS5803_Test_Demo();  		//压传测试demo
//	SW_Test_Demo();      		//自锁开关demo
//	PWM_Test_Demo();     		//电调PWM测试demo
//	Batt_Test_Demo();    		//电量demo
//  DHT11_Test_Demo();      //DHT11温湿度demo  
//	MPU6050_Test_Demo();   		//电子罗盘demo
//	PI_STM32_Test_Demo();		//树莓派串口demo
//	QMC5883L_Test_Demo();	//磁力计demo
//	SX1278_Test_Demo();	//LORA测试
}

/**
  * @funNm   HAL_GPIO_EXTI_Callback
  * @brief   外部中断回调函数
  * @param	 GPIO_Pin 引脚
  * @retval  void
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
		if(HAL_GPIO_ReadPin(POWER_KEY_STATE_GPIO_Port,POWER_KEY_STATE_Pin) == GPIO_PIN_RESET)
		{
			//HAL_GPIO_TogglePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin);
			POWER_KEY(0);//关机	
			delay_ms(50);
	}
}
	


