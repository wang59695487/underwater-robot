/**
  ******************************************************************************
  * @file    batt.c
  * @author  LiuYang
  * @brief   ADC��ѹ�ɼ�
  ******************************************************************************
  * @attention
  *	��ص�ѹ:  22K:4.99K �����ѹ     (4.4:1)
	* ADC1 IN1 --> ��ѹ���
	* 
  ******************************************************************************
  */

#include "batt.h"

uint32_t ADC_Value[100];
uint32_t BatVoltageBuf[BAT_V_FILTER_N + 1] = {0};
uint32_t Bat_Voltage;
uint32_t adcVoltage;
/**
 * @funNm : Batt_Init
 * @brief : ��ѹ�ɼ���ʼ��
 * @param : void
 * @retval: void
 */
void Batt_Init(void)
{
		HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_Value,100);
}

/**
 * @funNm : Voltage_RecursionFilter
 * @brief : ��ѹADC�ɼ�ԭʼ���ݾ�ֵ�˲�
 * @param : filterbuf ��ѹ������; 
 * @retval: ��ֵ�˲���ĵ�ѹԭʼֵ
 *///static 
static uint32_t Voltage_RecursionFilter(uint32_t *filterbuf)
{
		uint32_t filtersum = 0;
		int i= 0; 
		for( i = 0; i < BAT_V_FILTER_N; i++) 
		{
			filtersum += filterbuf[i];
		}
		return (int)(filtersum / BAT_V_FILTER_N);
}

/**
 * @funNm : GetVoltage
 * @brief : ��ȡ��ص�ѹ
 * @param : void
 * @retval: ��ѹֵ 
 */
float GetVoltage(void)
{
    uint32_t adcVoltage = Voltage_RecursionFilter(ADC_Value);   
    return (adcVoltage*3.3f/4096*(140/10+1)*1.0128);            
}

/**
 * @funNm : Get_Batt_Data
 * @brief : ���ݵ�ѹֵ���͵�ѹ����
 * @param : ��ѹֵ 
 * @retval: void
 */
void Get_Batt_Data(BattMsg_t *Batt)
{
	Batt->voltage = GetVoltage();
	if((Batt->voltage) <= 8)
	{
		HAL_GPIO_TogglePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin);
	}
} 

/**
 * @funNm : Batt_Test_Demo
 * @brief : ��ѹ�ɼ�����Demo
 * @param : void
 * @retval: void
 */
BattMsg_t Batt;
void Batt_Test_Demo(void)
{
	//extern BattMsg_t Batt;
	Batt_Init();
	while(1)
	{
		Get_Batt_Data(&Batt);
//		voltage=GetVoltage();
//		delay_ms(500);		
	}
}

	
//#include "batt.h"

//uint32_t ADC_Value;
//uint32_t BatVoltageBuf[BAT_V_FILTER_N + 1] = {0};
///**
// * @funNm : Batt_Init
// * @brief : ��ѹ�ɼ���ʼ��
// * @param : void
// * @retval: void
// */
//void Batt_Init(void)
//{
//		int i= 0; 
//		HAL_ADC_Start_DMA(&hadc1,&ADC_Value,sizeof(ADC_Value));
//		for(i = 0; i < BAT_V_FILTER_N + 1; i++)
//		{
//			BatVoltageBuf[i] = ADC_Value;
//			delay_ms(1);
//		}
//}

///**
// * @funNm : Voltage_RecursionFilter
// * @brief : ��ѹADC�ɼ�ԭʼ���ݾ�ֵ�˲�
// * @param : filterbuf ��ѹ������;  ADValue �²ɼ�������  
// * @retval: ��ֵ�˲���ĵ�ѹԭʼֵ
// */
//static uint32_t Voltage_RecursionFilter(uint32_t *filterbuf,uint32_t ADValue)
//{
//		uint32_t filtersum = 0;
//		int i= 0; 
//		filterbuf[BAT_V_FILTER_N] = ADValue;
//		for( i = 0; i < BAT_V_FILTER_N; i++) 
//		{
//			filterbuf[i] = filterbuf[i + 1]; // 
//			filtersum += filterbuf[i];
//		}
//		return (int)(filtersum / BAT_V_FILTER_N);
//}

///**
// * @funNm : GetVoltage
// * @brief : ��ȡ��ص�ѹ
// * @param : void
// * @retval: ��ѹֵ 
// */
//float GetVoltage(void)
//{
//    uint32_t adcVoltage = Voltage_RecursionFilter(BatVoltageBuf, ADC_Value);   
//    return (adcVoltage*3.3f/4096*(22/4.99+1)*1.0128);            
//}

///**
// * @funNm : Get_Batt_Data
// * @brief : ���ݵ�ѹֵ���͵�ѹ����
// * @param : ��ѹֵ 
// * @retval: void
// */
//void Get_Batt_Data(float voltage)
//{
//	voltage = GetVoltage();
//	if((voltage) <= 5)
//	{
//		HAL_GPIO_TogglePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin);
//	}
//} 

///**
// * @funNm : Batt_Test_Demo
// * @brief : ��ѹ�ɼ�����Demo
// * @param : void
// * @retval: void
// */
//void Batt_Test_Demo(void)
//{
//	float voltage;
//	Batt_Init();
//	while(1)
//	{
//    Get_Batt_Data(voltage);
//		delay_ms(500);		
//	}
//}

