/**
  ******************************************************************************
  * @file    board.c
  * @author  LiuYang
  * @brief   bsp
  ******************************************************************************
  * @attention
  *	UART1 --> lora
	* UART2 --> ��ݮ��
	* UART3 --> mpu6050
	* SPI1  --> MS5803
  ******************************************************************************
  */
#include "board.h"

//������Ϣ����
QueueHandle_t MPU6050_Rec_Queue = NULL;
QueueHandle_t MPU6050_Msg_Queue = NULL;
QueueHandle_t Control_Message_Queue = NULL;  
QueueHandle_t Command_Message_Queue = NULL;  
QueueHandle_t Pressure_Message_Queue = NULL; 
QueueHandle_t TempHum_Message_Queue = NULL;
QueueHandle_t Battery_Message_Queue = NULL; 

/**
 * @funNm : delay_ms
 * @brief : ��ʱ����
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
 * @brief : ��ʱnus
 * @param : nus:Ҫ��ʱ��us��.	0~204522252(���ֵ��2^32/fac_us@fac_us=168)	 
 * @retval: void
 */
void delay_sys_us(uint32_t Delay)//1��delay�����1.5us
{
		uint32_t cnt = Delay * 8;
    //uint32_t cnt = Delay * 35;
    uint32_t i = 0;
    for(i = 0; i < cnt; i++)__NOP();
}

/**
 * @funNm : Task_Queue_Semaphore_Timers_Create
 * @brief : ������Ϣ���С��ź�������ʱ������
 * @param : void
 * @retval: 1.�ڸ�������ִ��ǰ����  2.ע���ֹ�жϴ�ϴ���
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
 * @brief : �����жϺ���
 * @param : huart ĳ����
 * @retval: void
 */
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
		if(huart->Instance == USART1)//mpu6050
		{
			HAL_MPU_To_UART_Receive_IDLE(huart);
		}
		if(huart->Instance == USART2)//��ݮ��
		{
			HAL_PI_To_UART_Receive_IDLE(huart);
		}
		if(huart->Instance == USART3)//Loraģ��
		{
			HAL_LORA_To_UART_Receive_IDLE(huart);
		}
		if(huart->Instance == UART4)
		{
			
		}
}

/**
  * @funNm   BSP_Init
  * @brief   ���������ʼ��
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
		MS5803aInit(); //ѹ����ʼ��
		DHT11_init();
		qmc_init();			//�����Ƴ�ʼ��
		PID_Init();
		Stm32_Pi_Init();

		
	/*********Test Demo ֻ֧�ֵ�һģ�����*********/
//	LED_Test_Demo();     		//led ��demo
//	MS5803_Test_Demo();  		//ѹ������demo
//	SW_Test_Demo();      		//��������demo
//	PWM_Test_Demo();     		//���PWM����demo
//	Batt_Test_Demo();    		//����demo
//  DHT11_Test_Demo();      //DHT11��ʪ��demo  
//	MPU6050_Test_Demo();   		//��������demo
//	PI_STM32_Test_Demo();		//��ݮ�ɴ���demo
//	QMC5883L_Test_Demo();	//������demo
//	SX1278_Test_Demo();	//LORA����
}

/**
  * @funNm   HAL_GPIO_EXTI_Callback
  * @brief   �ⲿ�жϻص�����
  * @param	 GPIO_Pin ����
  * @retval  void
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
		if(HAL_GPIO_ReadPin(POWER_KEY_STATE_GPIO_Port,POWER_KEY_STATE_Pin) == GPIO_PIN_RESET)
		{
			//HAL_GPIO_TogglePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin);
			POWER_KEY(0);//�ػ�	
			delay_ms(50);
	}
}
	


