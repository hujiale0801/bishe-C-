#include "water.h"
#include "rtc.h"
#include "stmflash.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>



u8 WATER_DATA_MODE1[20];//æµ‡æ°´æ—¶é—´æ®µ
u8 WATER_DATA_MODE2[22];//æµ‡æ°´æ—¶é—´
u8 count=0;
void WATER_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //Ê¹ÄÜPB¶Ë¿ÚÊ±ÖÓ
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PB.6 ¶Ë¿ÚÅäÖÃ
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //¸ù¾İÉè¶¨²ÎÊı³õÊ¼»¯GPIOB.6
 GPIO_SetBits(GPIOB,GPIO_Pin_6);						 //PB.6 Êä³ö¸ß
}

