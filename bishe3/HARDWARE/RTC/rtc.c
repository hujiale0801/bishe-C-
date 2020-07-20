#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h" 			    
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"



_calendar_obj calendar;//Ê±ÖÓ½á¹¹Ì

static void RTC_NVIC_Config(void)
{	
  NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCÈ«¾ÖÖĞ¶Ï
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//ÏÈÕ¼ÓÅÏÈ¼¶1Î»,´ÓÓÅÏÈ¼¶3Î»
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//ÏÈÕ¼ÓÅÏÈ¼¶0Î»,´ÓÓÅÏÈ¼¶4Î»
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//Ê¹ÄÜ¸ÃÍ¨µÀÖĞ¶Ï
	NVIC_Init(&NVIC_InitStructure);		//¸ù¾İNVIC_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷
}

//ÊµÊ±Ê±ÖÓÅäÖÃ
//³õÊ¼»¯RTCÊ±ÖÓ,Í¬Ê±¼ì²âÊ±ÖÓÊÇ·ñ¹¤×÷Õı³£
//BKP->DR1ÓÃÓÚ±£´æÊÇ·ñµÚÒ»´ÎÅäÖÃµÄÉèÖÃ
//·µ»Ø0:Õı³£
//ÆäËû:´íÎó´úÂë

