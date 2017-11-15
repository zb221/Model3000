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
#include "M25P16_FLASH.h"
#include "e25LC512.h"
#include "parameter.h"
#include "ModBus.h"

/*-------------------------Global variable region----------------------*/
extern unsigned char flag_command;
extern unsigned char flag_function;
extern unsigned char rcv_buf[100];
extern volatile unsigned char rcv_new;
extern unsigned int rcv_cnt;
extern unsigned char cmd_tmp[CMD_LEN];
extern unsigned char cmd_buf[CMD_LEN];
extern unsigned char flag_screen;
extern int flag1, flag2, flag3, flag4;
extern unsigned char a;
extern REALTIMEINFO CurrentTime;
extern unsigned char flag_mode;

extern unsigned char MODEL_TYPE;

/*Output Parameters*/
extern float PcbTemp;
float H2AG = 0;
extern float OilTemp;
extern float TempResistor;
float H2DG = 0;
float H2G = 0;
float H2SldAv = 0;
float DayROC = 0;
float WeekROC = 0;
float MonthROC = 0;
float SensorTemp = 0;
extern float H2Resistor;

int temperature = 50;	/*sense default temperature*/
int PCB_temp = 40;	/*PCB control default temperature*/

const char print_menu[] = 
	"\n"
	"TimeStamp            PcbTemp  H2AG.ppm  OilTemp  H2DG.ppm  H2G.ppm  H2SldAv  DayROC  WeekROC  MonthROC  Message  \r\n";
const char debug_menu[] =
	"\n"
	"TimeStamp            PcbTemp  H2AG.ppm  OilTemp  H2DG.ppm  H2G.ppm  H2SldAv  DayROC  WeekROC  MonthROC  SensorTemp  H2Resistor  TemResistor  Message  \r\n";\
const char *message_menu[]=
{"rpt",
"wait",
"woff",
"avg",
"OVT"
"htr_off",
"warm_up",
"ramp_up",
"ramp_down",
"PF",
"R1",
"R2",
"R3"};
/***********************************************************
Function:	init peripherals.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: all peripherals init should add here.
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
	Initial_e2prom();
}
/***********************************************************
Function:	serial print data.
Input:	none
Output: none
Author: megzheng
Date: 2017/10/27
Description: setup the timer counter 0 interrupt.
***********************************************************/
void command_print(void)
{
	DS1390_GetTime(&CurrentTime);
	UARTprintf("%d/%d/%d %d:%d:%d",CurrentTime.SpecificTime.year+2000,CurrentTime.SpecificTime.month,
	CurrentTime.SpecificTime.day,CurrentTime.SpecificTime.hour,CurrentTime.SpecificTime.min,CurrentTime.SpecificTime.sec);
	UARTprintf("	%.3f	%.4f	%.4f	%.3f	",OilTemp,TempResistor,H2Resistor,PcbTemp);
	if(temperature>=50)
	{
	UARTprintf(message_menu[1]);
	}
	else
	{
	UARTprintf(message_menu[0]);	
	}
	UARTprintf("\r\n");
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
	//unsigned char i;
	FrecInit();

	init_peripherals();
	Init_ModBus();
	//LC512_Init();
	UARTprintf(print_menu);

	DAC8568_INIT_SET(temperature,2*65536/5);	/*DOUT-C = xV*65536/5*/
	DAC8568_PCB_TEMP_SET(PCB_temp,0x1000);
	
//	M25P16_TEST();
//	LC512_TEST();

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
      UARTprintf("a=%d\n",a);
      memset(rcv_buf,0,rcv_cnt);
      rcv_cnt=0;
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
			/* 1-4min capture 3min oil temp */
			flag1 = 0;
			if(flag_screen==0)
			{
        UARTprintf("1-4min capture 3min oil temp\n");	
			}
			break;

			case 2:
			/* 4-1H4min set 50 temp, keep 1H  */
			temperature = 50;
			DAC8568_INIT_SET(temperature,2*65536/5);
			flag1 = 0;
			UARTprintf("4-1H4min set 50 temp, keep 1H\n");
			break;

			case 3:
			/* 1H4min-1H7min stop heating, capture 3min oil temp */
			temperature = 0;
			DAC8568_INIT_SET(temperature,0);
			flag1 = 0;
			UARTprintf("1H4min-1H7min stop heating, capture 3min oil temp\n");
			break;

			case 4:
			/* 1H7min-2H7min set 50 temp and keep 1H */
			temperature = 50;
			DAC8568_INIT_SET(temperature,2*65536/5);
			flag1 = 0;
			UARTprintf("1H7min-2H7min set 50 temp and keep 1H\n");
			break;

			case 5:
			/* 2H7min-2H10min stop heating and capture oil temp 3min */
			temperature = 0;
			DAC8568_INIT_SET(temperature,0);
			flag1 = 0;
			UARTprintf("2H7min-2H10min stop heating and capture oil temp 3min\n");
			break;

			case 6:
			/* 2H10min-3H40min set 70 temp and keep 1.5H */
			temperature = 70;
			DAC8568_INIT_SET(temperature,2*65536/5);
			flag1 = 0;
			UARTprintf("2H10min-3H40min set 70 temp and keep 1.5H\n");
			break;

			case 7:
			/* 3H40min-4H10min set 50 temp and keep 0.5H */
			temperature = 50;
			DAC8568_INIT_SET(temperature,2*65536/5);
			flag1 = 0;
			UARTprintf("3H40min-4H10min set 50 temp and keep 0.5H\n");
			break;

			case 8:
			/* 4H10min-4H13min stop heating and capture oil temp 3min */
			temperature = 0;
			DAC8568_INIT_SET(temperature,0);
			flag1 = 0;
			UARTprintf("4H10min-4H13min stop heating and capture oil temp 3min\n");
			break;

			default:                                 /* Error Handling              */
			break;
		}

		switch (flag2)  {
			case 1:
			LED_RED_SET
			LED_BLUE_SET
			device_checkself();
			flag2 = 0;
			break;

			case 2:
			LED_RED_CLR
			LED_BLUE_CLR
			flag2 = 0;
			break;

			default:                                 /* Error Handling              */
			break;
		}

		if (flag3 == 1)
		{
			/*30min FLASH*/
			if(flag_screen==0)
			{
			UARTprintf("30min FLASH\n");			
			}
			flag3 = 0;
		}
		
		if (flag4 == 1)
 		{
 			/*1S command_print*/
 			ADC7738_acquisition_output(1);
 			ADC7738_acquisition_output(2);
 			ADC7738_acquisition_output(3);
			command_print();
			flag4 = 0;
		}
		ADC7738_acquisition(1);
		ADC7738_acquisition(2);
		ADC7738_acquisition(3);
		
		if(user_parameter.flag.ubit.recept_ok==1)
		{			
			Data_Ack_Processor();
		}

		if(user_parameter.flag.ubit.recept_write==1)
		{
			RW_ModBus_Data();
		}	

	}
}

