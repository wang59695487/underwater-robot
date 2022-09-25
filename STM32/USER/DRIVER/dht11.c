/**
  ******************************************************************************
  * @file    DHT11.c
  * @author  LiuYang
  * @brief   温湿度传感器
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
 * @brief : DHT11初始化
 * @param : void
 * @retval: void
 */
void DHT11_init(void)
{		
	  //拉高DHT11
		HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
}

/**
 * @funNm : DHT11_Mode_IPU
 * @brief : 使DHT11-DATA引脚变为上拉输入模式
 * @param : void
 * @retval: void
 */
static void DHT11_Mode_IPU(void)
{
 	 GPIO_InitTypeDef GPIO_InitStruct = {0};
	 
	 //选择要控制的DHT11_PORT引脚
	 GPIO_InitStruct.Pin = DHT11_Pin;
	 
   //设置引脚模式为浮空输入模式
	 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	 
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 
	 //调用库函数，初始化DHT11_PORT 
	 HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);	 
}

/**
 * @funNm : DHT11_Mode_Out_PP
 * @brief : 使DHT11-DATA引脚变为推挽输出模式
 * @param : void
 * @retval: void
 */
static void DHT11_Mode_Out_PP(void)
{
 	 GPIO_InitTypeDef GPIO_InitStruct = {0};
	 
	 //选择要控制的DHT11_PORT引脚
	 GPIO_InitStruct.Pin = DHT11_Pin;
	 
   //设置引脚模式为通用推挽输出
	 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	 
	 //设置引脚速率为50MHz
	 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	 
	 //调用库函数，初始化DHT11_PORT 
	 HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);		 
}

/**
 * @funNm : Read_Byte
 * @brief : 从DHT11读取一个字节，MSB先行
 * @param : void
 * @retval: temp
 */
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		//每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束
		while(DHT11_DATA_IN()==GPIO_PIN_RESET);

		//DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		delay_sys_us(40); //延时x us 这个延时需要大于数据0持续的时间即可	  
 	  
    //x us后仍为高电平表示数据“1” 
		if(DHT11_DATA_IN()==GPIO_PIN_SET) 
		{
			//等待数据1的高电平结束
			while(DHT11_DATA_IN()==GPIO_PIN_SET);

			//把第7-i位置1，MSB先行 
			temp|=(uint8_t)(0x01<<(7-i));  
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			//把第7-i位置0，MSB先行
			temp&=(uint8_t)~(0x01<<(7-i)); 
		}
	}
	return temp;
}

/**
 * @funNm : Read_DHT11
 * @brief : 一次完整的数据传输为40bit，高位先出
						8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
 * @param : DHT11_Data
 * @retval: SUCCESS / ERROR
 */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	DHT11_Mode_Out_PP();	//输出模式
	DHT11_DATA_OUT(LOW);	//主机拉低
	delay_ms(18);//延时18ms
	DHT11_DATA_OUT(HIGH); //总线拉高 主机延时30us
	delay_sys_us(30); //延时30us
	DHT11_Mode_IPU();//主机设为输入 判断从机响应信号
	
	
	if(DHT11_DATA_IN()==GPIO_PIN_RESET)   //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行   
	{
		//轮询直到从机发出 的80us 低电平 响应信号结束
		while(DHT11_DATA_IN()==GPIO_PIN_RESET);
		
		//轮询直到从机发出的 80us 高电平 标置信号结束
		while(DHT11_DATA_IN()==GPIO_PIN_SET);

		//开始接收数据 
		DHT11_Data->humi_int= Read_Byte();
		DHT11_Data->humi_deci= Read_Byte();
		DHT11_Data->temp_int= Read_Byte();
		DHT11_Data->temp_deci= Read_Byte();
		DHT11_Data->check_sum= Read_Byte();
		
		//读取结束，引脚改为输出模式
		DHT11_Mode_Out_PP();
		
		//主机拉高
		DHT11_DATA_OUT(HIGH);
		
		//检查读取的数据是否正确
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
 * @brief : DHT11读取数据,这里只读取了整数位
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
 * @brief : DHT11测试demo
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
			//读取温湿度值	
			DHT11_Read_Data(&TempHum);  
			
			//每100ms读取一次
			delay_ms(100);  
		}
}
