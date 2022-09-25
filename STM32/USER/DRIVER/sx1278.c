#include "sx1278.h"
/**
  ******************************************************************************
  * @file    sx1278.c
  * @author  LiuYang
  * @brief   Lora模块
  *
  *
  ******************************************************************************
  * @attention
  *	Lora模块 串口3 默认波特率: 9600
	* E32-170T30D 170MHz 0x0000 0x28 2.4kbps 9600 8N1 1W
  *	模块有四种工作模式，由引脚 M0、 M1 设置；详细情况如下表所示：
	*	模式（0-3）       M0      M1                  模式介绍                                               备注
	*	0 一般模式        0       0            串口打开，无线打开，透明传输                           接收方必须是模式 0、 1
	*
	*	1 唤醒模式        1       0						 串口打开，无线打开；                                  接收方可以是模式 0
	*	                                       和模式 0 唯一区别：数据包发射前，自动增加唤醒码，       接收方可以是模式 1
	*                                        这样才能唤醒工作在模式 2 的接收方
  *
	*	2 省电模式        0       1            串口接收关闭，无线处于空中唤醒模式，收到无线数          发射方必须模式 1
	* 																			 据后，打开串口发出数据。                               该模式下不能发射
	* 
	* 
	* 3 休眠模式        1        1           模块进入休眠，可以接收参数设置命令                     
	详见工作参数详解
  *
  ******************************************************************************
  */
SX1278_Parse_State_t sx1278_parse; 
SX1278_Info_t        sx1278_info;
UsartRec_t 						SX1278_Rec;

 /**
 * @funNm : SX1278_GetWorkingParam
 * @brief : LORA模块初始化,配置为休眠模式，进行参数配置，模块返回已保存的参数，即为获取工作状态参数
 * @param : void
 * @retval: void
 */
void SX1278_GetWorkingParam(void)
{
	u8 cmd[3] = {0xC1,0xC1,0xC1};		
	HAL_GPIO_WritePin(LORA_M0_GPIO_Port,LORA_M0_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LORA_M1_GPIO_Port,LORA_M1_Pin,GPIO_PIN_SET);
	HAL_Delay(500);	
	HAL_UART_Transmit(&SX1278_USART, cmd, 3, 0x100);
	HAL_Delay(500);	
	sx1278_parse = SX1278_GET_WORK_STA;
}
 /**
 * @funNm : SX1278_SetWorkingParam
 * @brief : 设置工作状态参数,配置信息，地址配置为0x00 00；串口校验位为8N1；TTL串口速率为9600bps；通用无线空中速率为2.4kbps	
 * @param : void
 * @retval: void
 */
void SX1278_SetWorkingParam(void)
{
	u8 cmd[6] = {0xC0,0x04,0x03,0x1B,0x28,0x47};	//配置信息,0xC0+工作参数，其中第二三字节表示高字节和低字节设备地址，第五字节表示信道，共54个,00H-36H;
																								//透明通信状态下，当目标地址信道完全一致时，可实现1对1通信，若多组之间需要避免干扰，最好将地址信道都避免重复
																								//详见E32-170T30D芯片手册第七章
	HAL_GPIO_WritePin(LORA_M0_GPIO_Port,LORA_M0_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LORA_M1_GPIO_Port,LORA_M1_Pin,GPIO_PIN_SET);
	HAL_Delay(500);	
	HAL_UART_Transmit(&SX1278_USART, cmd, 6, 0x100);
	HAL_Delay(500);	
}
 /**
 * @funNm : SX1278_Init
 * @brief : LORA模块初始化,并配置工作参数
 * @param : void
 * @retval: void
 */
void SX1278_Init(void)
{
 HAL_UART_Receive_DMA(&SX1278_USART, SX1278_Rec.RX_pData, sizeof(SX1278_Rec.RX_pData));  
	__HAL_UART_ENABLE_IT(&SX1278_USART, UART_IT_IDLE); 
	
	HAL_Delay(500);		
	
	SX1278_SetWorkingParam();//可不用每次都执行配置过程
	SX1278_GetWorkingParam();
	//配置为一般模式，M0=0；M1=0;
	HAL_GPIO_WritePin(LORA_M0_GPIO_Port,LORA_M0_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LORA_M1_GPIO_Port,LORA_M1_Pin,GPIO_PIN_RESET);
	HAL_Delay(500);	
}

 /**
 * @funNm : unpackData
 * @brief : 数据解析
 * @param : *buf; len
 * @retval: void
 */
static void unpackData(uint8_t *buf,u16 len)
{
	switch(sx1278_parse)
	{
		case SX1278_GET_WORK_STA:
		if(buf[0]==0xC0){
			sx1278_info.address = buf[1]<<8|buf[2];
			sx1278_info.baud = buf[1]<<8|buf[2];
		}
		break;
		case SX1278_GET_VERSION:break;
		case SX1278_RESET:break;
		default:break;
	}
}

void SX1278_GetData()
{

	 
}

/**
 * @funNm : HAL_LORA_To_UART_Receive_IDLE
 * @brief : LORA串口接收函数
 * @param : huart
 * @retval: void
 */
static uint32_t temp;
void HAL_LORA_To_UART_Receive_IDLE(UART_HandleTypeDef *huart)//可能需要更改
{
//	
//  HAL_UART_Receive_IT(huart,aRxBuffer,1);          // Enable the USART5 Interrupt
  if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))  
    {   
			__HAL_UART_CLEAR_IDLEFLAG(huart);  
			HAL_UART_DMAStop(huart);  
			temp = huart->hdmarx->Instance->CNDTR;  
			SX1278_Rec.RX_Size =  RX_LEN - temp;   
			SX1278_Rec.RX_flag=1;  
			SX1278_unpackData(SX1278_Rec.RX_pData,SX1278_Rec.RX_Size);
		 //	memset(SX1278_Rec.RX_pData,'\0',RX_LEN);
		
			HAL_UART_Receive_DMA(huart,SX1278_Rec.RX_pData,sizeof(SX1278_Rec.RX_pData));  
    }
} 
/**
 * @funNm : SX1278_SendData
 * @brief : 发送函数
 * @param : buf[]:数据    len:长度
 * @retval: void
 */
void SX1278_SendData(uint8_t buf[],u16 len)
{
	HAL_UART_Transmit(&SX1278_USART,buf, len ,0x100);
}

/**
 * @funNm : SX1278_Test_Demo
 * @brief : Lora测试程序
 * @param : void
 * @retval: void
 */
void SX1278_Test_Demo(void)
{
	SX1278_Init();
	while(1)
	{
		HAL_UART_Transmit(&SX1278_USART, (uint8_t*)"123", 3, 0x100);
		HAL_Delay(500);		
//		SX1278_GetData();
	}

}

//void SX1278_Test_Demo(void)
//{
////	SX1278_Init();
//	HAL_Delay(500);		
//	
//	SX1278_SetWorkingParam();
//	SX1278_GetWorkingParam();
//	//配置为一般模式，M0=0；M1=0;
//	HAL_GPIO_WritePin(LORA_M0_GPIO_Port,LORA_M0_Pin,GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LORA_M1_GPIO_Port,LORA_M1_Pin,GPIO_PIN_RESET);
//	HAL_Delay(500);		
//	
////	while(1)
////	{
////		HAL_UART_Transmit(&SX1278_USART, (uint8_t*)"123", 3, 0x100);
////		HAL_Delay(500);		
////		SX1278_GetData();
////	}
//}