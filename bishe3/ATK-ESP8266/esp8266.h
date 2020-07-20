#ifndef __ESP8266_H
#define __ESP8266_H	
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usmart.h" 
#include "usart2.h"	
#include "string.h"  

void atk_8266_init(void);
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8* atk_8266_check_cmd(u8 *str);
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8 atk_8266_quit_trans(void);
void atk_8266_at_response(u8 mode);
#endif

