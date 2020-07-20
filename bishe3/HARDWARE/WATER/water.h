#ifndef __WATER_H
#define __WATER_H	 
#include "sys.h"


#define FLASH_SAVE_ADDR_SET_WORK  0X08007900		//ʨ׃FLASH ѣզַ֘(ҘѫΪż˽ìȒǤֵҪճԚѾպë̹ռԃFLASHքճС+0X08000000)
#define FLASH_SAVE_ADDR_SET_WORKSLOT  0X08008000

extern u8 count;//浇水次数
extern u8 WATER_DATA_MODE1[20];//浇水时间
extern u8 WATER_DATA_MODE2[22];//浇水时间段

	


void WATER_Init(void);//��ʼ��
#endif

