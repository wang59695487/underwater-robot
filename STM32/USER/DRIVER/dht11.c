/**
  ******************************************************************************
  * @file    DHT11.c
  * @author  LiuYang
  * @brief   ��ʪ�ȴ�����
  ******************************************************************************
  * @attention
  *	 
	*  
	* 
  ******************************************************************************
  */
#include "dht11.h"

DHT11_Data_TypeDef DHT11_Data;
 /**
 * @funNm : DHT11_init
 * @brief : DHT11��ʼ��
 * @param : void
 * @retval: void
 */
void DHT11_init(void)
{		
	  //����DHT11
		HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
}

/**
 * @funNm : DHT11_Mode_IPU
 * @brief : ʹDHT11-DATA���ű�Ϊ��������ģʽ
 * @param : void
 * @retval: void
 */
static void DHT11_Mode_IPU(void)
{
 	 GPIO_InitTypeDef GPIO_InitStruct = {0};
	 
	 //ѡ��Ҫ���Ƶ�DHT11_PORT����
	 GPIO_InitStruct.Pin = DHT11_Pin;
	 
   //��������ģʽΪ��������ģʽ
	 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	 
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 
	 //���ÿ⺯������ʼ��DHT11_PORT 
	 HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);	 
}

/**
 * @funNm : DHT11_Mode_Out_PP
 * @brief : ʹDHT11-DATA���ű�Ϊ�������ģʽ
 * @param : void
 * @retval: void
 */
static void DHT11_Mode_Out_PP(void)
{
 	 GPIO_InitTypeDef GPIO_InitStruct = {0};
	 
	 //ѡ��Ҫ���Ƶ�DHT11_PORT����
	 GPIO_InitStruct.Pin = DHT11_Pin;
	 
   //��������ģʽΪͨ���������
	 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	 
	 //������������Ϊ50MHz
	 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	 
	 //���ÿ⺯������ʼ��DHT11_PORT 
	 HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);		 
}

/**
 * @funNm : Read_Byte
 * @brief : ��DHT11��ȡһ���ֽڣ�MSB����
 * @param : void
 * @retval: temp
 */
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		//ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����
		while(DHT11_DATA_IN()==GPIO_PIN_RESET);

		//DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
		delay_sys_us(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	  
 	  
    //x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� 
		if(DHT11_DATA_IN()==GPIO_PIN_SET) 
		{
			//�ȴ�����1�ĸߵ�ƽ����
			while(DHT11_DATA_IN()==GPIO_PIN_SET);

			//�ѵ�7-iλ��1��MSB���� 
			temp|=(uint8_t)(0x01<<(7-i));  
		}
		else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{			   
			//�ѵ�7-iλ��0��MSB����
			temp&=(uint8_t)~(0x01<<(7-i)); 
		}
	}
	return temp;
}

/**
 * @funNm : Read_DHT11
 * @brief : һ�����������ݴ���Ϊ40bit����λ�ȳ�
						8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
 * @param : DHT11_Data
 * @retval: SUCCESS / ERROR
 */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	DHT11_Mode_Out_PP();	//���ģʽ
	DHT11_DATA_OUT(LOW);	//��������
	delay_ms(18);//��ʱ18ms
	DHT11_DATA_OUT(HIGH); //�������� ������ʱ30us
	delay_sys_us(30); //��ʱ30us
	DHT11_Mode_IPU();//������Ϊ���� �жϴӻ���Ӧ�ź�
	
	
	if(DHT11_DATA_IN()==GPIO_PIN_RESET)   //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������   
	{
		//��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���
		while(DHT11_DATA_IN()==GPIO_PIN_RESET);
		
		//��ѯֱ���ӻ������� 80us �ߵ�ƽ �����źŽ���
		while(DHT11_DATA_IN()==GPIO_PIN_SET);

		//��ʼ�������� 
		DHT11_Data->humi_int= Read_Byte();
		DHT11_Data->humi_deci= Read_Byte();
		DHT11_Data->temp_int= Read_Byte();
		DHT11_Data->temp_deci= Read_Byte();
		DHT11_Data->check_sum= Read_Byte();
		
		//��ȡ���������Ÿ�Ϊ���ģʽ
		DHT11_Mode_Out_PP();
		
		//��������
		DHT11_DATA_OUT(HIGH);
		
		//����ȡ�������Ƿ���ȷ
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	else
	{		
		return ERROR;
	}   
}

/**
 * @funNm : DHT11_Read_Data
 * @brief : DHT11��ȡ����,����ֻ��ȡ������λ
 * @param : void
 * @retval: SUCCESS / ERROR
 */
bool DHT11_Read_Data(TempHumiMsg_t *TempHum)
{
	if( Read_DHT11(&DHT11_Data)==SUCCESS)
	{
		TempHum->humi = DHT11_Data.humi_int;
		TempHum->temp = DHT11_Data.temp_int;
		return SUCCESS;
	}else
	{
		return ERROR;
	}
}

/**
 * @funNm : DHT11_Test_Demo
 * @brief : DHT11����demo
 * @param : void
 * @retval: void
 */
TempHumiMsg_t TempHum;
void DHT11_Test_Demo(void)
{	
		//extern TempHumiMsg_t TempHum;
		DHT11_init();
		while(1)
		{
			//��ȡ��ʪ��ֵ	
			DHT11_Read_Data(&TempHum);  
			
			//ÿ100ms��ȡһ��
			delay_ms(100);  
		}
}
