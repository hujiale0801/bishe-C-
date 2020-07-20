#include "common.h"

//ATK-ESP8266 WIFI AP����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 atk_8266_wifiap_test(void)
{
	u8 netpro=0;	//����ģʽ

	u8 timex=0; 
	u8 ipbuf[16]; 	//IP����
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	

	netpro=atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[1]);	//ѡ������ģʽ
	if(netpro&0X02)   //UDP
	{
			
				if(atk_8266_ip_set("WIFI-AP Զ��UDP IP����",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf));	//IP����
				sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��UDP������
				atk_8266_send_cmd("AT+CIPMUX=0","OK",100);  //������ģʽ

				while(atk_8266_send_cmd(p,"OK",500));
			
	}
	else     //TCP
	{
		if(netpro&0X01)     //TCP Client    ͸��ģʽ����
		{
			
			if(atk_8266_ip_set("WIFI-AP Զ��IP����",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf));	//IP����
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������
			while(atk_8266_send_cmd(p,"OK",200));
			
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��		
			}
			else					//TCP Server
			{
					
					atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
					sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
					atk_8266_send_cmd(p,"OK",20);     //����Serverģʽ���˿ں�Ϊ8086
			}
	}
			
			
			delay_ms(200);
			
			atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP
			sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);
			
			atk_8266_wificonf_show(30,180,"�����豸����WIFI�ȵ�:",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);		
			USART2_RX_STA=0;
			while(1)
			{
				
					res=0;
					atk_8266_quit_trans();	//�˳�͸��
					atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //�ر�͸��ģʽ
					break;												 
				
				
				
					if((netpro==3)||(netpro==2))   //UDP
					{
						sprintf((char*)p,"ATK-8266%s����%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
						
						atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //����ָ�����ȵ�����
						delay_ms(200);
						atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
						timex=100;
					}
					else if((netpro==1))   //TCP Client
					{
						atk_8266_quit_trans();
						atk_8266_send_cmd("AT+CIPSEND","OK",20);       //��ʼ͸��
						sprintf((char*)p,"ATK-8266%s����%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
						
						u2_printf("%s",p);
						timex=100;
					}
					else    //TCP Server
					{
						sprintf((char*)p,"ATK-8266%s����%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//��������
					
						atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //����ָ�����ȵ�����
						delay_ms(200);
						atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
						timex=100;
					}
				
			
	if(timex)timex--;
	if(timex==1);
	t++;
	delay_ms(5);
	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{ 
		rlen=USART2_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
		USART2_RX_BUF[rlen]=0;		//��ӽ����� 
		printf("%s",USART2_RX_BUF);	//���͵�����   
		sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
		USART2_RX_STA=0;
		if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
		else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
	}  
	if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
	{
		constate=atk_8266_consta_check();//�õ�����״̬
		if(constate=='+');  //����״̬
		else; 	  	 
		t=0;
	}
	if((t%20)==0)LED0=!LED0;
	atk_8266_at_response(1);
}
	
	return res;		
} 







