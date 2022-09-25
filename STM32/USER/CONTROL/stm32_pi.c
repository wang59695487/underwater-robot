/**
  ******************************************************************************
  * @file    stm32_pi.c
  * @author  
  * @brief   stm32_pi
  ******************************************************************************
  * @attention
	* 
	*
  ******************************************************************************
  */
#include "stm32_pi.h"

PI_UsartRec_t stm32_pi_Rec;
/**
  * @funNm   Stm32_Pi_Init
  * @brief   树莓派串口初始化
  * @param	 void
  * @retval  void
*/
void Stm32_Pi_Init(void)
{
		HAL_UART_Receive_DMA(&PI_USART, stm32_pi_Rec.RX_pData, PI_RX_LEN);  
		__HAL_UART_ENABLE_IT(&PI_USART, UART_IT_IDLE); 
}
 
/**
  * @funNm   HAL_PI_To_UART_Receive_IDLE
  * @brief   树莓派串口接收函数
  * @param	 huart
  * @retval  void
*/
void HAL_PI_To_UART_Receive_IDLE(UART_HandleTypeDef *huart)
{
  	uint32_t temp;
 
		if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))  
    {   
        __HAL_UART_CLEAR_IDLEFLAG(huart);  
        HAL_UART_DMAStop(huart);  
        temp = huart->hdmarx->Instance->CNDTR;  
        stm32_pi_Rec.RX_Size =  PI_RX_LEN - temp;   
        stm32_pi_Rec.RX_flag=1;  
 			  Pi_Stm32_unpackData(stm32_pi_Rec.RX_pData,stm32_pi_Rec.RX_Size);
		   	memset(stm32_pi_Rec.RX_pData,'\0',PI_RX_LEN);
        HAL_UART_Receive_DMA(huart,stm32_pi_Rec.RX_pData,sizeof(stm32_pi_Rec.RX_pData));  
    }
} 

/**
  * @funNm   PI_STM32_Test_Demo
  * @brief   树莓派测试函数
  * @param	 huart
  * @retval  void
*/
void PI_STM32_Test_Demo(void)
{
		Stm32_Pi_Init();
		
		while(1)
		{
			Stm32_Pi_Init();
			HAL_UART_Transmit(&PI_USART, (uint8_t*)"123", 3, 0x100);
			delay_ms(500);
		}
}

/**
  * @funNm   SendToPi
  * @brief   stm32 -> Pi  数据上传函数  发往UART
  * @param	 id  data  len
  * @retval  void
*/
uint8_t send_buf[30]={0};
void SendToPi(uint8_t id,uint8_t* data,uint8_t len)
{
    static uint8_t count = 0;
	  send_buf[0] = 0xfe;
    send_buf[1] = 7+len;
    send_buf[2] = count;
    send_buf[3] = id;
    for(uint8_t i=0;i<len;i++)
    {
        send_buf[4+i] = *(data+i);
    }
    send_buf[len+6] = 0x0a;
//		HAL_UART_Transmit(&PI_USART, (uint8_t*)"123", 3, 0x100);
		//Send_string_xlen(USART2,send_buf, send_buf[1]);
}




