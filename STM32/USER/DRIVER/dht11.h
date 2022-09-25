#ifndef __DHT11_H
#define __DHT11_H 
#include "includes.h" 
#define HIGH  1
#define LOW   0

//带参宏，可以像内联函数一样使用,输出高电平或低电平
#define DHT11_DATA_OUT(a)	if (a)	\
														HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);\
													else		\
														HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET)
//读取引脚的电平
#define  DHT11_DATA_IN()	  HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)
//温湿度数据(含小数)
typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
}DHT11_Data_TypeDef;
//温湿度结构体
typedef struct
{
	float  temp;		//温度
	float  humi;	 	//湿度
}TempHumiMsg_t;
//函数声明
void DHT11_init(void);	//DHT11初始化
static void DHT11_Mode_IPU(void);	//使DHT11-DATA引脚变为上拉输入模式
static void DHT11_Mode_Out_PP(void);//使DHT11-DATA引脚变为推挽输出模式
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);//温湿度读取函数
static uint8_t Read_Byte(void);//从DHT11读取一个字节，MSB先行
bool DHT11_Read_Data(TempHumiMsg_t *TempHum);// DHT11读取数据,只读取了整数位
void DHT11_Test_Demo(void);
#endif















