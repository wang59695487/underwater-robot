/**
  ******************************************************************************
  * @file    MS5803.c
  * @author  LiuYang
  * @brief   MS5803_05BA压传
  ******************************************************************************
  * @attention
	* SPI1  --> MS5803
	*
  ******************************************************************************
  */
#include "MS5803.h"

/**
  * @funNm		SPIReadWriteByte
  * @brief		spi读写数据
  * @param		Txdata		写数据
  * @retval		Rxdata  	读出来的数据
*/
static uint8_t SPIxReadWriteByte(uint8_t byte)
{
    uint8_t txdata = byte, rxdata;
    if(HAL_SPI_TransmitReceive(&MS5803_SPI, &txdata, &rxdata, 1, 10) != HAL_OK)
    {
        rxdata = 0xFF; 
    }
    return rxdata;
}

/**
  * @funNm		MS5803aInit
  * @brief		压传初始化
  * @param		void
  * @retval		void
*/
u16 g_Deep = 0; //H 单位cm  T 单位 摄氏度
u32 C[8] = {0};
int g_Temper = 0;
long double P_base = 0, pa = 0;
unsigned long D1 = 0,D2 = 0;
void MS5803aInit(void)
{
//		u8 n_crc;
    MS5803_CS_High;
    MS5803_CS_Low;
    SPIxReadWriteByte(0x1E);
    delay_sys_us(2865); 
    MS5803_CS_High;

    MS5803_CS_Low;
    SPIxReadWriteByte(0xA0);
    C[0] = SPIxReadWriteByte(0xFF)*256;
    C[0] = C[0]+SPIxReadWriteByte(0xFF);
    MS5803_CS_High;

    MS5803_CS_Low;
    SPIxReadWriteByte(0xA2);
    C[1] = SPIxReadWriteByte(0xFF)*256;
    C[1] = C[1]+SPIxReadWriteByte(0xFF);
    MS5803_CS_High;

    MS5803_CS_Low;
    SPIxReadWriteByte(0xA4);
    C[2] = SPIxReadWriteByte(0xFF)*256;
    C[2] = C[2]+SPIxReadWriteByte(0xFF);
    MS5803_CS_High;

    MS5803_CS_Low;
    SPIxReadWriteByte(0xA6);
    C[3] = SPIxReadWriteByte(0xFF)*256;
    C[3] = C[3]+SPIxReadWriteByte(0xFF);
    MS5803_CS_High;

    MS5803_CS_Low;
    SPIxReadWriteByte(0xA8);
    C[4] = SPIxReadWriteByte(0xFF)*256;
    C[4] = C[4]+SPIxReadWriteByte(0xFF);
    MS5803_CS_High;

    MS5803_CS_Low;
    SPIxReadWriteByte(0xAA);
    C[5] = SPIxReadWriteByte(0xFF)*256;
    C[5] = C[5]+SPIxReadWriteByte(0xFF);
    MS5803_CS_High;

    MS5803_CS_Low;
    SPIxReadWriteByte(0xAC);
    C[6] = SPIxReadWriteByte(0xFF)*256;
    C[6] = C[6]+SPIxReadWriteByte(0xFF);
    MS5803_CS_High;

    MS5803_CS_Low;
    SPIxReadWriteByte(0xAE);
    C[7] = SPIxReadWriteByte(0xFF)*256;
    C[7] = C[7]+SPIxReadWriteByte(0xFF);
    MS5803_CS_High;

//    n_crc = crc4(C);
    Read_P();
    Read_T();
    Change_MS5803();
    P_base = pa;
}

/**
  * @funNm		crc4
  * @brief		CRC校验函数 
  * @param		n_prom
  * @retval		void
*/
unsigned char crc4(unsigned int n_prom[])
{
    int cnt;                    // simple counter
    unsigned int n_rem;         // crc reminder
    unsigned int crc_read;      // original value of the crc 
    unsigned char n_bit;
    n_rem = 0x00;
    crc_read = n_prom[7];         //save read CRC
    n_prom[7] = (0xFF00 & (n_prom[7]));     //CRC byte is replaced by 0
    for (cnt = 0; cnt < 16; cnt++)        // operation is performed on bytes
    {   // choose LSB or MSB
        if (cnt%2==1)
        {
            n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
        }
        else
        {
            n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
        }
        for (n_bit = 8; n_bit > 0; n_bit--)
        {
            if (n_rem & (0x8000))
            {
                n_rem = (n_rem << 1) ^ 0x3000;
            }
            else
            {
                n_rem = (n_rem << 1);
            }
        }
    }
    n_rem = (0x000F & (n_rem >> 12)); // // final 4-bit reminder is CRC code
    n_prom[7] = crc_read;     // restore the crc_read to its original place
    return (n_rem ^ 0x00);
}

/**
  * @funNm		Read_P
  * @brief		读取压强
  * @param		void
  * @retval		void
*/
void Read_P(void)
{
    MS5803_CS_High;
    MS5803_CS_Low;
    SPIxReadWriteByte(0x46);
    MS5803_CS_High;
    delay_ms(10); 

    MS5803_CS_Low;
    SPIxReadWriteByte(0x00);
    D1=SPIxReadWriteByte(0x00)*65536;
    D1=D1+SPIxReadWriteByte(0x00)*256;
    D1=D1+SPIxReadWriteByte(0x00);
    MS5803_CS_High;
}

