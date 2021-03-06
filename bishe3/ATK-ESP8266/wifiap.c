#include "common.h"

//ATK-ESP8266 WIFI AP测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 atk_8266_wifiap_test(void)
{
	u8 netpro=0;	//网络模式

	u8 timex=0; 
	u8 ipbuf[16]; 	//IP缓存
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	

	netpro=atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[1]);	//选择网络模式
	if(netpro&0X02)   //UDP
	{
			
				if(atk_8266_ip_set("WIFI-AP 远端UDP IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf));	//IP输入
				sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
				atk_8266_send_cmd("AT+CIPMUX=0","OK",100);  //单链接模式

				while(atk_8266_send_cmd(p,"OK",500));
			
	}
	else     //TCP
	{
		if(netpro&0X01)     //TCP Client    透传模式测试
		{
			
			if(atk_8266_ip_set("WIFI-AP 远端IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf));	//IP输入
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器
			while(atk_8266_send_cmd(p,"OK",200));
			
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传		
			}
			else					//TCP Server
			{
					
					atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
					sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
					atk_8266_send_cmd(p,"OK",20);     //开启Server模式，端口号为8086
			}
	}
			
			
			delay_ms(200);
			
			atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP
			sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
			
			atk_8266_wificonf_show(30,180,"请用设备连接WIFI热点:",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);		
			USART2_RX_STA=0;
			while(1)
			{
				
					res=0;
					atk_8266_quit_trans();	//退出透传
					atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //关闭透传模式
					break;												 
				
				
				
					if((netpro==3)||(netpro==2))   //UDP
					{
						sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
						
						atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //发送指定长度的数据
						delay_ms(200);
						atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
						timex=100;
					}
					else if((netpro==1))   //TCP Client
					{
						atk_8266_quit_trans();
						atk_8266_send_cmd("AT+CIPSEND","OK",20);       //开始透传
						sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
						
						u2_printf("%s",p);
						timex=100;
					}
					else    //TCP Server
					{
						sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
					
						atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //发送指定长度的数据
						delay_ms(200);
						atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
						timex=100;
					}
				
			
	if(timex)timex--;
	if(timex==1);
	t++;
	delay_ms(5);
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		rlen=USART2_RX_STA&0X7FFF;	//得到本次接收到的数据长度
		USART2_RX_BUF[rlen]=0;		//添加结束符 
		printf("%s",USART2_RX_BUF);	//发送到串口   
		sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
		USART2_RX_STA=0;
		if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
		else t=0;                   //状态为已经连接了,10秒后再检查
	}  
	if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
	{
		constate=atk_8266_consta_check();//得到连接状态
		if(constate=='+');  //连接状态
		else; 	  	 
		t=0;
	}
	if((t%20)==0)LED0=!LED0;
	atk_8266_at_response(1);
}
	
	return res;		
} 







