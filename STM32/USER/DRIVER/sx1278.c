#include "sx1278.h"
/**
  ******************************************************************************
  * @file    sx1278.c
  * @author  LiuYang
  * @brief   Loraģ��
  *
  *
  ******************************************************************************
  * @attention
  *	Loraģ�� ����3 Ĭ�ϲ�����: 9600
	* E32-170T30D 170MHz 0x0000 0x28 2.4kbps 9600 8N1 1W
  *	ģ�������ֹ���ģʽ�������� M0�� M1 ���ã���ϸ������±���ʾ��
	*	ģʽ��0-3��       M0      M1                  ģʽ����                                               ��ע
	*	0 һ��ģʽ        0       0            ���ڴ򿪣����ߴ򿪣�͸������                           ���շ�������ģʽ 0�� 1
	*
	*	1 ����ģʽ        1       0						 ���ڴ򿪣����ߴ򿪣�                                  ���շ�������ģʽ 0
	*	                                       ��ģʽ 0 Ψһ�������ݰ�����ǰ���Զ����ӻ����룬       ���շ�������ģʽ 1
	*                                        �������ܻ��ѹ�����ģʽ 2 �Ľ��շ�
  *
	*	2 ʡ��ģʽ        0       1            ���ڽ��չرգ����ߴ��ڿ��л���ģʽ���յ�������          ���䷽����ģʽ 1
	* 																			 �ݺ󣬴򿪴��ڷ������ݡ�                               ��ģʽ�²��ܷ���
	* 
	* 
	* 3 ����ģʽ        1        1           ģ��������ߣ����Խ��ղ�����������                     
	��������������
  *
  ******************************************************************************
  */
SX1278_Parse_State_t sx1278_parse; 
SX1278_Info_t        sx1278_info;
UsartRec_t 						SX1278_Rec;

 /**
 * @funNm : SX1278_GetWorkingParam
 * @brief : LORAģ���ʼ��,����Ϊ����ģʽ�����в������ã�ģ�鷵���ѱ���Ĳ�������Ϊ��ȡ����״̬����
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
 * @brief : ���ù���״̬����,������Ϣ����ַ����Ϊ0x00 00������У��λΪ8N1��TTL��������Ϊ9600bps��ͨ�����߿�������Ϊ2.4kbps	
 * @param : void
 * @retval: void
 */
void SX1278_SetWorkingParam(void)
{
	u8 cmd[6] = {0xC0,0x04,0x03,0x1B,0x28,0x47};	//������Ϣ,0xC0+�������������еڶ����ֽڱ�ʾ���ֽں͵��ֽ��豸��ַ�������ֽڱ�ʾ�ŵ�����54��,00H-36H;
																								//͸��ͨ��״̬�£���Ŀ���ַ�ŵ���ȫһ��ʱ����ʵ��1��1ͨ�ţ�������֮����Ҫ������ţ���ý���ַ�ŵ��������ظ�
																								//���E32-170T30DоƬ�ֲ������
	HAL_GPIO_WritePin(LORA_M0_GPIO_Port,LORA_M0_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LORA_M1_GPIO_Port,LORA_M1_Pin,GPIO_PIN_SET);
	HAL_Delay(500);	
	HAL_UART_Transmit(&SX1278_USART, cmd, 6, 0x100);
	HAL_Delay(500);	
}
 /**
 * @funNm : SX1278_Init
 * @brief : LORAģ���ʼ��,�����ù�������
 * @param : void
 * @retval: void
 */
void SX1278_Init(void)
{
 HAL_UART_Receive_DMA(&SX1278_USART, SX1278_Rec.RX_pData, sizeof(SX1278_Rec.RX_pData));  
	__HAL_UART_ENABLE_IT(&SX1278_USART, UART_IT_IDLE); 
	
	HAL_Delay(500);		
	
	SX1278_SetWorkingParam();//�ɲ���ÿ�ζ�ִ�����ù���
	SX1278_GetWorkingParam();
	//����Ϊһ��ģʽ��M0=0��M1=0;
	HAL_GPIO_WritePin(LORA_M0_GPIO_Port,LORA_M0_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LORA_M1_GPIO_Port,LORA_M1_Pin,GPIO_PIN_RESET);
	HAL_Delay(500);	
}

 /**
 * @funNm : unpackData
 * @brief : ���ݽ���
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
 * @brief : LORA���ڽ��պ���
 * @param : huart
 * @retval: void
 */
static uint32_t temp;
void HAL_LORA_To_UART_Receive_IDLE(UART_HandleTypeDef *huart)//������Ҫ����
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
 * @brief : ���ͺ���
 * @param : buf[]:����    len:����
 * @retval: void
 */
void SX1278_SendData(uint8_t buf[],u16 len)
{
	HAL_UART_Transmit(&SX1278_USART,buf, len ,0x100);
}

/**
 * @funNm : SX1278_Test_Demo
 * @brief : Lora���Գ���
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
//	//����Ϊһ��ģʽ��M0=0��M1=0;
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