/**
  * @funNm		Read_T
  * @brief		读取温度
  * @param		void
  * @retval		void
*/
void Read_T()
{
    MS5803_CS_High;
    MS5803_CS_Low;
    SPIxReadWriteByte(0x56);
    MS5803_CS_High;
    delay_ms(10); 
    MS5803_CS_Low;
    SPIxReadWriteByte(0x00);
    D2=SPIxReadWriteByte(0x00)*65536;
    D2=D2+SPIxReadWriteByte(0x00)*256;
    D2=D2+SPIxReadWriteByte(0x00);
    MS5803_CS_High;
}

/**
  * @funNm	Change_MS5803
  * @brief	压传数据补偿公式 
  * @param	void
  * @retval	void
*/
void Change_MS5803(void)
{
		long double TEMP,P;
    long double OFF,SENS,T2,SENS2,OFF2,dT;
    SPI_treatment(D1,D2);
    dT = D2-C[5] * pow(2,8);
		TEMP = (2000 + (dT * C[6]) / pow(2,23));
    OFF = C[2] * pow(2,18) + dT * C[4] / pow(2,5);
    SENS = C[1] * pow(2,17) + dT*C[3] / pow(2,7);
    
    if(TEMP < 2000)
    {
        T2 = 3*dT*dT / pow(2,33);
        OFF2 = 3 * (TEMP-2000) * (TEMP - 2000) / 8;
        SENS2 = 7 * (TEMP-2000) * (TEMP-2000) / 8;
        if(TEMP < -1500)
        {
            SENS2 = SENS2 + 3 * (TEMP + 1500) * (TEMP + 1500);
        }
    }
    else if(TEMP >= 2000)
    {
        T2 = 0;
        OFF2 = 0;
        SENS2 = 0;
    }
    TEMP = (TEMP - T2) / 100;
    OFF = OFF- OFF2;
    SENS = SENS - SENS2;
    pa=(((D1 * SENS) / pow(2,21) - OFF) / pow(2,15)) / 100;
    P = (P_base - pa);  //P单位mbar=100帕斯卡
    (P>=0) ? (P=P) : (P=-P);
    g_Deep = P * 100 / 98;	//单位 cm
    g_Temper = TEMP;
}

/**
  * @funNm	SPI_treatment
  * @brief	压传数据出错舍弃函数 
  * @param	d11  d22
  * @retval	void
*/
unsigned long d1_l = 0,d2_l = 0;
void SPI_treatment(unsigned long d11,unsigned long d22)
{
    if(( D1 == 0XFFFFFF )||( D2 == 0XFFFFFF )) {
        D1=d1_l;
        D2=d2_l;
    }
    else if(( D1 == 0 )||( D2 == 0 )) {
        D1=d1_l;
        D2=d2_l;
    }
    d1_l=d11;
    d2_l=d22;
}

/**
  * @funNm	MS5803_Read_Sometimes
  * @brief	间歇性采集压传数值 
  * @param	void
  * @retval	void
*/
int YA_C_Flag = 1;
void MS5803_Read_Sometimes(void)
{
    if(YA_C_Flag == 1)
    {
        MS5803_CS_Low;
        SPIxReadWriteByte(0x00);
        D1=SPIxReadWriteByte(0x00)*65536;
        D1=D1+SPIxReadWriteByte(0x00)*256;
        D1=D1+SPIxReadWriteByte(0x00);
        MS5803_CS_High;

        MS5803_CS_Low;
        SPIxReadWriteByte(0x56);
        MS5803_CS_High;

        YA_C_Flag = 0;
    }
    else if(YA_C_Flag == 0) {
        MS5803_CS_Low;
        SPIxReadWriteByte(0x00);
        D2=SPIxReadWriteByte(0x00)*65536;
        D2=D2+SPIxReadWriteByte(0x00)*256;
        D2=D2+SPIxReadWriteByte(0x00);
        MS5803_CS_High;

        MS5803_CS_Low;
        SPIxReadWriteByte(0x46);
        MS5803_CS_High;

        YA_C_Flag = 1;
    }
    SPI_treatment(D1,D2);
    Change_MS5803();
}

/**
  * @funNm	Get_Pressure
  * @brief	获取数据
  * @param	ms5803结构体
  * @retval	void
*/
void Get_Pressure(PressureMsg_t *ms5803)
{
	MS5803_Read_Sometimes();  		            //获取压传的深度值
	ms5803->depth = g_Deep;
	ms5803->Temperature  = g_Temper;  
}

/**
  * @funNm		MS5803_Test_Demo
  * @brief		MS5803测试demo
  * @param		void
  * @retval		void
*/
PressureMsg_t ms5803;
void MS5803_Test_Demo(void)
{
	//extern PressureMsg_t ms5803;
		//PressureMsg_t ms5803;
    MS5803aInit();
    while(1)
    {
        Get_Pressure(&ms5803);
        delay_ms(20);
    }
}
