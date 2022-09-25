#ifndef __DHT11_H
#define __DHT11_H 
#include "includes.h" 
#define HIGH  1
#define LOW   0

//���κ꣬��������������һ��ʹ��,����ߵ�ƽ��͵�ƽ
#define DHT11_DATA_OUT(a)	if (a)	\
														HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);\
													else		\
														HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET)
//��ȡ���ŵĵ�ƽ
#define  DHT11_DATA_IN()	  HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)
//��ʪ������(��С��)
typedef struct
{
	uint8_t  humi_int;		//ʪ�ȵ���������
	uint8_t  humi_deci;	 	//ʪ�ȵ�С������
	uint8_t  temp_int;	 	//�¶ȵ���������
	uint8_t  temp_deci;	 	//�¶ȵ�С������
	uint8_t  check_sum;	 	//У���
}DHT11_Data_TypeDef;
//��ʪ�Ƚṹ��
typedef struct
{
	float  temp;		//�¶�
	float  humi;	 	//ʪ��
}TempHumiMsg_t;
//��������
void DHT11_init(void);	//DHT11��ʼ��
static void DHT11_Mode_IPU(void);	//ʹDHT11-DATA���ű�Ϊ��������ģʽ
static void DHT11_Mode_Out_PP(void);//ʹDHT11-DATA���ű�Ϊ�������ģʽ
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);//��ʪ�ȶ�ȡ����
static uint8_t Read_Byte(void);//��DHT11��ȡһ���ֽڣ�MSB����
bool DHT11_Read_Data(TempHumiMsg_t *TempHum);// DHT11��ȡ����,ֻ��ȡ������λ
void DHT11_Test_Demo(void);
#endif















