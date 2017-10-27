/***********************************************
@		Description: This file is main input for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/10.
***********************************************/

#include <stdio.h>                      /* standard I/O .h-file              */
#include <ctype.h>                      /* character functions               */
#include <LPC21xx.H>                    /* LPC21xx definitions               */
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"                    /* global project definition file    */
#include "config.h"
#include "Peripherals_LPC2194.h"
#include "AD7738.h"
#include "DAC8568.h"
#include "Command.h"
#include "app.h"
#include "DS1390.h"

/*-------------------------Global variable region----------------------*/
//extern int flag1, flag2;
extern unsigned char flag_command;
extern unsigned char flag_function;
extern unsigned char rcv_buf[100];
extern volatile unsigned char rcv_new;
extern unsigned int rcv_cnt;
extern unsigned char cmd_tmp[CMD_LEN];
extern unsigned char cmd_buf[CMD_LEN];
extern unsigned char flag_screen;
extern int flag1, flag2, flag3;
extern unsigned char a;
extern REALTIMEINFO CurrentTime;
extern unsigned char flag_mode;
/***********************************************************
Function:	init peripherals.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: setup the timer counter 0 interrupt.
***********************************************************/
void init_peripherals(void)
{
	LED_init();
	init_serial();
	init_timer();
	init_PWM();
	SPI0_INIT();
	SPI1_INIT();
	AD7738_CS_INIT();
	DAC8568_CS_INIT();
	Initial_DS1390();
}

/***********************************************************
Function:	Main function.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: main function for Model3000 project.
***********************************************************/
int main (void)  
{
	unsigned char i;
//	unsigned char tmp[100];
//	unsigned char a;
 	FrecInit();
	init_peripherals();
	UARTprintf("model3000 test\n");
	
	while (1)  
	{
		
		if(rcv_new==1)
		{
		rcv_new=0;
		UART0_SendData(rcv_buf,rcv_cnt);
		//UARTprintf("\n");
		a=get_true_char_stream(cmd_tmp,rcv_buf);
		//UART0_SendData(cmd_tmp,a);
		//UARTprintf("\n");
		//UARTprintf("a=%d\n",a);
		memset(rcv_buf,0,rcv_cnt);
		rcv_cnt=0;
		DS1390_GetTime(&CurrentTime);
		UARTprintf("%d,%d,%d,%d,%d,%d\n",CurrentTime.SpecificTime.year+2000,CurrentTime.SpecificTime.month,
		CurrentTime.SpecificTime.day,CurrentTime.SpecificTime.hour,CurrentTime.SpecificTime.min,CurrentTime.SpecificTime.sec);
		}

		if(flag_command==0)
		{
			if(findcmdfunction(cmd_tmp)==1)
			{
				memset(cmd_tmp,0,a);
				a=0;
				flag_screen=1;
				UARTprintf("guanbihuixian\n");
			}		
		}

		switch(flag_command){
			case 1:
			alarm_arg();			
			break;
			case 2:
			config_arg_d1();
			break;
			case 3:
			da_arg();
			break;
			case 4:
			db_arg();
			break;
			case 5:
			dx_arg();
			break;
			case 6:
			gg_arg();
			break;
			case 7:
			aop_arg();
			break;
			case 8:
			aoerr_arg();
			break;
			case 9:
			install_arg();
			break;
			case 10:
			date_arg();
			break;
			case 11:
			record_arg();
			break;
			case 12:
			clear_arg();
			break;
			case 13:
			ci_arg();
			break;
			case 14:
			setmid_arg();
			break;
			case 15:
			cf_arg();
			break;
			case 16:
			config_arg_d0();
			break;
			case 17:
			config_arg_d2();
			break;
			case 18:
			config_arg_d3();
			break;
			default:
			memset(cmd_tmp,0,strlen(cmd_tmp));
			a=0;
			break;			
		}
		switch (flag1)  {
			case 1:
			//capture oil temp;
			ADC7738_acquisition(1);
			ADC7738_acquisition_output(1);
			flag1 = 0;
			if(flag_screen==0)
			{
			UARTprintf("capture oil temp\n");			
			}
			break;

			case 2:
			//set 50 temp;
			DAC8568_INIT_SET(50,0xF000);
			flag1 = 0;
			UARTprintf("set 50 temp\n");
			break;

			case 3:
			//capture Temperature_of_resistance and Hydrogen_Resistance;
			ADC7738_acquisition(1);
			ADC7738_acquisition_output(1);
			ADC7738_acquisition(2);
			ADC7738_acquisition_output(2);
			flag1 = 0;
			UARTprintf("capture Temperature_of_resistance and Hydrogen_Resistance\n");
			break;

			case 4:
			//Stop heating;
			DAC8568_INIT_SET(0,0);
			flag1 = 0;
			UARTprintf("Stop heating\n");
			break;

			default:                                 /* Error Handling              */
			break;
		}

		switch (flag2)  {
			case 1:
			//200ms LED
			LED_RED_SET
			LED_BLUE_SET
			LED_RED_CLR
			LED_BLUE_CLR
			if(flag_screen==0)
			{
			UARTprintf("200ms LED\n");			
			}
			flag2 = 0;
			break;

			case 2:
			//300ms ADC
			if(flag_screen==0)
			{
			UARTprintf("300ms ADC\n");			
			}				
			ADC7738_acquisition(1);
			ADC7738_acquisition(2);
			flag2 = 0;
			break;

			case 3:
			//600 ms checkself
			device_checkself();
			if(flag_screen==0)
      {
			UARTprintf("600 ms checkself\n");			
			}				
			flag2 = 0;
			break;

			case 4:
			//800ms DS1390
			if(flag_screen==0)
      {
			UARTprintf("800ms DS1390\n");			
			}				
			flag2 = 0;
			break;

			default:                                 /* Error Handling              */
			break;
		}

		if (flag3 == 1)
		{
			//30min FLASH
			if(flag_screen==0)
			{
			UARTprintf("30min FLASH\n");			
			}
			flag3 = 0;
		}
	}
}

