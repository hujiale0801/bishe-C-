#include "water.h"
#include "rtc.h"
#include "stmflash.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>



u8 WATER_DATA_MODE1[20];//浇水时间段
u8 WATER_DATA_MODE2[22];//浇水时间
u8 count=0;
void WATER_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PB.6 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.6
 GPIO_SetBits(GPIOB,GPIO_Pin_6);						 //PB.6 �����
}

