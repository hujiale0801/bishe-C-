#include "esp8266.h"
#include "light.h"
#include "oled.h"
#include "rtc.h"
#include "stmflash.h"
#include "water.h"
#include <stdio.h>
#include <string.h>
#include  <stdlib.h> 



_calendar_obj now_time;
//初始化esp8266,进入AP多连接Server模式，ESP8266作为服务器
void atk_8266_init(){
	atk_8266_send_cmd("AT","OK",20);
	atk_8266_send_cmd("AT+CWMODE=3","OK",20);
	atk_8266_send_cmd("AT+CIPMUX=1","OK",20);
	while(atk_8266_send_cmd("AT+CIPSERVER=1,8999","OK",200));
	//while(atk_8266_send_cmd("AT+CWJAP=\"lege\",\"hujiale19980801\"","GOT",600));
	while(atk_8266_send_cmd("AT+CIFSR","APIP",20));
}


//发送命令
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	u2_printf("%s\r\n",cmd);	
	if(ack&&waittime)		
	{
		while(--waittime)	
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)
			{
				if(atk_8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;
				}
					USART2_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

//检查相应信息是否和预期信息一致
u8* atk_8266_check_cmd(u8 *str)
{
	u8 i=0;
	char *s;
	char *strx=0;
	OLED_Init();
	OLED_Clear();
	if(USART2_RX_STA&0X8000)
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
		//OLED_ShowString(0,0,USART2_RX_BUF,8);
		if(strstr((const char*)USART2_RX_BUF,"APIP")){
			s=strtok((char*)USART2_RX_BUF,"\"");
			while(s!=NULL)
			{
				if(i==1||i==5){
					OLED_ShowString(0,0,(u8 *)s,8);
					delay_ms(5000);
					if(i==5){
						break;
					}
				}
				i++;
				s = strtok(NULL, "\"");
			}
		}else{
			OLED_ShowString(0,0,USART2_RX_BUF,8);
		}
	}
	return (u8*)strx;
}

//发送数据
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	u2_printf("%s",data);	
	if(ack&&waittime)
	{
		while(--waittime)	
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)
			{
				if(atk_8266_check_cmd(ack))break;
				USART2_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

//退出透传模式
u8 atk_8266_quit_trans(void)
{
	while((USART2->SR&0X40)==0);	
	USART2->DR='+';      
	delay_ms(15);					
	while((USART2->SR&0X40)==0);	
	USART2->DR='+';      
	delay_ms(15);				
	while((USART2->SR&0X40)==0);
	USART2->DR='+';      
	delay_ms(500);
	return atk_8266_send_cmd("AT","OK",20);
}

//获取接收的信息并上传给电脑
void atk_8266_at_response(u8 mode)
{
	u8 i=0;
	char* str;
	char* link;
	char *name;
	char *password;
	if(USART2_RX_STA&0X8000)	
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;
		printf("%s",USART2_RX_BUF);
		if(strstr((const char*)USART2_RX_BUF,(const char*)"OPEN")){ //判断是否为开灯信号：OPEN
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		}else if(strstr((const char*)USART2_RX_BUF,(const char*)"CLOSE")){ //判断是否为关灯信号：CLOSE
			GPIO_SetBits(GPIOB,GPIO_Pin_5);
		}else if(strstr((const char*)USART2_RX_BUF,(const char*)"NOW")){  //判断是否为立即浇水信号：NOW
			 GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		}else if(strstr((const char*)USART2_RX_BUF,(const char*)"STOP")){  //判断是否为停止浇水信号：STOP
			 GPIO_SetBits(GPIOB,GPIO_Pin_6);
		}else if(strstr((const char*)USART2_RX_BUF,(const char*)"LINKWIFI")){  //判断是否为连接WIFI：LINKWIFI-wifi名-密码
			i=0;
			str=strtok((char*)USART2_RX_BUF,"-");
			while(str!=NULL)
			{
				switch(i){
					case 1:name=str;
						break;
					case 2:password=str;
						break;			
				}
				
        i++;
        str = strtok(NULL, "-");
			}
			sprintf(link,"AT+CWJAP=\"%s\",\"%s\"",name,password); 
			atk_8266_send_cmd("AT","OK",20);
			atk_8266_send_cmd("AT+CWMODE=3","OK",20);
			atk_8266_send_cmd("AT+CIPMUX=1","OK",20);
			while(atk_8266_send_cmd("AT+CIPSERVER=1,8999","OK",200));
			while(atk_8266_send_cmd((u8*)link,"GOT",500));
			while(atk_8266_send_cmd("AT+CIFSR","APIP",20));
			OLED_Clear();
		}else if(strstr((const char*)USART2_RX_BUF,(const char*)"SET_TIME")){ //设置当前时间SET_TIME-年-月-日-时-分-秒
			i=0;
			str=strtok((char*)USART2_RX_BUF,"-");
			while(str!=NULL)
			{
				switch(i){
					case 1:now_time.w_year=atoi(str);
						break;
					case 2:now_time.w_month=atoi(str);
						break;	
					case 3:now_time.w_date=atoi(str);
						break;
					case 4:now_time.hour=atoi(str);
						break;
					case 5:now_time.min=atoi(str);
						break;
					case 6:now_time.sec=atoi(str);
						break;					
				}
        i++;
        str = strtok(NULL, "-");
			}
			RTC_Init(now_time.w_year,now_time.w_month,now_time.w_date,now_time.hour,now_time.min,now_time.sec);
		}else if(strstr((const char*)USART2_RX_BUF,(const char*)"SET_WORK")){ //设置浇水时间段：SET_WORK-时-分-时-分
			STMFLASH_Write(FLASH_SAVE_ADDR_SET_WORK,(u16*)USART2_RX_BUF,sizeof(USART2_RX_BUF));//数据保存
		}else if(strstr((const char*)USART2_RX_BUF,(const char*)"SET_LOT")){//设置浇水的频率：SET_WORKSLOT-设置时间-设置分钟-每多少分钟浇一次水-持续多长时间（分钟）-浇水次数
			STMFLASH_Write(FLASH_SAVE_ADDR_SET_WORKSLOT,(u16*)USART2_RX_BUF,sizeof(USART2_RX_BUF));//数据保存
			count=0;
		}else{
			printf("%s",USART2_RX_BUF);
		}
		if(mode)USART2_RX_STA=0;
	} 
}
