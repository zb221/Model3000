/***********************************************
@		Description: This file is command settings for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: megzheng.
@		Date: 2017/10/16.
***********************************************/
#include <stdio.h>                      /* standard I/O .h-file              */
#include <stdlib.h>
#include <ctype.h>                      /* character functions               */
#include <LPC21xx.H>                    /* LPC21xx definitions               */
#include <string.h>
#include "main.h"                    /* global project definition file    */
#include "config.h"
#include "Peripherals_LPC2194.h"
#include "Command.h"
#include "parameter.h"
#include "DAC8568.h"
#include "DS1390.h"
#include "M25P16_FLASH.h"
#include "e25LC512.h"
#include "isp.h"
#include "Modbus.h"

unsigned char cmd_tmp[60] = {0};
unsigned char cmd_buf[60] = {0};
unsigned char flag_command = 0;//flag_command��ֵ��Ӧ��Ӧ������ִ�к���
unsigned char flag_function = 0;//flag_function��ֵ��Ӧÿ�������к���ִ�е���һ��
unsigned char flag_screen = 0;//flag_screen 0 �������� 1 �رջ���
unsigned char flag_relay1 = 1;//relay 1 status flag
unsigned char flag_relay2 = 2;//relay 2 status flag
unsigned char flag_relay3 = 3;//relay 3 status flag
unsigned char flag_chaoshi = 0;//��ֹһֱ����ĳ�����ĳ�������У�����һ����С��ʹ�����0
unsigned char flag_done = 0;//�����������������֣������������Ϊ0������Ϊ1����ʾ��Ҫ��������
unsigned char flag_relay_done = 0;//�ж����õ����ĸ�relay�Ĺ��ܣ�relay����������ת��־λ
unsigned char flag_mode = 1;//mode flag
unsigned char a = 0;//cmd_tmp�ĳ���
unsigned int readlog_number = 0;//��ȡ������Ϣ����

extern const char print_menu[];
extern const char debug_menu[];
extern const char calibrate_menu[];
extern unsigned char print_time;

extern REALTIMEINFO CurrentTime;
//*****************************************************************************/
//cmd catalogue
//*****************************************************************************/  
cmd_list_struct  cmd_list[]={  
/*����  ����Ŀ¼    ������        ������Ϣ*/	
{"a",       1,      alarm_arg,     "alarm<arg1> <arg2>"},
{"d",       2,      config_arg_d1,    "config"}, 
{"da",      3,      da_arg,        "config"},
{"db",      4,      db_arg,        "config"},
{"dx",      5,      dx_arg,        "config"},
{"g",       6,      gg_arg,        "config"},
{"h",       7,      aop_arg,       "config"},
{"i",       8,      aoerr_arg,     "config"},
{"is",      9,      install_arg,   "config"}, 
{"rs",      10,     date_arg,      "config"},
{"t",       11,     record_arg,    "config"},
{"x",       12,     clear_arg,     "config"},
{"ci",      13,     ci_arg,        "config"},
{"mi",      14,     setmid_arg,    "config"},
{"cf",      15,     cf_arg,        "config"},
{"d 0",     16,     config_arg_d0,    "config"}, 
{"d 1",     2,      config_arg_d1,    "config"}, 
{"d 2",     17,     config_arg_d2,    "config"}, 
{"d 3",     18,     config_arg_d3,    "config"},
{"cl",      19,     firmware_arg,   "firmware"},
};