u8 RTC_Init(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	//¼ì²éÊÇ²»ÊÇµÚÒ»´ÎÅäÖÃÊ±ÖÓ
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//Ê¹ÄÜPWRºÍBKPÍâÉèÊ±ÖÓ   
	PWR_BackupAccessCmd(ENABLE);	//Ê¹ÄÜºó±¸¼Ä´æÆ÷·ÃÎÊ  
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5051)		//´ÓÖ¸¶¨µÄºó±¸¼Ä´æÆ÷ÖĞ¶Á³öÊı¾İ:¶Á³öÁËÓëĞ´ÈëµÄÖ¸¶¨Êı¾İ²»Ïàºõ
		{	 			
			
		BKP_DeInit();	//¸´Î»±¸·İÇøÓò 	
		RCC_LSEConfig(RCC_LSE_ON);	//ÉèÖÃÍâ²¿µÍËÙ¾§Õñ(LSE),Ê¹ÓÃÍâÉèµÍËÙ¾§Õñ
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//¼ì²éÖ¸¶¨µÄRCC±êÖ¾Î»ÉèÖÃÓë·ñ,µÈ´ıµÍËÙ¾§Õñ¾ÍĞ÷
			{
			temp++;
			delay_ms(10);
			}
		if(temp>=250)return 1;//³õÊ¼»¯Ê±ÖÓÊ§°Ü,¾§ÕñÓĞÎÊÌâ	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//ÉèÖÃRTCÊ±ÖÓ(RTCCLK),Ñ¡ÔñLSE×÷ÎªRTCÊ±ÖÓ    
		RCC_RTCCLKCmd(ENABLE);	//Ê¹ÄÜRTCÊ±ÖÓ  
		RTC_WaitForLastTask();	//µÈ´ı×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄĞ´²Ù×÷Íê³É
		RTC_WaitForSynchro();		//µÈ´ıRTC¼Ä´æÆ÷Í¬²½  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//Ê¹ÄÜRTCÃëÖĞ¶Ï
		RTC_WaitForLastTask();	//µÈ´ı×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄĞ´²Ù×÷Íê³É
		RTC_EnterConfigMode();/// ÔÊĞíÅäÖÃ	
		RTC_SetPrescaler(32767); //ÉèÖÃRTCÔ¤·ÖÆµµÄÖµ
		RTC_WaitForLastTask();	//µÈ´ı×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄĞ´²Ù×÷Íê³É
		RTC_Set(syear,smon,sday,hour,min,sec);  //ÉèÖÃÊ±¼ä	
		RTC_ExitConfigMode(); //ÍË³öÅäÖÃÄ£Ê½  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//ÏòÖ¸¶¨µÄºó±¸¼Ä´æÆ÷ÖĞĞ´ÈëÓÃ»§³ÌĞòÊı¾İ
		}
	else//ÏµÍ³¼ÌĞø¼ÆÊ±
		{

		RTC_WaitForSynchro();	//µÈ´ı×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄĞ´²Ù×÷Íê³É
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//Ê¹ÄÜRTCÃëÖĞ¶Ï
		RTC_WaitForLastTask();	//µÈ´ı×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄĞ´²Ù×÷Íê³É
		}
	RTC_NVIC_Config();//RCTÖĞ¶Ï·Ö×éÉèÖÃ		    				     
	RTC_Get();//¸üĞÂÊ±¼ä	
	return 0; //ok

}		 				    
//RTCÊ±ÖÓÖĞ¶Ï
//Ã¿Ãë´¥·¢Ò»´Î  
//extern u16 tcnt; 
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//ÃëÖÓÖĞ¶Ï
	{							
		RTC_Get();//¸üĞÂÊ±¼ä   
 	}
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//ÄÖÖÓÖĞ¶Ï
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);		//ÇåÄÖÖÓÖĞ¶Ï	  	
	  RTC_Get();				//¸üĞÂÊ±¼ä   
  	printf("Alarm Time:%d-%d-%d %d:%d:%d\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//Êä³öÄÖÁåÊ±¼ä	
		
  	} 				  								 
	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//ÇåÄÖÖÓÖĞ¶Ï
	RTC_WaitForLastTask();	  	    						 	   	 
}
//ÅĞ¶ÏÊÇ·ñÊÇÈòÄêº¯Êı
//ÔÂ·İ   1  2  3  4  5  6  7  8  9  10 11 12
//ÈòÄê   31 29 31 30 31 30 31 31 30 31 30 31
//·ÇÈòÄê 31 28 31 30 31 30 31 31 30 31 30 31
//ÊäÈë:Äê·İ
//Êä³ö:¸ÃÄê·İÊÇ²»ÊÇÈòÄê.1,ÊÇ.0,²»ÊÇ
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //±ØĞëÄÜ±»4Õû³ı
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//Èç¹ûÒÔ00½áÎ²,»¹ÒªÄÜ±»400Õû³ı 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   
//ÉèÖÃÊ±ÖÓ
//°ÑÊäÈëµÄÊ±ÖÓ×ª»»ÎªÃëÖÓ
//ÒÔ1970Äê1ÔÂ1ÈÕÎª»ù×¼
//1970~2099ÄêÎªºÏ·¨Äê·İ
//·µ»ØÖµ:0,³É¹¦;ÆäËû:´íÎó´úÂë.
//ÔÂ·İÊı¾İ±í											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //ÔÂĞŞÕıÊı¾İ±í	  
//Æ½ÄêµÄÔÂ·İÈÕÆÚ±í
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//°ÑËùÓĞÄê·İµÄÃëÖÓÏà¼Ó
	{
		if(Is_Leap_Year(t))seccount+=31622400;//ÈòÄêµÄÃëÖÓÊı
		else seccount+=31536000;			  //Æ½ÄêµÄÃëÖÓÊı
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //°ÑÇ°ÃæÔÂ·İµÄÃëÖÓÊıÏà¼Ó
	{
		seccount+=(u32)mon_table[t]*86400;//ÔÂ·İÃëÖÓÊıÏà¼Ó
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//ÈòÄê2ÔÂ·İÔö¼ÓÒ»ÌìµÄÃëÖÓÊı	   
	}
	seccount+=(u32)(sday-1)*86400;//°ÑÇ°ÃæÈÕÆÚµÄÃëÖÓÊıÏà¼Ó 
	seccount+=(u32)hour*3600;//Ğ¡Ê±ÃëÖÓÊı
    seccount+=(u32)min*60;	 //·ÖÖÓÃëÖÓÊı
	seccount+=sec;//×îºóµÄÃëÖÓ¼ÓÉÏÈ¥

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//Ê¹ÄÜPWRºÍBKPÍâÉèÊ±ÖÓ  
	PWR_BackupAccessCmd(ENABLE);	//Ê¹ÄÜRTCºÍºó±¸¼Ä´æÆ÷·ÃÎÊ 
	RTC_SetCounter(seccount);	//ÉèÖÃRTC¼ÆÊıÆ÷µÄÖµ

	RTC_WaitForLastTask();	//µÈ´ı×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄĞ´²Ù×÷Íê³É  	
	return 0;	    
}

