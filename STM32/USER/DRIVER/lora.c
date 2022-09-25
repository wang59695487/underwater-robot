/**
  ******************************************************************************
  * @file    lora.c
  * @author  LiuYang
  * @brief   lora通信协议
  ******************************************************************************
  * @attention
  *	Lora模块  串口3  默认波特率: 9600bps  3.3-5.5V供电，大于5V可保持最佳性能
	* E22-230T30S  频率230MHz  空速15.6kbps  奇偶校验8N1  1W
  *	模块有四种工作模式，由引脚 M0、M1设置；详细情况如下表所示
	*	模式(0-3)     M0    M1          模式介绍   
	*	0 一般模式    0     0    串口打开，无线打开，透明传输；支持特殊指令空中配置  
	*                       
	*	1 唤醒模式    1     0		 可以定义为WOR发送方和WOR接收方；支持空中唤醒 
	*	                        
	*	2 省电模式    0     1    用户可通过串口对寄存器进行访问，从而控制模块工作状态
	* 												
	* 3 休眠模式    1     1    模块进入休眠                     
	详见工作参数详解
  ******************************************************************************
  */
#include "lora.h"

//定义
UsartRec_t LORA_Rec;
LORA_SET lora_set; 
/**
 * @funNm : set_chain
 * @brief : 设置信道
 * @param : num 0-64
 * @retval: void
 */
void set_chain(u8 num)
{
    LORA_SET lora;
    lora.frame = 0xc0;
    lora.addr = 0x05;
    lora.lenth = 0x01;
    lora.data = num;
    memcpy(&lora_set, &lora, sizeof(lora));
    HAL_UART_Transmit(&lora_USART, (u8 *)&lora_set, sizeof(lora_set), 100);
}

/**
 * @funNm : set_addrH
 * @brief : 设置地址高位
 * @param : num 
 * @retval: void
 */
void set_addrH(u8 num)
{
    LORA_SET lora;
    lora.frame = 0xc0;
    lora.addr = 0x00;
    lora.lenth = 0x01;
    lora.data = num;
    memcpy(&lora_set, &lora, sizeof(lora));
    HAL_UART_Transmit(&lora_USART, (u8 *)&lora_set, sizeof(lora_set), 100);
}

/**
 * @funNm : set_addrL
 * @brief : 设置地址低位
 * @param : num
 * @retval: void
 */
void set_addrL(u8 num)
{
    LORA_SET lora;
    lora.frame = 0xc0;
    lora.addr = 0x01;
    lora.lenth = 0x01;
    lora.data = num;
    memcpy(&lora_set, &lora, sizeof(lora));
    HAL_UART_Transmit(&lora_USART, (u8 *)&lora_set, sizeof(lora_set), 100);
}

/**
 * @funNm : set_netid
 * @brief : 设置网络地址
 * @param : 0-64
 * @retval: void
 */
void set_netid(u8 num)
{
    LORA_SET lora;
    lora.frame = 0xc0;
    lora.addr = 0x05;
    lora.lenth = 0x01;
    lora.data = num;
    memcpy(&lora_set, &lora, sizeof(lora));
    HAL_UART_Transmit(&lora_USART, (u8 *)&lora_set, sizeof(lora_set), 100);
}

/**
 * @funNm : lora_init
 * @brief : lora初始化
 * @param : ID: 模块id(0-65535), chain: 信道(0-64)
 * @retval: void
 */
u8 cmd[12] = {0xc0, 0x00, 0x09, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x03, 0x00}; //lora配置指令
void lora_init(u16 id, u8 chain)
{
    u16 flag = 0, data = 0xfe ;
		M0SET(1);
    M1SET(0);
    __HAL_UART_ENABLE_IT(&lora_USART, UART_FLAG_IDLE);
    HAL_UART_Receive_DMA(&lora_USART, LORA_Rec.RX_pData, sizeof(LORA_Rec.RX_pData));
    __HAL_UART_CLEAR_IDLEFLAG(&lora_USART);

    if(flag == data)
    {
        return;
    } else {
        M0SET(0);
        M1SET(1);
        HAL_Delay(100);
        cmd[3] = (id & 0xff00) >> 8;
        cmd[4] = id & 0x00ff;
        cmd[8] = chain;
        HAL_UART_Transmit(&lora_USART, cmd, sizeof(cmd),100);
        HAL_Delay(100);
        M0SET(0);
        M1SET(0);
        HAL_Delay(100);
    }
}

///**
// * @funNm : Lora_SendData
// * @brief : 发送函数
// * @param : pbData：数据    pnDataLen：长度
// * @retval: void
// */
//void Lora_SendData(u8* pbData,u16 pnDataLen)
//{
//	HAL_UART_Transmit(&lora_USART, pbData, pnDataLen, 0x100);
//}

/**
 * @funNm : HAL_LORA_To_UART_Receive_IDLE
 * @brief : 串口服务函数
 * @param : huart 某串口
 * @retval: void
 */
void HAL_LORA_To_UART_Receive_IDLE(UART_HandleTypeDef *huart)
{
		static uint32_t temp;
    if((__HAL_UART_GET_FLAG(&lora_USART, UART_FLAG_IDLE) != RESET))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&lora_USART);
        HAL_UART_DMAStop(&lora_USART);
				temp = huart->hdmarx->Instance->CNDTR; 
        LORA_Rec.RX_Size = RX_LEN - temp;
				LORA_Rec.RX_flag = 1; 
        memset(LORA_Rec.RX_pData,0,sizeof(LORA_Rec.RX_pData));
        HAL_UART_Receive_DMA(&lora_USART, LORA_Rec.RX_pData, sizeof(LORA_Rec.RX_pData));
    }
}

/**
 * @funNm : Lora_Test_Demo
 * @brief : Lora测试Demo
 * @param : void
 * @retval: void
 */
void Lora_Test_Demo(void)
{
	lora_init(0,0); //lora初始化，id：0； chain: 0.
	while(1)
	{
		Lora_SendData((uint8_t*)"666", 1); 
	}
}
