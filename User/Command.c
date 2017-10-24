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

unsigned char cmd_tmp[CMD_LEN];
unsigned char cmd_buf[CMD_LEN];
unsigned char flag_command;//flag_command的值对应相应命令行执行函数
unsigned char flag_function;//flag_function的值对应每个命令行函数执行到哪一步
unsigned char flag_screen;//flag_screen 0 开启回显 1 关闭回显
unsigned char flag_relay1;//relay 1 status flag
unsigned char flag_relay2;//relay 2 status flag
unsigned char flag_relay3;//relay 3 status flag
unsigned char flag_chaoshi;//防止一直卡在某命令的某个步骤中，超过一定大小就使步骤归0
unsigned char flag_done;//命令行输入设置数字，如果都是数字为0，不是为1，提示需要重新输入
unsigned char flag_relay_done;//判断设置的是哪个relay的功能，relay设置限制跳转标志位
unsigned char a;//cmd_tmp的长度
unsigned int readlog_number;//读取报警信息数量
//*****************************************************************************/
//cmd catalogue
//*****************************************************************************/  
cmd_list_struct  cmd_list[]={  
/*命令  参数目录    处理函数        报警信息*/	
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
			//flag 置位跳出命令行设置
			break;
			default:
			{
				if(isprint(src[src_count]))//去除不可打印字符
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
	unsigned char i;
	for(i=0;i<sizeof(cmd_list)/sizeof(cmd_list[0]);i++)
	{
		if(strcmp(cmd_list[i].cmd_file,cmd_tmp)==0)
	 {
		//相应处理函数标志位置位
		flag_command=cmd_list[i].max_args; 
		//UARTprintf("flag_command=%d\n",flag_command);
		return 1;
	 }
	}
  //memset(cmd_tmp,0,strlen(cmd_tmp));
  //a=0;	
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
void alarm_arg(void)//还需增加将继电器状态值存入E2P中
{
	unsigned char i;
	switch(flag_function){
		case 0:
		UARTprintf("Relay #1 Select mode:\n\
		0 - Disable\n\
		1 - Hydrogen level\n\
		2 - Rate of Change\n\
		3 - OilTemp level\n\
		Select function: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
			break;
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x30:
						UARTprintf("Relay #1 disable\n");
					  flag_relay1=0;
						flag_function++;
					break;
					case 0x31:
						UARTprintf("Enter Trigger (ppm H2):");
					  flag_relay1=1;
						flag_function=7;						
					break;
					case 0x32:
						UARTprintf("Enter Trigger (ppm H2/day):");
					  flag_relay1=2;
						flag_function=8;							
					break;
					case 0x33:
						UARTprintf("Enter Trigger (Oil temperature):");
					  flag_relay1=3;
						flag_function=9;							
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		flag_relay_done=1;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
			break;
				case 2:
		UARTprintf("Relay #2 Select mode:\n\
		0 - Disable\n\
		1 - Hydrogen level\n\
		2 - Rate of Change\n\
		3 - OilTemp level\n\
		Select function: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
			break;
		case 3:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x30:
						UARTprintf("Relay #2 disable\n");
					  flag_relay2=0;
						flag_function++;
					break;
					case 0x31:
						UARTprintf("2 Enter Trigger (ppm H2):");
						flag_relay2=1;
						flag_function=7;						
					break;
					case 0x32:
						UARTprintf("2 Enter Trigger (ppm H2/day):");
						flag_relay2=2;
					  flag_function=8;							
					break;
					case 0x33:
						UARTprintf("2 Enter Trigger (Oil temperature):");
					  flag_relay2=3;
						flag_function=9;							
					break;
					default:
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;	
					break;				
				}
		}
		flag_relay_done=3;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
			break;
				case 4:
		UARTprintf("Relay #3 Select mode:\n\
		0 - Disable\n\
		1 - Hydrogen level\n\
		2 - Rate of Change\n\
		3 - OilTemp level\n\
		Select function: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
			break;
		case 5:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x30:
						UARTprintf("Relay #3 disable\n");
						flag_relay3=0;
					  flag_function++;
					break;
					case 0x31:
						UARTprintf("3 Enter Trigger (ppm H2/day):");
						flag_relay3=1;
					  flag_function=7;						
					break;
					case 0x32:
						UARTprintf("3 Enter Trigger (ppm H2/day):");
						flag_relay3=2;
					  flag_function=8;							
					break;
					case 0x33:
						UARTprintf("3 Enter Trigger (Oil temperature):");
						flag_relay3=3;
					  flag_function=9;							
					break;
					default:
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
					break;				
				}
		}
		flag_relay_done=5;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
			break;
		case 6:
			UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
			flag_function=0;
		  flag_command=0;
		  flag_screen=0;
			flag_chaoshi=0;
		case 7:
		  if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_alarm_low_l16.hilo=atoi(cmd_tmp);
					UARTprintf("relay h2 ppm low %d\n",run_parameter.h2_ppm_alarm_low_l16.hilo);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					if(flag_relay_done==1)
					{
					  flag_function=2;
					}
					if(flag_relay_done==3)
					{
					  flag_function=4;
					}
					if(flag_relay_done==5)
					{
					  flag_function=6;
					}
				}
			}
			memset(cmd_tmp,0,strlen(cmd_tmp));
			a=0;					 
		break;
		case 8:
		  if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						break;				
					}
					else
					{
						flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_alarm_DRC.hilo=atoi(cmd_tmp);
					UARTprintf("rate of change  %d\n",run_parameter.h2_ppm_alarm_DRC.hilo);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					if(flag_relay_done==1)
					{
					  flag_function=2;
					}
					if(flag_relay_done==3)
					{
					  flag_function=4;
					}
					if(flag_relay_done==5)
					{
					  flag_function=6;
					}
				}
			}
			memset(cmd_tmp,0,strlen(cmd_tmp));
			a=0;	
		break;
		case 9:
			if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						break;				
					}
					else
					{
						flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.OilTemp_Alarm_celsius.hilo=atoi(cmd_tmp);
					UARTprintf("oil temperature  %d\n",run_parameter.OilTemp_Alarm_celsius.hilo);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					if(flag_relay_done==1)
					{
					  flag_function=2;
					}
					if(flag_relay_done==3)
					{
					  flag_function=4;
					}
					if(flag_relay_done==5)
					{
					  flag_function=6;
					}
				}
			}
			memset(cmd_tmp,0,strlen(cmd_tmp));
			a=0;				
		break;
		default:
		flag_command=0;
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
void config_arg_d0(void)//还需增加从E2P读取这些参数
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
    flag_chaoshi++;	
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 1:
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
    Hydrogen reporting range(in oil LowH2Range-HighH2Range): %0.4f - %0.4f%% H2\n",
		(run_parameter.h2_ppm_report_low_l16.hilo/20.F),(run_parameter.h2_ppm_report_high_l16.hilo/20.F));
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
		Not-Ready output is %0.1fmA\n",run_parameter.h2_ppm_out_current_low.hilo,
		run_parameter.h2_ppm_out_current_high.hilo,
		run_parameter.h2_ppm_error_out_current.hilo,
		run_parameter.h2_ppm_no_ready_out_current.hilo);	
		#endif
		if(flag_relay1==0){UARTprintf("Relays#1:disable\n");}else{UARTprintf("Relays#1:enable\n");}
		if(flag_relay2==0){UARTprintf("Relays#2:disable\n");}else{UARTprintf("Relays#2:enable\n");}
		if(flag_relay3==0){UARTprintf("Relays#3:disable\n");}else{UARTprintf("Relays#3:enable\n");}
		
		if(flag_relay1==1)
		{
			UARTprintf("(relays#1)");
			UARTprintf(" threshold is %u ppm Hydrogen\n",
			(unsigned int)(run_parameter.h2_ppm_alarm_low_l16.hilo*10000.F));
		}
		if(flag_relay2==1)
		{
			UARTprintf("(relays#2)");
			UARTprintf(" threshold is %u ppm Hydrogen\n",
			(unsigned int)(run_parameter.h2_ppm_alarm_low_l16.hilo*10000.F));
		}
		if(flag_relay3==1)
		{
			UARTprintf("(relays#3)");
			UARTprintf(" threshold is %u ppm Hydrogen\n",
			(unsigned int)(run_parameter.h2_ppm_alarm_low_l16.hilo*10000.F));
		}
		if(flag_relay1==2)
		{
			UARTprintf("(relays#1)");
			UARTprintf(" threshold is %u ppm Hydrogen/Day\n",
			(unsigned int)(run_parameter.h2_ppm_alarm_DRC.hilo*10000.F));
		}
		if(flag_relay2==2)
		{
			UARTprintf("(relays#2)");
			UARTprintf(" threshold is %u ppm Hydrogen/Day\n",
			(unsigned int)(run_parameter.h2_ppm_alarm_DRC.hilo*10000.F));
		}
		if(flag_relay3==2)
		{
			UARTprintf("(relays#3)");
			UARTprintf(" threshold is %u ppm Hydrogen/Day\n",
			(unsigned int)(run_parameter.h2_ppm_alarm_DRC.hilo*10000.F));
		}
		if(flag_relay1==3)
		{
			UARTprintf("(relays#1)");
			UARTprintf(" threshold is %u degrees Celsius\n",
			(unsigned int)(run_parameter.OilTemp_Alarm_celsius.hilo));
		}
		if(flag_relay2==3)
		{
			UARTprintf("(relays#2)");
			UARTprintf(" threshold is %u degrees Celsius\n",
			(unsigned int)(run_parameter.OilTemp_Alarm_celsius.hilo));
		}
		if(flag_relay3==3)
		{
			UARTprintf("(relays#3)");
			UARTprintf(" threshold is %u degrees Celsius\n",
			(unsigned int)(run_parameter.OilTemp_Alarm_celsius.hilo));
		}
		flag_function++;
		flag_chaoshi++;	
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		UARTprintf("\
		Pressure compensation is disabled\n\
		Gas Pressure is %0.1f atm\n",1.0); 	
		case 1:
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
void config_arg_d2(void)
{
		switch(flag_function){
		case 0:
		UARTprintf("Manufacturing information is:\n\
		Sensor Serial Number: %s\n\
		Sensor Board Serial Number: %s\n\
		Interface Board Serial Number: %s\n\
		Date Built: 20161206\n",run_parameter.sensor_serial_number.sensor_serial_number_str,
		run_parameter.sensor_board_serial_number.sensor_board_serial_number_str,
		run_parameter.interface_board_serial_number.interface_board_serial_number_str); 	
    flag_function++;
    flag_chaoshi++;	
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 1:
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
void config_arg_d3(void)
{
		switch(flag_function){
		case 0:						 
		UARTprintf("Product Configuration:\n\
		Sample rate is %u:%u:%u/sample (times/sample)\n",
		run_parameter.realtime.hour,run_parameter.realtime.minute,run_parameter.realtime.second);	
		flag_function++;
		flag_chaoshi++;	
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 1:
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
		switch(flag_function){
		case 0:						 
    UARTprintf("Current H2 value is %u ppm H2\n",run_parameter.h2_ppm_l16.hilo/20);
		//存储数据到FLASH中
		flag_function++;
		flag_chaoshi++;	
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 1:
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
	unsigned char i;	
	switch(flag_function){
		case 0:
		UARTprintf("Enter actual hydrogen in ppm: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 1:
			if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_l16.hilo=atoi(cmd_tmp);
					flag_function++;
				}
			}
			memset(cmd_tmp,0,strlen(cmd_tmp));
			a=0;
		break;
		case 2:
		UARTprintf("Set hydrogen to %d ppm (Y/N)?",run_parameter.h2_ppm_l16.hilo);
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;	
		case 3:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function++;
					break;
					case 0x6e://n            
						flag_function=0;						
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 4:
		UARTprintf("Calibration Gas finished\n");	
		UARTprintf("Cal. date is: %d-%d-%d (Y/N)?",
		run_parameter.field_calibration_date.year,
		run_parameter.field_calibration_date.month,
		run_parameter.field_calibration_date.day); 
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
						flag_function=8;						
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 6:
		UARTprintf("Cal Message:......\n");    
    UARTprintf("Cal. date : %d-%d-%d\r\n",run_parameter.field_calibration_date.year,
		run_parameter.field_calibration_date.month,
    run_parameter.field_calibration_date.day);
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 7:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;
		break;
		case 8:
		UARTprintf("Enter Year: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 9:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.field_calibration_date.year=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 10:
		UARTprintf("Enter Month: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;					
		break;
		case 11:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.field_calibration_date.month=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 12:
		UARTprintf("Enter Day: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 13:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.field_calibration_date.day=atoi(cmd_tmp);
					flag_function=7;
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		default:
		flag_command=0;
		break;						
	}
}

void dx_arg(void)
{
	unsigned char i;	
	switch(flag_function){
		case 0:
		UARTprintf("Enter actual hydrogen in ppm: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 1:
			if(strlen(cmd_tmp)>0)
			{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_l16.hilo=atoi(cmd_tmp);
					flag_function++;
				}
			}
			memset(cmd_tmp,0,strlen(cmd_tmp));
			a=0;
		break;
		case 2:
		UARTprintf("Set hydrogen to %d ppm (Y/N)?",run_parameter.h2_ppm_l16.hilo);
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;	
		case 3:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function++;
					break;
					case 0x6e://n            
						flag_function=0;						
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 4:
		UARTprintf("Calibration Gas finished\n");	
		UARTprintf("Cal. date is: %d-%d-%d (Y/N)?",
		run_parameter.field_calibration_date.year,
		run_parameter.field_calibration_date.month,
		run_parameter.field_calibration_date.day); 
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
						flag_function=8;						
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 6:
		UARTprintf("Cal Message:......\n");    
    UARTprintf("Cal. date : %d-%d-%d\r\n",run_parameter.field_calibration_date.year,
		run_parameter.field_calibration_date.month,
    run_parameter.field_calibration_date.day);
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 7:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;
		break;
		case 8:
		UARTprintf("Enter Year: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 9:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.field_calibration_date.year=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 10:
		UARTprintf("Enter Month: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;					
		break;
		case 11:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.field_calibration_date.month=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 12:
		UARTprintf("Enter Day: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 13:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.field_calibration_date.day=atoi(cmd_tmp);
					flag_function=7;
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		default:
		flag_command=0;
		break;						
	}	
}
void gg_arg(void)
{
	switch(flag_function){
		case 0:
		UARTprintf("TimeStamp            ");
		UARTprintf("PcbTemp  ");   
		UARTprintf("H2AG.ppm   ");
		UARTprintf("OilTemp  ");		
		UARTprintf("H2DG.ppm   ");
		UARTprintf("H2G.ppm   ");
		UARTprintf("H2SldAv  ");
		UARTprintf("DayROC   ");
		UARTprintf("WeekROC  ");
		UARTprintf("MonthROC ");
		UARTprintf("SnsrTemp ");
		UARTprintf("H2Resistor ");
		UARTprintf("STResistor ");
		UARTprintf("MonthROC ");
		UARTprintf("\r\n");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 1:
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
void aop_arg(void)//h
{
	unsigned char i;
	switch(flag_function){
		case 0:
		UARTprintf("Hydrogen reporting range(in oil LowH2Range-HighH2Range): %0.4f - %0.4f%% H2\n",
		(run_parameter.h2_ppm_report_low_l16.hilo/20.F),(run_parameter.h2_ppm_report_high_l16.hilo/20.F));
    UARTprintf("Change (Y/N)?");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
						flag_function++;
						UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 2:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;
		break;
		case 3:
		UARTprintf("Enter new LowH2Range: ");
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 4:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_report_low_l16.hilo=atof(cmd_tmp)*20;
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 5:
		UARTprintf("Enter new HighH2Range: ");
		flag_function++;		
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 6:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_report_high_l16.hilo=atof(cmd_tmp)*20;
					UARTprintf("New hydrogen reporting range(in oil LowH2Range-HighH2Range): %0.4f - %0.4f%% H2\n",
					(run_parameter.h2_ppm_report_low_l16.hilo/20.F),(run_parameter.h2_ppm_report_high_l16.hilo/20.F));		
					UARTprintf("...Wait...SAVED - Done\n");
					flag_function=2;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		default:
		flag_command=0;
		break;
	}

}
void aoerr_arg(void)//i
{
	unsigned char i;
	switch(flag_function){
		case 0:
    UARTprintf("DAC range is %0.2fmA to %0.2fmA(LowH2Current-HighH2Current), error output is %0.2fmA, not ready output is %0.2fmA\n",
		run_parameter.h2_ppm_out_current_low.hilo,
		run_parameter.h2_ppm_out_current_high.hilo,
		run_parameter.h2_ppm_error_out_current.hilo,
		run_parameter.h2_ppm_no_ready_out_current.hilo);		
    UARTprintf("Change (Y/N)?");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
						flag_function++;
						UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 2:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;
		break;
		case 3:
		UARTprintf("Enter new low H2 output current: ");
		flag_function++;		
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 4:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_out_current_low.hilo=atof(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 5:
		UARTprintf("Enter new high H2 output current: ");
		flag_function++;		
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 6:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_out_current_high.hilo=atof(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 7:
		UARTprintf("Enter new error H2 output current: ");
		flag_function++;		
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 8:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_error_out_current.hilo=atof(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 9:
		UARTprintf("Enter new not ready H2 output current: ");
		flag_function++;		
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 10:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.h2_ppm_no_ready_out_current.hilo=atof(cmd_tmp);
					flag_function=2;
					UARTprintf("New DAC range is %0.2fmA to %0.2fmA(LowH2Current-HighH2Current), error output is %0.2fmA, not ready output is %0.2fmA\n",
					run_parameter.h2_ppm_out_current_low.hilo,
					run_parameter.h2_ppm_out_current_high.hilo,
					run_parameter.h2_ppm_error_out_current.hilo,
					run_parameter.h2_ppm_no_ready_out_current.hilo);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		default:
		flag_command=0;
		break;
	}
}
void install_arg(void)//is
{
	unsigned char i;	
	switch(flag_function){
		case 0:
		UARTprintf("Erase All Data Log\r\n");	
		UARTprintf("...wait...\r\n");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 1:
    UARTprintf("System time is 20%d-%d-%d %d:%d:%d Change (Y/N)?",
		run_parameter.realtime.year,run_parameter.realtime.month,
    run_parameter.realtime.day,run_parameter.realtime.hour,
    run_parameter.realtime.minute,run_parameter.realtime.second);
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 2:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
						flag_function=4;
					break;
					case 0x6e://n            
						flag_function++;						
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
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
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 5:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.year=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 6:
		UARTprintf("Enter Month: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;	
		break;
		case 7:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.month=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;	
		break;
		case 8:
		UARTprintf("Enter Day: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 9:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.day=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 10:
		UARTprintf("Enter Hour: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;					
		break;
		case 11:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.hour=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 12:
		UARTprintf("Enter Minute: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 13:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.minute=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 14:
		UARTprintf("Enter Second: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 15:
				if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.second=atoi(cmd_tmp);
					flag_function=3;
					UARTprintf("Change time to 20%d-%d-%d %d:%d:%d ",
					run_parameter.realtime.year,run_parameter.realtime.month,
					run_parameter.realtime.day,run_parameter.realtime.hour,
					run_parameter.realtime.minute,run_parameter.realtime.second);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		default:
		flag_command=0;
		break;						
	}		
}
void date_arg(void)//rs
{
unsigned char i;	
	switch(flag_function){
		case 0:
    UARTprintf("System time is 20%d-%d-%d %d:%d:%d Change (Y/N)?",
		run_parameter.realtime.year,run_parameter.realtime.month,
    run_parameter.realtime.day,run_parameter.realtime.hour,
    run_parameter.realtime.minute,run_parameter.realtime.second);
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
					UARTprintf("not a illegal interger\n");
					flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 4:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.year=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 5:
		UARTprintf("Enter Month: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;	
		break;
		case 6:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.month=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;	
		break;
		case 7:
		UARTprintf("Enter Day: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 8:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.day=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 9:
		UARTprintf("Enter Hour: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;					
		break;
		case 10:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.hour=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 11:
		UARTprintf("Enter Minute: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 12:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.minute=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 13:
		UARTprintf("Enter Second: ");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 14:
				if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.realtime.second=atoi(cmd_tmp);
					flag_function=2;
					UARTprintf("Change time to 20%d-%d-%d %d:%d:%d ",
					run_parameter.realtime.year,run_parameter.realtime.month,
					run_parameter.realtime.day,run_parameter.realtime.hour,
					run_parameter.realtime.minute,run_parameter.realtime.second);
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		default:
		flag_command=0;
		break;						
	}		
}
void record_arg(void)//t
{
	unsigned char i;
	switch(flag_function){
		case 0:
		UARTprintf("Trace Functions:\r\n\
		c = clear log\r\n\
		d = display log\r\n\
		e = exit\r\n\
		Select function: ");
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x63://c            
					flag_function++;
					break;
					case 0x64://d            
					flag_function=5;						
					break;
					case 0x65://e
					UARTprintf("Exit...\n");
					flag_function=4;
					break;
					default:
					UARTprintf("not a illegal interger\n");
					flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 2:
		UARTprintf("Erase the Data Log (Y/N)?");
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 3:  
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y             
					flag_function++;
					//delete
					UARTprintf("All clear\n");
					break;
					case 0x6e://n            
					flag_function++;	
					UARTprintf("exit\n");
					break;
					default:
					UARTprintf("not a illegal interger\n");
					flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 4:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;				
		break;
		case 5:  //d命令处理
		UARTprintf("Enter Number of entries to show (max.999 alarm log): ");
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;	
		break;
		case 6:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					readlog_number=atoi(cmd_tmp);
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 7:
    UARTprintf("Begin Log Data - RS232\r\n\r\n");
    UARTprintf("TimeStamp             ");
    UARTprintf("H2DG  ");
    UARTprintf("OilTemp  ");
    UARTprintf("DayROCppm  ");
    UARTprintf("Msg    \r\n");	
		//从存储中打印数据
		UARTprintf("\r\nEnd Log Data - RS232\n");	
		flag_function=4;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;	
		break;
		default:
		flag_command=0;
		break;
	}
}
void clear_arg(void)//x
{
	unsigned char i;	
	switch(flag_function){
		case 0:
		UARTprintf("Clear field calibration values (Y/N)?");
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;
		break;
		case 1:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y
            //清除油中氢校准数据和产品配置
					UARTprintf("Returns to last factory calibration data\n");
					UARTprintf("Done - Wait......\n");					
					flag_function++;
					break;
					case 0x6e://n 
						UARTprintf("No change - Done\n");
						flag_function++;						
					break;
					default:
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
	unsigned char i;
	switch(flag_function){
		case 0:
		UARTprintf("Calibrate 4-20mA output (Y/N)?");	
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 2:
		flag_function=0;
		flag_command=0;
		flag_screen=0;	
		flag_chaoshi=0;			
		break;
		case 3:
		UARTprintf("Set to 3.000mA, Enter actual value: ");	
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		case 4:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.modbus_id=atof(cmd_tmp);//此parameter有待修改
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;	
		break;
		case 5:
		UARTprintf("Set to 19.000mA, Enter actual value: ");
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;				
		break;
		case 6:
		if(strlen(cmd_tmp)>0)
		{
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.modbus_id=atof(cmd_tmp);//此parameter有待修改
					UARTprintf("Test the output\n");
					flag_function++;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 7:
		UARTprintf("Set to 3.000mA, Is this good (Y/N)?");
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;	
		break;
		case 9:
		UARTprintf("Set to 19.000mA, Is this good (Y/N)?");
		flag_function++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;			
		break;
		case 10:
		if(strlen(cmd_tmp)>0)
		{
				switch(cmd_tmp[0]){
					case 0x79://y
					UARTprintf("\n...Wait...SAVED  Done......\r\n\r\n");
					flag_function=2;
					break;
					case 0x6e://n 
					flag_function=3;						
					break;
					default:
					UARTprintf("not a illegal interger\n");
					flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		default:
		flag_command=0;
		break;
	}
}
void setmid_arg(void)//mi
{
	unsigned char i;	
	switch(flag_function){
		case 0:
		UARTprintf("Modbus ID is %d Change (Y/N)?",run_parameter.modbus_id);	
		flag_function++;
		flag_chaoshi++;
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
						UARTprintf("not a illegal interger\n");
					  flag_chaoshi++;
					break;				
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
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
			  for(i=0;i<a;i++)
				{
				  if((cmd_tmp[i]>=0x30)&&(cmd_tmp[i]<=0x39)==0)
					{
	          flag_done=1;
						UARTprintf("not a illegal interger\n");
						flag_chaoshi++;
						break;				
					}
					else
					{
					  flag_done=0;
					}
				}
				if(flag_done==0)
				{
					run_parameter.modbus_id=atoi(cmd_tmp);
					UARTprintf("New Modbus ID is %d\r\n",run_parameter.modbus_id);
					UARTprintf("\n...SAVED  Done......\r\n\r\n");
					flag_function=2;
				}
		}
		memset(cmd_tmp,0,strlen(cmd_tmp));
		a=0;		
		break;
		default:
		flag_command=0;
		break;
	}
}
void cf_arg(void)
{
	UARTprintf("clear_arg");
}
	
	





