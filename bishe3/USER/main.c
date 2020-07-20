#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usmart.h" 
#include "usart2.h"	
#include "esp8266.h"
#include "oled.h"
#include "bmp.h"
#include "rtc.h"
#include "light.h"
#include "water.h"
#include "stmflash.h"

const u8 MODE1_Buffer[]={"SET_WORK-08-00-08-00"};
#define MODE1_SIZE sizeof(MODE1_Buffer)		//数组长度
	const u8 MODE2_Buffer[]={"SET_WORKSLOT-10-05-05-05-1"};
#define MODE2_SIZE sizeof(MODE2_Buffer)		//数组长度
 int main(void)
 {  
	  delay_init();	    	 //延时函数初始化
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级	  
	  uart_init(115200);	 	//串口初始化为9600	
	  USART2_Init(115200);  //初始化串口2波特率为115200
	  
	  usmart_dev.init(SystemCoreClock/1000000);	//鲁玫脢录禄炉USMART	
		RTC_Init(2020,5,13,20,47,5);
		//STMFLASH_Write(FLASH_SAVE_ADDR_SET_WORK,(u16*)MODE1_Buffer,MODE1_SIZE);//????
		//STMFLASH_Write(FLASH_SAVE_ADDR_SET_WORKSLOT,(u16*)MODE2_Buffer,MODE2_SIZE);//????
		LIGHT_Init();
		WATER_Init();
		OLED_Init();			//鍒濆鍖朞LED 
	  atk_8266_init();		  
	  while(1)
	  {			
		OLED_Clear();
		atk_8266_at_response(1);			
		show();  			
		};  	
}
 
























