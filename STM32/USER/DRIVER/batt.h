#ifndef __BATT_H
#define __BATT_H

#include "includes.h"
 //�����˲���С
#define BAT_V_FILTER_N		( 100 )
typedef struct
{
	float  voltage;		//�¶�
}BattMsg_t;
//��������
void Batt_Init(void);	//��ѹ�ɼ���ʼ��
void Get_Batt_Data(BattMsg_t *Batt);	//���ݵ�ѹֵ���͵�ѹ����
void Batt_Test_Demo(void);//��ѹ�ɼ�����Demo

#endif