//³õÊ¼»¯ÄÖÖÓ		  
//ÒÔ1970Äê1ÔÂ1ÈÕÎª»ù×¼
//1970~2099ÄêÎªºÏ·¨Äê·İ
//syear,smon,sday,hour,min,sec£ºÄÖÖÓµÄÄêÔÂÈÕÊ±·ÖÃë   
//·µ»ØÖµ:0,³É¹¦;ÆäËû:´íÎó´úÂë.
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//°ÑËùÓĞÄê·İµÄÃëÖÓÏà¼Ó
	{
		if(Is_Leap_Year(t))seccount+=31622400;//ÈòÄêµÄÃëÖÓÊı
		else seccount+=31536000;			  //Æ½ÄêµÄÃëÖÓÊı
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //°ÑÇ°ÃæÔÂ·İµÄÃëÖÓÊıÏà¼Ó
	{
		seccount+=(u32)mon_table[t]*86400;//ÔÂ·İÃëÖÓÊıÏà¼Ó
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//ÈòÄê2ÔÂ·İÔö¼ÓÒ»ÌìµÄÃëÖÓÊı	   
	}
	seccount+=(u32)(sday-1)*86400;//°ÑÇ°ÃæÈÕÆÚµÄÃëÖÓÊıÏà¼Ó 
	seccount+=(u32)hour*3600;//Ğ¡Ê±ÃëÖÓÊı
    seccount+=(u32)min*60;	 //·ÖÖÓÃëÖÓÊı
	seccount+=sec;//×îºóµÄÃëÖÓ¼ÓÉÏÈ¥ 			    
	//ÉèÖÃÊ±ÖÓ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//Ê¹ÄÜPWRºÍBKPÍâÉèÊ±ÖÓ   
	PWR_BackupAccessCmd(ENABLE);	//Ê¹ÄÜºó±¸¼Ä´æÆ÷·ÃÎÊ  
	//ÉÏÃæÈı²½ÊÇ±ØĞëµÄ!
	
	RTC_SetAlarm(seccount);
 
	RTC_WaitForLastTask();	//µÈ´ı×î½üÒ»´Î¶ÔRTC¼Ä´æÆ÷µÄĞ´²Ù×÷Íê³É  	
	
	return 0;	    
}


//µÃµ½µ±Ç°µÄÊ±¼ä
//·µ»ØÖµ:0,³É¹¦;ÆäËû:´íÎó´úÂë.
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
    timecount=RTC_GetCounter();	 
 	temp=timecount/86400;   //µÃµ½ÌìÊı(ÃëÖÓÊı¶ÔÓ¦µÄ)
	if(daycnt!=temp)//³¬¹ıÒ»ÌìÁË
	{	  
		daycnt=temp;
		temp1=1970;	//´Ó1970Äê¿ªÊ¼
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//ÊÇÈòÄê
			{
				if(temp>=366)temp-=366;//ÈòÄêµÄÃëÖÓÊı
				else {temp1++;break;}  
			}
			else temp-=365;	  //Æ½Äê 
			temp1++;  
		}   
		calendar.w_year=temp1;//µÃµ½Äê·İ
		temp1=0;
		while(temp>=28)//³¬¹ıÁËÒ»¸öÔÂ
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)//µ±ÄêÊÇ²»ÊÇÈòÄê/2ÔÂ·İ
			{
				if(temp>=29)temp-=29;//ÈòÄêµÄÃëÖÓÊı
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//Æ½Äê
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;	//µÃµ½ÔÂ·İ
		calendar.w_date=temp+1;  	//µÃµ½ÈÕÆÚ 
	}
	temp=timecount%86400;     		//µÃµ½ÃëÖÓÊı   	   
	calendar.hour=temp/3600;     	//Ğ¡Ê±
	calendar.min=(temp%3600)/60; 	//·ÖÖÓ	
	calendar.sec=(temp%3600)%60; 	//ÃëÖÓ
	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//»ñÈ¡ĞÇÆÚ   
	return 0;
}	 
//»ñµÃÏÖÔÚÊÇĞÇÆÚ¼¸
//¹¦ÄÜÃèÊö:ÊäÈë¹«ÀúÈÕÆÚµÃµ½ĞÇÆÚ(Ö»ÔÊĞí1901-2099Äê)
//ÊäÈë²ÎÊı£º¹«ÀúÄêÔÂÈÕ 
//·µ»ØÖµ£ºĞÇÆÚºÅ																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// Èç¹ûÎª21ÊÀ¼Í,Äê·İÊı¼Ó100  
	if (yearH>19)yearL+=100;
	// Ëù¹ıÈòÄêÊıÖ»Ëã1900ÄêÖ®ºóµÄ  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  