/***********************************************************
Function:get true char stream 
Input: none
Output: none
Author: megzheng
Date: 2017/10/17
Description:  .
***********************************************************/
unsigned long int get_true_char_stream(unsigned char *dest,const unsigned char *src)
{
  unsigned long int dest_count=0;  
  unsigned long int src_count=0;
	
	while(src[src_count]!=0x0d && src[src_count+1]!=0x0a)
	{
		switch(src[src_count])
		{
			case 0x08://backspace
			{
				if(dest_count>0)
				{
				dest_count--;
				}
				src_count++;
			}	
			break;
			case 0x1B:
			//flag ��λ��������������
			break;
			default:
			{
				if(isprint(src[src_count]))//ȥ�����ɴ�ӡ�ַ�
				{
					if((src[src_count]>='A')&&(src[src_count]<='Z'))
					{
						dest[dest_count]=src[src_count]+32;
						src_count++;
						dest_count++;
					}
					else
					{
					dest[dest_count]=src[src_count];
					src_count++;
					dest_count++;
					}			
				}				
			}
			break;
		}
	}	
	return dest_count; 
}
/***********************************************************
Function:get true cmd function 
Input: none
Output: stream len
Author: megzheng
Date: 2017/10/17
Description:  .
***********************************************************/
unsigned char findcmdfunction(unsigned char *tmp)
{
	unsigned char i = 0;
	
	for(i=0;i<sizeof(cmd_list)/sizeof(cmd_list[0]);i++)
	{
		if(strcmp(cmd_list[i].cmd_file,cmd_tmp) == 0)
		{
			flag_command = cmd_list[i].max_args; 
			return 1;
		}else{
			flag_screen = 0;
			flag_function = 0;
			flag_command = 0;
			flag_screen = 0;
			flag_chaoshi = 0;
		}
	}
//	UARTprintf("There is no corresponding command. Please check the input command.\n");
	return 0; 
}
/***********************************************************
Function:setting relay 0 1 2 3
Input: none
Output: 1 respond chaoshi 0 no chaoshi
Author: megzheng
Date: 2017/10/19
Description:  .
***********************************************************/
unsigned char judge_flag_chaoshi(void)
{
	if(flag_chaoshi>=20)
	{
	  flag_chaoshi=0;
		flag_function=0;
		flag_command=0;
		return 1;
	}
	return 0;
}
/***********************************************************
Function:setting relay 0 1 2 3
Input: none
Output: none
Author: megzheng
Date: 2017/10/19
Description:  .
***********************************************************/
void alarm_arg(void)//�������ӽ��̵���״ֵ̬����E2P��
{
	unsigned char i = 0;
	unsigned int temp = 0;
	
	switch(flag_function){
		case 0:
			flag_chaoshi = 0;
			UARTprintf("Relay #1 Select mode:\n0 - Disable\n1 - Hydrogen level\n2 - Rate of Change\n3 - OilTemp level\nSelect function:\n");
			flag_function++;
			flag_chaoshi++;
			break;
		
		case 1:
			if(strlen(cmd_tmp)>0)
			{
				switch(cmd_tmp[0]){
					case 0x30:
						UARTprintf("Relay #1 disable\n");
						flag_relay1 = 0;
						flag_function++;
						run_parameter.status_flag.ubit.relay1=0;
					break;
					case 0x31:
						UARTprintf("Enter Trigger (ppm H2):");
						flag_relay1 = 1;
						flag_function = 7;
					break;
					case 0x32:
						UARTprintf("Enter Trigger (ppm H2/day):");
						flag_relay1 = 2;
						flag_function = 8;			
					break;
					case 0x33:
						UARTprintf("Enter Trigger (Oil temperature):");
						flag_relay1 = 3;
						flag_function = 9;
					break;
					default:
						UARTprintf("not a illegal interger, set 0-3 at here\n");
						flag_chaoshi++;
					  if (flag_chaoshi > 5){
						  UARTprintf("Please input A command again and set 0-3 at here, exit A OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
			}
			flag_relay_done = 1;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
			
		case 2:
			flag_chaoshi = 0;
			UARTprintf("Relay #2 Select mode:\n0 - Disable\n1 - Hydrogen level\n2 - Rate of Change\n3 - OilTemp level\nSelect function: ");
			flag_function++;
			flag_chaoshi++;
			break;
		
		case 3:
			if(strlen(cmd_tmp)>0)
			{
				switch(cmd_tmp[0]){
					case 0x30:
						UARTprintf("Relay #2 disable\n");
						flag_relay2 = 0;
						flag_function++;
						run_parameter.status_flag.ubit.relay2=0;	
					break;
					case 0x31:
						UARTprintf("2 Enter Trigger (ppm H2):");
						flag_relay2 = 1;
						flag_function = 7;
					break;
					case 0x32:
						UARTprintf("2 Enter Trigger (ppm H2/day):");
						flag_relay2 = 2;
						flag_function = 8;				
					break;
					case 0x33:
						UARTprintf("2 Enter Trigger (Oil temperature):");
						flag_relay2 = 3;
						flag_function = 9;		
					break;
					default:
						UARTprintf("not a illegal interger, set 0-3 at here\n");
						flag_chaoshi++;	
					  if (flag_chaoshi > 5){
						  UARTprintf("Please input A command again and set 0-3 at here, exit A OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
			}
			flag_relay_done = 3;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
			
		case 4:
			flag_chaoshi = 0;
			UARTprintf("Relay #3 Select mode:\n0 - Disable\n1 - Hydrogen level\n2 - Rate of Change\n3 - OilTemp level\nSelect function: ");
			flag_function++;
			flag_chaoshi++;
			break;
				
		case 5:
			if(strlen(cmd_tmp)>0)
			{
					switch(cmd_tmp[0]){
						case 0x30:
							UARTprintf("Relay #3 disable\n");
							flag_relay3 = 0;
							flag_function++;
						  run_parameter.status_flag.ubit.relay3=0;
						break;
						case 0x31:
							UARTprintf("3 Enter Trigger (ppm H2):");
							flag_relay3 = 1;
							flag_function = 7;
						break;
						case 0x32:
							UARTprintf("3 Enter Trigger (ppm H2/day):");
							flag_relay3 = 2;
							flag_function = 8;					
						break;
						case 0x33:
							UARTprintf("3 Enter Trigger (Oil temperature):");
							flag_relay3 = 3;
							flag_function = 9;						
						break;
						default:
							UARTprintf("not a illegal interger, set 0-3 at here\n");
							flag_chaoshi++;
							if (flag_chaoshi > 5){
								UARTprintf("Please input A command again and set 0-3 at here, exit A OK.\n");
								flag_function = 0;
								flag_command = 0;
								flag_screen = 0;
								flag_chaoshi = 0;
							}
						break;				
					}
			}
			flag_relay_done = 5;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
			
		case 6:
			UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
			flag_function = 0;
		  flag_command = 0;
		  flag_screen = 0;
		  flag_chaoshi = 0;
			break;

		case 7:
		  if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
					flag_done = 0;
				  if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
					{
	          flag_done = 1;
						UARTprintf("Please set 0-9 at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 10){
							UARTprintf("Please input A command again and set 0-9 at here, exit A OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
						break;				
					}
				}
				if(flag_done == 0)
				{
					temp = atoi(cmd_tmp);
					if (temp >= 5000)
						temp = 5000;
					run_parameter.h2_ppm_alert_low_l16.hilo = temp;
					e2prom512_write(&run_parameter.h2_ppm_alert_low_l16.ubit.lo,2,153*2);
					UARTprintf("relay h2 ppm alert low  %d\n",run_parameter.h2_ppm_alert_low_l16.hilo | run_parameter.h2_ppm_alert_low_h16.hilo << 16);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					if(flag_relay_done == 1)
					{
					  flag_function = 2;
					}
					if(flag_relay_done ==3 )
					{
					  flag_function = 4;
					}
					if(flag_relay_done == 5)
					{
					  flag_function = 6;
					}
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;					 
		  break;
			
		case 8:
		  if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
					flag_done = 0;
				  if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
					{
	          flag_done = 1;
						UARTprintf("Please set 0-9 at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 10){
							UARTprintf("Please input A command again and set 0-9 at here, exit A OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
						break;				
					}
				}
				if(flag_done == 0)
				{
					temp = atoi(cmd_tmp);
					if (temp >= 5000)
						temp = 5000;
					run_parameter.h2_ppm_alarm_low_l16.hilo = temp;
					e2prom512_write(&run_parameter.h2_ppm_alarm_low_l16.ubit.lo,2,155*2);
					UARTprintf("rate of change  %d\n",temp);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					if(flag_relay_done == 1)
					{
					  flag_function = 2;
					}
					if(flag_relay_done == 3)
					{
					  flag_function = 4;
					}
					if(flag_relay_done == 5)
					{
					  flag_function = 6;
					}
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;	
		  break;
			
		case 9:
			if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
					flag_done = 0;
				  if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
					{
	          flag_done = 1;
						UARTprintf("Please set 0-9 at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 10){
							UARTprintf("Please input A command again and set 0-9 at here, exit A OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
						break;				
					}
				}
				if(flag_done == 0)
				{
					temp = atoi(cmd_tmp);
					if (temp >= 105)
						temp = 105;
					run_parameter.OilTemp_Alarm_celsius.hilo = temp;
					e2prom512_write(&run_parameter.OilTemp_Alarm_celsius.ubit.lo,2,156*2);
					UARTprintf("Oil temperature  %d\n",temp);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					if(flag_relay_done == 1)
					{
					  flag_function = 2;
					}
					if(flag_relay_done == 3)
					{
					  flag_function = 4;
					}
					if(flag_relay_done == 5)
					{
					  flag_function = 6;
					}
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;				
		  break;
			
		default:
			flag_function = 0;
			flag_command = 0;
			flag_screen = 0;
			flag_chaoshi = 0;
      break;
	}
}

/***********************************************************
Function:show product information
Input: none
Output: none
Author: megzheng
Date: 2017/10/19
Description:  .
***********************************************************/
void config_arg_d0(void)//e2c do not save just init give these value
{
	switch(flag_function){
		case 0:
			UARTprintf("Product information:\n\
			Model Number : %s\n\
			Serial Number: %s\n\
			Sensor model : %s\n\
			Firmware Rev : %s\n\
			Hardware Version: %s\n\
			Latest Calibration\n\
			Factory: %s\n\
			Field: %d-%d-%d\n",
			run_parameter.model_number.model_number_str,
			run_parameter.product_serial_number.product_serial_number_str,
			run_parameter.sensor_serial_number.sensor_serial_number_str,
			run_parameter.firmware_revesion.firmware_revesion_str,
			run_parameter.hardware_version.hardware_version_str,
			run_parameter.factory.factory_str,	
			run_parameter.field_calibration_date.year,
			run_parameter.field_calibration_date.month,
			run_parameter.field_calibration_date.day);
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;
		
		default:
			flag_function = 0;
			flag_command = 0;
			flag_screen = 0;	
		  break;
	}
}
/***********************************************************
Function:show product information
Input: none
Output: none
Author: megzheng
Date: 2017/10/19
Description:  .
***********************************************************/
void config_arg_d1(void)
{
		switch(flag_function){
		case 0:
			UARTprintf("User configuration is:\n\
			Hydrogen reporting range(in oil LowH2Range-HighH2Range): %0.4f - %0.4f %%H2\n",
			(run_parameter.h2_ppm_report_low_l16.hilo/10000.F),(run_parameter.h2_ppm_report_high_l16.hilo/10000.F));
			#ifdef  OutVoltage
			UARTprintf("\
			Current Output is disabled\n\
			Isolated Output is enabled: %0.1fV to %0.1fV (LowH2Voltage-HighH2Voltage)\n\
			Error output is: %0.1fV\n\
			Not-Ready output is %0.1fV\n",cmd_ConfigData.LowVout,cmd_ConfigData.HighVout,cmd_ConfigData.ErrVout,cmd_ConfigData.NotRVout);
			#else 
			UARTprintf("\
			Voltage Output is disabled\n\
			Isolated Output is enabled: %0.1fmA to %0.1fmA (LowH2Current-HighH2Current)\n\
			Error output is: %0.1fmA\n\
			Not-Ready output is %0.1fmA\n",
		  (double)((run_parameter.h2_ppm_out_current_low.hilo)/100.F),
			(double)((run_parameter.h2_ppm_out_current_high.hilo)/100.F),
			(double)((run_parameter.h2_ppm_error_out_current.hilo)/100.F),
			(double)((run_parameter.h2_ppm_no_ready_out_current.hilo)/100.F));	
			#endif
			if(flag_relay1==0){UARTprintf("Relays#1:disable\n");}else{UARTprintf("Relays#1:enable\n");}
			if(flag_relay2==0){UARTprintf("Relays#2:disable\n");}else{UARTprintf("Relays#2:enable\n");}
			if(flag_relay3==0){UARTprintf("Relays#3:disable\n");}else{UARTprintf("Relays#3:enable\n");}
			
			if(flag_relay1==1)
			{
				UARTprintf("(relays#1)");
				UARTprintf(" threshold is %d ppm Hydrogen\n",
				run_parameter.h2_ppm_alert_low_l16.hilo);
			}
			if(flag_relay2==1)
			{
				UARTprintf("(relays#2)");
				UARTprintf(" threshold is %d ppm Hydrogen\n",
				(run_parameter.h2_ppm_alert_low_l16.hilo));
			}
			if(flag_relay3==1)
			{
				UARTprintf("(relays#3)");
				UARTprintf(" threshold is %d ppm Hydrogen\n",
				(run_parameter.h2_ppm_alert_low_l16.hilo));
			}
			if(flag_relay1==2)
			{
				UARTprintf("(relays#1)");
				UARTprintf(" threshold is %d ppm Hydrogen/Day\n",
				(run_parameter.h2_ppm_alarm_low_l16.hilo));
			}
			if(flag_relay2==2)
			{
				UARTprintf("(relays#2)");
				UARTprintf(" threshold is %d ppm Hydrogen/Day\n",
				(run_parameter.h2_ppm_alarm_low_l16.hilo));
			}
			if(flag_relay3==2)
			{
				UARTprintf("(relays#3)");
				UARTprintf(" threshold is %d ppm Hydrogen/Day\n",
				(run_parameter.h2_ppm_alarm_low_l16.hilo));
			}
			if(flag_relay1==3)
			{
				UARTprintf("(relays#1)");
				UARTprintf(" threshold is %d degrees Celsius\n",
				(run_parameter.OilTemp_Alarm_celsius.hilo));
			}
			if(flag_relay2==3)
			{
				UARTprintf("(relays#2)");
				UARTprintf(" threshold is %d degrees Celsius\n",
				(run_parameter.OilTemp_Alarm_celsius.hilo));
			}
			if(flag_relay3==3)
			{
				UARTprintf("(relays#3)");
				UARTprintf(" threshold is %d degrees Celsius\n",
				(run_parameter.OilTemp_Alarm_celsius.hilo));
			}
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
			
		default:
			flag_function=0;
			flag_command=0;
			flag_screen=0;
			break;
	}
}
void config_arg_d2(void)
{
		switch(flag_function){
		case 0:
			UARTprintf("Manufacturing information is:\n\
			Sensor Serial Number: %s\n\
			Sensor Board Serial Number: %s\n\
			Interface Board Serial Number: %s\n\
			Date Built: 20180101\n",run_parameter.sensor_serial_number.sensor_serial_number_str,
			run_parameter.sensor_board_serial_number.sensor_board_serial_number_str,
			run_parameter.interface_board_serial_number.interface_board_serial_number_str); 	
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
		break;

		default:
			flag_function = 0;
			flag_command = 0;
			flag_screen = 0;
			break;
	}
}

void config_arg_d3(void)
{
		switch(flag_function){
		case 0:						 
			UARTprintf("Product Configuration:\n\
			Sample rate is %us/sample (times/sample)\n",
			print_time);
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;

		default:
			flag_function=0;
			flag_command=0;
			flag_screen=0;
			break;
	}
}

void firmware_command(void)
{
    UARTprintf("Ready.... got room at image 2 ...send Hex File...\n");
    IO1CLR	= EN_485_DE; //RS485 rev status
    ISPbyCode();         //write new firmware
}

void firmware_arg(void)
{
	switch(flag_function){
		case 0:
			flag_chaoshi = 0;
			UARTprintf("Download new firmware (Y/N)?");
			flag_function++;
			flag_chaoshi++;
			break;

		case 1:
			if(strlen(cmd_tmp)>0)
			{
					switch(cmd_tmp[0]){
						case 0x79://y             
							flag_function = 2;
						break;
						case 0x6e://n            
							flag_function=0;
							flag_command=0;
							flag_screen=0;	
							flag_chaoshi=0;
							UARTprintf("No change - Done\n");
							break;
						default:
							UARTprintf("not a illegal interger, set Y or N\n");
							flag_chaoshi++;
							if (flag_chaoshi > 5){
								UARTprintf("Please input CL command again and set Y or N at here, exit CL OK.\n");
								flag_function = 0;
								flag_command = 0;
								flag_screen = 0;
								flag_chaoshi = 0;
							}
						break;	
					}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a=0;
			break;
			
		 case 2:
			firmware_command();
		  flag_function++;
		  break;	 

		default:
			flag_function = 0;
			flag_command = 0;
			flag_screen = 0;
			flag_chaoshi = 0;
			break;
	}

}
/***********************************************************
Function:command da calibrate H2 ppm
Input: none
Output: none
Author: megzheng
Date: 2017/10/22
Description:  .
***********************************************************/
void da_arg(void)
{
	unsigned int temp = 0;	
	switch(flag_function){
		case 0:
			temp = (unsigned int)(output_data.H2AG / 20);
			UARTprintf("Current H2 value is %d ppm H2\n",temp);
		  Intermediate_Data.da_H2ppm = temp;
//			run_parameter.h2_ppm_calibration_gas_h16.hilo = temp >> 16;
//			run_parameter.h2_ppm_calibration_gas_l16.hilo = temp & 0xFFFF;
//			e2prom512_write(&run_parameter.h2_ppm_calibration_gas_h16.ubit.lo,4,126*2);
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;

		default:
			flag_function=0;
			flag_command=0;
			flag_screen=0;
			break;
	}
}
/***********************************************************
Function:command db calibrate H2 ppm
Input: none
Output: none
Author: megzheng
Date: 2017/10/22
Description:  .
***********************************************************/
void db_arg(void)
{
	unsigned char i = 0;
	unsigned int temp = 0;
	
	switch(flag_function){
		case 0:
			flag_chaoshi = 0;
			UARTprintf("Enter actual hydrogen in ppm: ");
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;

		case 1:
			if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
					flag_done = 0;
				  if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
					{
	          flag_done = 1;
						UARTprintf("Please set 0-9 at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input DB command again and set 0-9 at here, exit DB OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}		
					}
				}
				if(flag_done == 0)
				{
					temp = atoi(cmd_tmp);
					if (temp > 5000)
						temp = 5000;
					Intermediate_Data.db_H2ppm = temp;
					flag_function++;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
		  break;
			
		case 2:
			flag_chaoshi = 0;
			UARTprintf("Set hydrogen to %d ppm (Y/N)?",Intermediate_Data.db_H2ppm);		
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 3:
			if(strlen(cmd_tmp)>0)
			{
				switch(cmd_tmp[0]){
					case 0x79://y
						temp = Intermediate_Data.db_H2ppm - Intermediate_Data.da_H2ppm;
						run_parameter.h2_ppm_calibration_gas_h16.hilo = temp >> 16;
						run_parameter.h2_ppm_calibration_gas_l16.hilo = temp & 0xFFFF;
						e2prom512_write(&run_parameter.h2_ppm_calibration_gas_h16.ubit.lo,4,126*2);
						flag_function++;
					break;
					case 0x6e://n            
						UARTprintf("Give up the hydrogen calibration in oil, exit DB OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					break;
					default:
						UARTprintf("not a illegal interger, Please set Y or N at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input DB command again and set Y or N at here, exit DB OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 4:
			flag_chaoshi = 0;
			UARTprintf("Calibration Gas finished\n");	
			UARTprintf("Cal. date is: %d-%d-%d (Y/N)?",
			run_parameter.calibration_date.year,
			run_parameter.calibration_date.month,
			run_parameter.calibration_date.day); 
			flag_function++;
			break;
		
		case 5:
			if(strlen(cmd_tmp)>0)
			{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function++;
					break;
					case 0x6e://n            
						flag_function = 8;						
					break;
					default:
						UARTprintf("not a illegal interger, Please set Y or N at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input DB command again and set Y or N at here, exit DB OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 6:
			UARTprintf("Cal Message:......\n");    
			UARTprintf("Cal. date : %d-%d-%d\r\n",run_parameter.calibration_date.year,
			run_parameter.calibration_date.month,
			run_parameter.calibration_date.day);
			UARTprintf("DB command work OK, exit success.\n");
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;
		
		case 8:
			flag_chaoshi = 0;
			UARTprintf("Enter Year: ");
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 9:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input DB command again and set 0-9 at here, exit DB OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}		
				}
				if(flag_done==0)
				{
					run_parameter.calibration_date.year = atoi(cmd_tmp);
					flag_function++;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
			
		case 10:
			UARTprintf("Enter Month: ");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;					
			break;
		
		case 11:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input DB command again and set 0-9 at here, exit DB OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					run_parameter.calibration_date.month = atoi(cmd_tmp);
					flag_function++;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;
		
		case 12:
			UARTprintf("Enter Day: ");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;			
			break;
		
		case 13:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input DB command again and set 0-9 at here, exit DB OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.calibration_date.day = atoi(cmd_tmp);
					e2prom512_write(&run_parameter.calibration_date.day,4,128*2);
					flag_function = 7;
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		default:
			flag_function=0;
			flag_command=0;
			flag_screen=0;	
			flag_chaoshi=0;
		  break;
	}
}


void dx_arg(void)
{
	unsigned char i = 0;
	unsigned int temp = 0;
	
	switch(flag_function){
		case 0:
			flag_chaoshi = 0;
			UARTprintf("Enter actual hydrogen in ppm: ");
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;

		case 1:
			if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
					flag_done = 0;
				  if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
					{
	          flag_done = 1;
						UARTprintf("Please set 0-9 at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input DX command again and set 0-9 at here, exit DX OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}		
					}
				}
				if(flag_done == 0)
				{
					temp = atoi(cmd_tmp);
					if (temp > 5000)
						temp = 5000;
					Intermediate_Data.db_H2ppm = temp;
					flag_function++;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
		  break;
			
		case 2:
			flag_chaoshi = 0;
			UARTprintf("Set hydrogen to %d ppm (Y/N)?",Intermediate_Data.db_H2ppm);		
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 3:
			if(strlen(cmd_tmp)>0)
			{
				switch(cmd_tmp[0]){
					case 0x79://y
						temp = Intermediate_Data.db_H2ppm - (unsigned int)(output_data.H2AG / 20);
						run_parameter.h2_ppm_calibration_gas_h16.hilo = temp >> 16;
						run_parameter.h2_ppm_calibration_gas_l16.hilo = temp & 0xFFFF;
						e2prom512_write(&run_parameter.h2_ppm_calibration_gas_h16.ubit.lo,4,126*2);
						flag_function++;
					break;
					case 0x6e://n            
						UARTprintf("Give up the hydrogen calibration in oil, exit DX OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					break;
					default:
						UARTprintf("not a illegal interger, Please set Y or N at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input DX command again and set Y or N at here, exit DX OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 4:
			flag_chaoshi = 0;
			UARTprintf("Calibration Gas finished\n");	
			UARTprintf("Cal. date is: %d-%d-%d (Y/N)?",
			run_parameter.calibration_date.year,
			run_parameter.calibration_date.month,
			run_parameter.calibration_date.day); 
			flag_function++;
			break;
		
		case 5:
			if(strlen(cmd_tmp)>0)
			{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function++;
					break;
					case 0x6e://n            
						flag_function = 8;						
					break;
					default:
						UARTprintf("not a illegal interger, Please set Y or N at here, try again.\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input DX command again and set Y or N at here, exit DX OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 6:
			UARTprintf("Cal Message:......\n");    
			UARTprintf("Cal. date : %d-%d-%d\r\n",run_parameter.calibration_date.year,
			run_parameter.calibration_date.month,
			run_parameter.calibration_date.day);
			UARTprintf("DX command work OK, exit success.\n");
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;
		
		case 8:
			flag_chaoshi = 0;
			UARTprintf("Enter Year: ");
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 9:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input DX command again and set 0-9 at here, exit DX OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}		
				}
				if(flag_done==0)
				{
					run_parameter.calibration_date.year = atoi(cmd_tmp);
					flag_function++;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
			
		case 10:
			UARTprintf("Enter Month: ");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;					
			break;
		
		case 11:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input DX command again and set 0-9 at here, exit DX OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					run_parameter.calibration_date.month = atoi(cmd_tmp);
					flag_function++;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;
		
		case 12:
			UARTprintf("Enter Day: ");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;			
			break;
		
		case 13:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input DX command again and set 0-9 at here, exit DX OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.calibration_date.day = atoi(cmd_tmp);
					e2prom512_write(&run_parameter.calibration_date.day,4,128*2);
					flag_function = 7;
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		default:
			flag_function=0;
			flag_command=0;
			flag_screen=0;	
			flag_chaoshi=0;
		  break;
	}
}


void gg_arg(void)
{
	switch(flag_function){
		case 0:
			switch (output_data.MODEL_TYPE){
			case 1:
			UARTprintf(print_menu);
			break;

			case 2:
			UARTprintf(debug_menu);
			break;

			case 3:
			UARTprintf(calibrate_menu);
			break;

			default:
			break;
		}
		flag_function++;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a = 0;
		break;

		default:
			flag_function=0;
			flag_command=0;
			flag_screen=0;	
			break;
	}
}
void aop_arg(void)//h
{
	unsigned char i = 0;
	switch(flag_function){
		case 0:
			UARTprintf("Hydrogen reporting range(in oil LowH2Range-HighH2Range): %0.4f - %0.4f%% H2\n",
			(run_parameter.h2_ppm_report_low_l16.hilo/10000.F),(run_parameter.h2_ppm_report_high_l16.hilo/10000.F));
			UARTprintf("Change (Y/N)?");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function = 3;
					break;
					case 0x6e://n            
						flag_function=0;
						flag_command=0;
						flag_screen=0;	
						flag_chaoshi=0;
						UARTprintf("\nGive up change the hydrogen range, exit H OK.\r\n\r\n");
						break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input H command again and set Y or N at here, exit H OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a = 0;		
		break;
		
		case 3:
			UARTprintf("Enter new LowH2Range: ");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 4:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input H command again and set 0-9 at here, exit H OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_report_low_l16.hilo = atof(cmd_tmp)*10000;
					e2prom512_write(&run_parameter.h2_ppm_report_low_h16.ubit.lo,4,141*2);
					flag_function++;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 5:
		UARTprintf("Enter new HighH2Range: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a = 0;
		break;
		
		case 6:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input H command again and set 0-9 at here, exit H OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_report_high_l16.hilo = atof(cmd_tmp)*10000;
					if (run_parameter.h2_ppm_report_high_l16.hilo > run_parameter.h2_ppm_report_low_l16.hilo){
					e2prom512_write(&run_parameter.h2_ppm_report_high_h16.ubit.lo,4,143*2);
					UARTprintf("New hydrogen reporting range(in oil LowH2Range-HighH2Range): %0.4f - %0.4f%% H2\n",
					(float)run_parameter.h2_ppm_report_low_l16.hilo/10000,(float)(run_parameter.h2_ppm_report_high_l16.hilo)/10000);
					UARTprintf("...Wait...SAVED - Done\n");
					}else{
					UARTprintf("Please input H command again and set HighH2Range > LowH2Range, exit H OK.\n");
					}
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a = 0;
		break;
		
		default:
			flag_function=0;
			flag_command=0;
			flag_screen=0;	
			flag_chaoshi=0;
			break;
	}

}

void aoerr_arg(void)//i
{
	unsigned char i = 0;
	switch(flag_function){
		case 0:
			UARTprintf("DAC range is %0.2fmA to %0.2fmA(LowH2Current-HighH2Current), error output is %0.2fmA, not ready output is %0.2fmA\n",
			(float)run_parameter.h2_ppm_out_current_low.hilo/100,
			(float)run_parameter.h2_ppm_out_current_high.hilo/100,
			(float)run_parameter.h2_ppm_error_out_current.hilo/100,
			(float)run_parameter.h2_ppm_no_ready_out_current.hilo/100);		
			UARTprintf("Change (Y/N)?");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function = 3;
					break;
					case 0x6e://n            
						flag_function=0;
						flag_command=0;
						flag_screen=0;	
						flag_chaoshi=0;
						UARTprintf("\nGive up change the hydrogen range, exit I OK.\r\n\r\n");
						break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input I command again and set Y or N at here, exit I OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;	
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a = 0;		
		break;

		case 3:
		UARTprintf("Enter new low H2 output current: ");
		flag_function++;	
    flag_chaoshi = 0;		
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a = 0;
		break;

		case 4:
		if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input I command again and set 0-9 at here, exit I OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_out_current_low.hilo = atof(cmd_tmp)*100;
					e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);
					flag_function++;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;
		
		case 5:
			UARTprintf("Enter new high H2 output current: ");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 6:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input I command again and set 0-9 at here, exit I OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_out_current_high.hilo = atof(cmd_tmp)*100;
					if (run_parameter.h2_ppm_out_current_high.hilo > run_parameter.h2_ppm_out_current_low.hilo){
					e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);
					flag_function++;
					}else{
					UARTprintf("Please input I command again and set HighH2Current > LowH2Current, exit I OK.\n");
					flag_function = 0;
					flag_command = 0;
					flag_screen = 0;
					flag_chaoshi = 0;
					}
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;			
			break;
		
		case 7:
			UARTprintf("Enter new error H2 output current: ");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 8:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input I command again and set 0-9 at here, exit I OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_error_out_current.hilo = atof(cmd_tmp)*100;
					if (run_parameter.h2_ppm_error_out_current.hilo < run_parameter.h2_ppm_out_current_low.hilo){
					e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);
					flag_function++;
					}else{
					UARTprintf("Please input I command again and set error_out_current < LowH2Current, exit I OK.\n");
					flag_function = 0;
					flag_command = 0;
					flag_screen = 0;
					flag_chaoshi = 0;
					}
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;			
			break;
		
		case 9:
			UARTprintf("Enter new not ready H2 output current: ");
			flag_function++;		
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 10:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input I command again and set 0-9 at here, exit I OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_no_ready_out_current.hilo = atof(cmd_tmp)*100;
					if ((run_parameter.h2_ppm_no_ready_out_current.hilo < run_parameter.h2_ppm_out_current_low.hilo)&&(run_parameter.h2_ppm_no_ready_out_current.hilo != run_parameter.h2_ppm_error_out_current.hilo)){
					e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);
					flag_function++;
					UARTprintf("New DAC range is %0.2fmA to %0.2fmA(LowH2Current-HighH2Current), error output is %0.2fmA, not ready output is %0.2fmA\n",
					(float)run_parameter.h2_ppm_out_current_low.hilo/100,
					(float)run_parameter.h2_ppm_out_current_high.hilo/100,
					(float)run_parameter.h2_ppm_error_out_current.hilo/100,
					(float)run_parameter.h2_ppm_no_ready_out_current.hilo/100);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}else{
					UARTprintf("Please input I command again, set no_ready_out_current < LowH2Current and no_ready_out_current != error_out_current, exit I OK.\n");
					flag_function = 0;
					flag_command = 0;
					flag_screen = 0;
					flag_chaoshi = 0;
					}
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;			
			break;
		
		default:
			flag_function=0;
			flag_command=0;
			flag_screen=0;	
			flag_chaoshi=0;
			break;
	}
}

void is_time_set(unsigned char type)
{
	unsigned char Temp;
	unsigned char Tens, units;
	REALTIMEINFO TimeBCD;
  REALTIMEINFO	RealTime_Modbus;
	static unsigned char Y = 0, M = 0, D = 0, H = 0, F = 0, S = 0;
	
	switch (type){
		case 0:
		Temp = atoi(cmd_tmp);		//
		Tens = Temp / 10;
		units = Temp % 10;
		TimeBCD.SpecificTime.sec = ((Tens << 4) & 0xF0) | (units & 0x0F);
			S = TimeBCD.SpecificTime.sec;
		
		run_parameter.reserved_parameter35 = (0x20<<8 | Y);
		run_parameter.reserved_parameter341 = (M<<8 | D);
		run_parameter.reserved_parameter36 = H;
		run_parameter.reserved_parameter37 = (F<<8 | S);

		RealTime_Modbus.Real_Time[1]=run_parameter.reserved_parameter341>>8;//��
		RealTime_Modbus.Real_Time[2]=run_parameter.reserved_parameter341&0xFF;//��	
		RealTime_Modbus.Real_Time[0]=run_parameter.reserved_parameter35&0xFF;//��

		RealTime_Modbus.Real_Time[4]=run_parameter.reserved_parameter36&0xFF;//ʱ	
		RealTime_Modbus.Real_Time[5]=run_parameter.reserved_parameter37>>8;//��
		RealTime_Modbus.Real_Time[6]=run_parameter.reserved_parameter37&0xFF;//��
		UARTprintf("Change time to 20%x-%x-%x %x:%x:%x",\
		(long int)RealTime_Modbus.Real_Time[0],\
		(long int)RealTime_Modbus.Real_Time[1],\
		(long int)RealTime_Modbus.Real_Time[2],(long int)RealTime_Modbus.Real_Time[4],\
		(long int)RealTime_Modbus.Real_Time[5],(long int)RealTime_Modbus.Real_Time[6]);		
		//*/
		Write1390(REG_STATUS,0x80); //д������
		Write1390(REG_YEAR,RealTime_Modbus.Real_Time[0]); 
		Write1390(REG_MONTH,RealTime_Modbus.Real_Time[1]);
		Write1390(REG_DAY,RealTime_Modbus.Real_Time[2]);
		////	Write1390(REG_WEEK,TimeBCD.SpecificTime.week);	
		Write1390(REG_HOUR,RealTime_Modbus.Real_Time[4]);
		Write1390(REG_MIN,RealTime_Modbus.Real_Time[5]);
		Write1390(REG_SEC,RealTime_Modbus.Real_Time[6]);
		Write1390(REG_STATUS,0x00); //��ֹд��		
		
		break;
		case 1:
		Temp = atoi(cmd_tmp);		//
		Tens = Temp / 10;
		units = Temp % 10;
		TimeBCD.SpecificTime.min = ((Tens << 4) & 0xF0) | (units & 0x0F);
			F = TimeBCD.SpecificTime.min;
		break;
		case 2:
		Temp = atoi(cmd_tmp);		//
		Tens = Temp / 10;
		units = Temp % 10;
		TimeBCD.SpecificTime.hour = ((Tens << 4) & 0xF0) | (units & 0x0F);
			H = TimeBCD.SpecificTime.hour;
		break;
		case 3:
		Temp = atoi(cmd_tmp);		//
		Tens = Temp / 10;
		units = Temp % 10;
		TimeBCD.SpecificTime.day = ((Tens << 4) & 0xF0) | (units & 0x0F);
			D = TimeBCD.SpecificTime.day;
		break;
		case 4:
		Temp = atoi(cmd_tmp);		//
		Tens = Temp / 10;
		units = Temp % 10;
		TimeBCD.SpecificTime.month = ((Tens << 4) & 0xF0) | (units & 0x0F);
			M = TimeBCD.SpecificTime.month;
		break;
		case 5:
		Temp = atoi(cmd_tmp) - 2000;		//
		Tens = Temp / 10;
		units = Temp % 10;
		TimeBCD.SpecificTime.year = ((Tens << 4) & 0xF0) | (units & 0x0F);
		Y = TimeBCD.SpecificTime.year;
		break;
		default:
		break;
	}

}

void install_arg(void)//is
{
	unsigned char i = 0;	
	
	switch(flag_function){
		case 0:
			UARTprintf("Erase All Data Log\r\n");
		  for (i=0;i<30;i++)
      M25P16_erase_map(i*0x10000,SE);

		  Intermediate_Data.M25P16_Data_Addr = 0;
      e2prom512_write((unsigned char *)&Intermediate_Data.M25P16_Data_Addr,sizeof(unsigned int),116*2);
			UARTprintf("...wait...\r\n");
			flag_function++;
			flag_chaoshi = 0;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 1:
			UARTprintf("System time is 20%x-%x-%x %x:%x:%x Change (Y/N)?",
			run_parameter.reserved_parameter35&0xFF,run_parameter.reserved_parameter341>>8,
			run_parameter.reserved_parameter341&0xFF,run_parameter.reserved_parameter36&0xFF,
			run_parameter.reserved_parameter37>>8,run_parameter.reserved_parameter37&0xFF);
			flag_function++;
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
		
		case 2:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function = 4;
					break;
					case 0x6e://n            
						flag_function=0;
						flag_command=0;
						flag_screen=0;	
						flag_chaoshi=0;
						UARTprintf("\nGive up change the System time, exit IS OK.\r\n\r\n");
						break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input IS command again and set Y or N at here, exit IS OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;	
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a = 0;
		break;
		
		case 3:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;
		break;
		
		case 4:
		UARTprintf("Enter Year: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 5:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input IS command again and set 0-9 at here, exit IS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					is_time_set(5);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 6:
		UARTprintf("Enter Month: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;	
		break;
		
		case 7:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input IS command again and set 0-9 at here, exit IS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}		
				}
				if(flag_done==0)
				{
					is_time_set(4);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;	
		break;
		
		case 8:
		UARTprintf("Enter Day: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 9:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input IS command again and set 0-9 at here, exit IS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					is_time_set(3);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 10:
		UARTprintf("Enter Hour: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;					
		break;
		
		case 11:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input IS command again and set 0-9 at here, exit IS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}		
				}
				if(flag_done==0)
				{
					is_time_set(2);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 12:
		UARTprintf("Enter Minute: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;			
		break;
		
		case 13:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input IS command again and set 0-9 at here, exit IS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					is_time_set(1);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 14:
		UARTprintf("Enter Second: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 15:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input IS command again and set 0-9 at here, exit IS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}		
				}
				if(flag_done==0)
				{
					is_time_set(0);
					flag_function=3;
//					UARTprintf("System time is 20%x-%x-%x %x:%x:%x",
//					run_parameter.reserved_parameter35&0xFF,run_parameter.reserved_parameter341>>8,
//					run_parameter.reserved_parameter341&0xFF,run_parameter.reserved_parameter36&0xFF,
//					run_parameter.reserved_parameter37>>8,run_parameter.reserved_parameter37&0xFF);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		default:
		flag_command=0;
		break;						
	}		
}

void date_arg(void)//rs
{
  unsigned char i = 0;	
	switch(flag_function){
		case 0:
		UARTprintf("System time is 20%x-%x-%x %x:%x:%x Change (Y/N)?",
		run_parameter.reserved_parameter35&0xFF,run_parameter.reserved_parameter341>>8,
		run_parameter.reserved_parameter341&0xFF,run_parameter.reserved_parameter36&0xFF,
		run_parameter.reserved_parameter37>>8,run_parameter.reserved_parameter37&0xFF);
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function = 3;
					break;
					case 0x6e://n            
						flag_function=0;
						flag_command=0;
						flag_screen=0;	
						flag_chaoshi=0;
						UARTprintf("\nGive up change the System time, exit RS OK.\r\n\r\n");
						break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input RS command again and set Y or N at here, exit RS OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;	
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;	
		
		case 2:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;
		break;
		
		case 3:
		UARTprintf("Enter Year: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 4:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input RS command again and set 0-9 at here, exit RS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}		
				}
				if(flag_done==0)
				{
					is_time_set(5);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 5:
		UARTprintf("Enter Month: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;	
		break;
		
		case 6:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input RS command again and set 0-9 at here, exit RS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}
				}
				if(flag_done==0)
				{
					is_time_set(4);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;	
		break;
		
		case 7:
		UARTprintf("Enter Day: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 8:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input RS command again and set 0-9 at here, exit RS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}		
				}
				if(flag_done==0)
				{
					is_time_set(3);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 9:
		UARTprintf("Enter Hour: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;					
		break;
		
		case 10:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input RS command again and set 0-9 at here, exit RS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}	
				}
				if(flag_done==0)
				{
					is_time_set(2);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 11:
		UARTprintf("Enter Minute: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;			
		break;
		
		case 12:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input RS command again and set 0-9 at here, exit RS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}
				}
				if(flag_done==0)
				{
					is_time_set(1);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 13:
		UARTprintf("Enter Second: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 14:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input RS command again and set 0-9 at here, exit RS OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}
				}
				if(flag_done==0)
				{
					is_time_set(0);
					flag_function=2;
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		default:
		flag_command=0;
		break;						
	}		
}

void record_arg(void)//t
{
	unsigned short i = 0;
  unsigned char buffer[256] = {0};
	unsigned char b = 0; 
	switch(flag_function){
		case 0:
		UARTprintf("Trace Functions:\r\n\
		c = clear log\r\n\
		d = display log\r\n\
		e = exit\r\n\
		Select function: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x63://clear log
					flag_function++;
					break;
					case 0x64://display log    
					flag_function=5;						
					break;
					case 0x65://e
					UARTprintf("Exit...\n");
					flag_function=4;
					break;
					default:
						UARTprintf("not a illegal interger, set C/D or E\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
						UARTprintf("Please input T command again and set C/D or E at here, exit T OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
						}
						break;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 2:
		UARTprintf("Erase the Data Log (Y/N)?");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 3:  
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
					flag_function++;
					M25P16_erase_map(31*0x10000,SE);
					UARTprintf("All clear\n");
					break;
					case 0x6e://n            
					flag_function++;	
					UARTprintf("Give up Erase the Data Log, exit\n");
					break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input T command again and set Y or N at here, exit T OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					  break;				
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;			
		break;
		
		case 4:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;				
		break;
		
		case 5:  //d�����
		UARTprintf("Enter Number of entries to show (max.256 alarm log): ");
		flag_function++;
		flag_chaoshi = 0;	
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;	
		break;
		
		case 6:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input T command again and set 0-9 at here, exit T OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}		
				}
				if(flag_done==0)
				{
					readlog_number=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;			
		break;
		
		case 7:
		UARTprintf("Begin Log Data - RS232\r\n\r\n");
		UARTprintf("TimeStamp    ");
		UARTprintf("H2DG    ");
		UARTprintf("OilTemp    ");
		UARTprintf("DayROCppm    ");
		UARTprintf("Msg    \r\n");	
		//�Ӵ洢�д�ӡ����
		e2prom512_read(&b,sizeof(unsigned char),233*2);
		Intermediate_Data.Alarm_page = b;
//		UARTprintf("Alarm_page=%d\n",Intermediate_Data.Alarm_page);
		if (readlog_number <= 256 && readlog_number > 0){
			for (i=0;i<readlog_number;i++){
				Intermediate_Data.Alarm_page--;
				if (Intermediate_Data.Alarm_page > 255)
					Intermediate_Data.Alarm_page = 255;
				
				  M25P16_Read_Data(buffer,256,31*0x10000+Intermediate_Data.Alarm_page*256);
				
				UARTprintf("%x-%x-%x %x:%x:%x %x %.2f %x \n",(buffer[0]<<8)|buffer[1],buffer[2],buffer[3],buffer[4],
				buffer[5],buffer[6],(buffer[7]<<24)|(buffer[8]<<16)|(buffer[9]<<8)|buffer[10],(float)((buffer[11]<<8)|buffer[12])/100.0,(buffer[13]<<24)|(buffer[14]<<16)|(buffer[15]<<8)|buffer[16]);
			}
		}else{
		  UARTprintf("max.256 alarm log. Please set 1-256 at here.\n");
		}
		UARTprintf("\r\nEnd Log Data - RS232\n");	
		flag_function=4;
		readlog_number = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;	
		break;
		
		default:
		flag_command=0;
		break;
	}
}

void clear_arg(void)//x
{
	switch(flag_function){
		case 0:
		UARTprintf("Clear field calibration values (Y/N)?");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y
					//���������У׼���ݺͲ�Ʒ���ã���ȡ��������
					run_parameter.h2_ppm_calibration_gas_h16.hilo = 0;
					run_parameter.h2_ppm_calibration_gas_l16.hilo = 0;
					e2prom512_write(&run_parameter.h2_ppm_calibration_gas_h16.ubit.lo,4,126*2);

					run_parameter.unit_id.ubit.lo = 1;
					
					run_parameter.h2_ppm_report_low_h16.hilo = 0;
					run_parameter.h2_ppm_report_low_l16.hilo = 0;
					run_parameter.h2_ppm_report_high_h16.hilo = 0;
					run_parameter.h2_ppm_report_high_l16.hilo = 5000;

					run_parameter.h2_ppm_alarm_low_h16.hilo = 0;
					run_parameter.h2_ppm_alarm_low_l16.hilo = 2500; //DayDRC

					run_parameter.h2_ppm_alert_low_h16.hilo = 0;
					run_parameter.h2_ppm_alert_low_l16.hilo = 2500;

					run_parameter.OilTemp_Alarm_celsius.hilo = 70;

					run_parameter.h2_ppm_out_current_low.hilo = 400;
					run_parameter.h2_ppm_out_current_high.hilo = 2000;
					run_parameter.h2_ppm_error_out_current.hilo = 300;
					run_parameter.h2_ppm_no_ready_out_current.hilo = 200;
					
					e2prom512_write(&run_parameter.unit_id.ubit.lo,2,150*2);
					e2prom512_write(&run_parameter.h2_ppm_report_low_h16.ubit.lo,4,141*2);
					e2prom512_write(&run_parameter.h2_ppm_report_high_h16.ubit.lo,4,143*2);
					
					e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);
					
					e2prom512_write(&run_parameter.h2_ppm_alert_low_h16.ubit.lo,4,152*2);
					e2prom512_write(&run_parameter.h2_ppm_alarm_low_h16.ubit.lo,4,154*2);
					e2prom512_write(&run_parameter.OilTemp_Alarm_celsius.ubit.lo,2,156*2);
					
					strcpy(run_parameter.own_id.own_id_str,"user 01");
					e2prom512_write(&run_parameter.own_id.own_id_str,sizeof(run_parameter.own_id.own_id_str),201*2);
					//211-220
					strcpy(run_parameter.sub_station_id.sub_station_id_str,"Sub-Station 01");
					e2prom512_write(&run_parameter.sub_station_id.sub_station_id_str,sizeof(run_parameter.own_id.own_id_str),211*2);
					//221-230
					strcpy(run_parameter.transformer_id.transformer_id_str,"Transformer 01");
					e2prom512_write(&run_parameter.transformer_id.transformer_id_str,sizeof(run_parameter.own_id.own_id_str),221*2);
	
					UARTprintf("Returns to last factory calibration data\n");
					UARTprintf("Done - Wait......\n");					
					flag_function++;
					break;
					case 0x6e://n 
						UARTprintf("No change - Done\n");
						flag_function++;						
					break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input X command again and set Y or N at here, exit X OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 2:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;		
		break;
		
		default:
		flag_command=0;
		break;
	}
}

void ci_arg(void)//ci
{
	unsigned char i = 0;
	switch(flag_function){
		case 0:
		UARTprintf("Calibrate 4-20mA output (Y/N)?");	
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y
					flag_function=3;
					break;
					case 0x6e://n 
						UARTprintf("No change - Done\n");
						flag_function++;						
					break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input CI command again and set Y or N at here, exit CI OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 2:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;			
		break;
		
		case 3:
		UARTprintf("Set to 4.000mA, Enter actual value: ");	
		flag_function++;
		flag_chaoshi=0;	
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 4:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input CI command again and set 0-9 at here, exit CI OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}			
				}
				if(flag_done==0)
				{
//					run_parameter.h2_ppm_out_current_low.hilo=(short)(atof(cmd_tmp)*100);//��parameter�д��޸�
					run_parameter.h2_ppm_out_current_low.hilo = atof(cmd_tmp)*100;
					UARTprintf("run_parameter.h2_ppm_out_current_low.hilo=%d\n",run_parameter.h2_ppm_out_current_low.hilo);
					e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,2,145*2);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;	
		break;
		
		case 5:
		UARTprintf("Set to 20.000mA, Enter actual value: ");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;				
		break;
		
		case 6:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input CI command again and set 0-9 at here, exit CI OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}			
				}
				if(flag_done==0)
				{
//					run_parameter.h2_ppm_out_current_high.hilo=(short)(atof(cmd_tmp)*100);//��parameter�д��޸�
					run_parameter.h2_ppm_out_current_high.hilo = atof(cmd_tmp)*100;
					UARTprintf("run_parameter.h2_ppm_out_current_high.hilo=%d\n",run_parameter.h2_ppm_out_current_high.hilo);
					e2prom512_write(&run_parameter.h2_ppm_out_current_high.ubit.lo,2,146*2);
					UARTprintf("Test the output\n");
					flag_function++;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;			
		break;
		
		case 7:
		UARTprintf("Set to 4.000mA, Is this good (Y/N)?");
		flag_function++;
		flag_chaoshi=0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;				
		break;
		
		case 8:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y
					flag_function++;
					break;
					case 0x6e://n 
					flag_function=3;						
					break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input CI command again and set Y or N at here, exit CI OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;	
		break;
		
		case 9:
		UARTprintf("Set to 20.000mA, Is this good (Y/N)?");
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;			
		break;
		
		case 10:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y
					UARTprintf("Calibrate %.2f-%.2fmA output\n",(float)run_parameter.h2_ppm_out_current_low.hilo/100,(float)run_parameter.h2_ppm_out_current_high.hilo/100);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					flag_function=2;
					break;
					case 0x6e://n 
					flag_function=3;						
					break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input CI command again and set Y or N at here, exit CI OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		default:
		flag_command=0;
		break;
	}
}

void setmid_arg(void)//mi
{
	unsigned char i = 0;	
	switch(flag_function){
		case 0:
		UARTprintf("Modbus ID is %d Change (Y/N)?",run_parameter.unit_id.ubit.lo);	
		flag_function++;
		flag_chaoshi = 0;
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;
		break;
		
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y
					UARTprintf("Set Modbus ID to: ");		
					flag_function=3;
					break;
					case 0x6e://n 
					UARTprintf("No change - Done\n");
					flag_function++;						
					break;
					default:
						UARTprintf("not a illegal interger, set Y or N\n");
						flag_chaoshi++;
						if (flag_chaoshi > 5){
							UARTprintf("Please input MI command again and set Y or N at here, exit MI OK.\n");
							flag_function = 0;
							flag_command = 0;
							flag_screen = 0;
							flag_chaoshi = 0;
						}
					break;				
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		case 2:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;		
		break;
		
		case 3:
		if(strlen(cmd_tmp)>0)
		{
				flag_done = 0;
				if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
				{
					flag_done = 1;
					UARTprintf("Please set 0-9 at here, try again.\n");
					flag_chaoshi++;
					if (flag_chaoshi > 5){
						UARTprintf("Please input CI command again and set 0-9 at here, exit CI OK.\n");
						flag_function = 0;
						flag_command = 0;
						flag_screen = 0;
						flag_chaoshi = 0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.unit_id.ubit.lo = atoi(cmd_tmp);
					e2prom512_write(&run_parameter.unit_id.ubit.lo,2,150*2);
					UARTprintf("New Modbus ID is %d\r\n",run_parameter.unit_id.ubit.lo);
					UARTprintf("\n...SAVED  Done......\r\n\r\n");
					flag_function = 2;
				}
		}
		memset(cmd_tmp,0,sizeof(cmd_tmp));
		a=0;		
		break;
		
		default:
		flag_command=0;
		break;
	}
}

/***********************************************************
Function: configuration Mode ,temperature and screen.
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description:  .
***********************************************************/
void cf_arg(void)
{
	unsigned char i = 0;
	switch(flag_function)
	{
		case 0:
			UARTprintf("0 - Exit cf\n1 - Normal\n2 - Debug\n3 - Calibration\n4 - set temperature\n5 - turn off the screen\n6 - turn on the screen\nSelect function:\n");
			flag_function++;
			break;
		
		case 1:
			if(strlen(cmd_tmp)>0)
			{
				switch(cmd_tmp[0]){
					case 0x30://0
						UARTprintf("Exit cf Success, exit cf OK.\n");	
						flag_screen = 0;					
						flag_function++;
					break;
					case 0x31://n 
						UARTprintf("change mode to normal Success, exit cf OK.\n");
						output_data.MODEL_TYPE = 1;
				    UARTprintf(print_menu);
					  flag_screen = 0;
						flag_function++;						
					break;
					case 0x32:
						UARTprintf("change mode to debug Success, exit cf OK.\n");
						output_data.MODEL_TYPE = 2;
				    UARTprintf(debug_menu);
					  flag_screen = 0;
						flag_function++;
					break;
					case 0x33:
						UARTprintf("change mode to calibration Success, exit cf OK.\n");
						output_data.MODEL_TYPE = 3;
						UARTprintf(calibrate_menu);
						output_data.temperature = 50;
						DAC8568_INIT_SET(output_data.temperature,2*65536/5);	/* Set Senseor temperature :DOUT-C = xV*65536/5 */
		        Intermediate_Data.Start_print_calibrate_H2R = 1;
					  flag_screen = 0;
						flag_function++;
					break;
					case 0x34:
						UARTprintf("Please input the temperature you want:\n");
						flag_function = 3;
					break;
					case 0x35:
						UARTprintf("\nturn off the screen Success, exit cf OK.\r\n\r\n");
						flag_screen = 1;
						flag_function++;
					break;
					case 0x36:
						UARTprintf("\nturn on the screen Success, exit cf OK.\r\n\r\n");
						flag_screen = 0;
						flag_function++;
						break;
					default:
						UARTprintf("Please input cf command again and set 0-6 at here, exit cf OK.\n");
						flag_screen = 0;
						flag_function = 0;
						flag_command = 0;
						break;				
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;
			break;
			
		case 2:
			flag_function = 0;
			flag_command = 0;	
			break;
		
		case 3:
			if(strlen(cmd_tmp)>0)
			{
				flag_done = 0;
				for(i=0;i<a;i++)
				{
					if((cmd_tmp[i] < 0x30) || (cmd_tmp[i] > 0x39))
					{
						flag_done = 1;
						UARTprintf("Please input cf command again and set 0-9 at here, exit cf OK.\n");
						flag_screen = 0;
						flag_function = 2;
					}
				}
				if(flag_done == 0)
				{
					output_data.temperature = atoi(cmd_tmp);
					UARTprintf("now heat temperature is %d\r\n",output_data.temperature);
					UARTprintf("\nSAVED  heat temperature Success, exit cf OK.\r\n\r\n");
					DAC8568_INIT_SET(output_data.temperature,2*65536/5);
					flag_screen = 0;
					flag_function = 2;
				}
			}
			memset(cmd_tmp,0,sizeof(cmd_tmp));
			a = 0;		
			break;
		
		default:
			flag_function = 0;
			flag_command = 0;
		  flag_screen = 0;
			break;
	}
}
	
	





