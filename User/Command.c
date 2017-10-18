/***********************************************
@		Description: This file is command settings for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: megzheng.
@		Date: 2017/10/16.
***********************************************/
#include <stdio.h>                      /* standard I/O .h-file              */
#include <ctype.h>                      /* character functions               */
#include <LPC21xx.H>                    /* LPC21xx definitions               */
#include <string.h>

#include "main.h"                    /* global project definition file    */
#include "config.h"
#include "Peripherals_LPC2194.h"
#include "Command.h"

unsigned char cmd_tmp[CMD_LEN];
unsigned char cmd_buf[CMD_LEN];
unsigned char flag_command;
unsigned char flag_function;
//*****************************************************************************/
//cmd catalogue
//*****************************************************************************/  
cmd_list_struct  cmd_list[]={  
/*命令  参数目录    处理函数        报警信息*/	
{"a",       1,      alarm_arg,     "alarm<arg1> <arg2>"},
{"d",       8,      config_arg,    "config"}, 
{"da",      8,      da_arg,        "config"},
{"db",      8,      db_arg,        "config"},
{"dx",      8,      dx_arg,        "config"},
{"g",       8,      gg_arg,        "config"},
{"h",       8,      aop_arg,       "config"},
{"i",       8,      aoerr_arg,     "config"},
{"is",      8,      install_arg,   "config"},
{"rs",      8,      date_arg,      "config"},
{"t",       8,      record_arg,    "config"},
{"x",       8,      clear_arg,     "config"},
{"ci",      1,      ci_arg,        "config"},
{"mi",      8,      setmid_arg,    "config"},
{"cf",      8,      cf_arg,        "config"},
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
Output: none
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
		flag_command=i+1; 
		UARTprintf("flag_command=%d\n",flag_command);
		return 1;
	 }
	}
  memset(cmd_tmp,0,strlen(cmd_tmp));	
	return 0; 
}
/***********************************************************
Function:setting relay 0 1 2 3
Input: none
Output: none
Author: megzheng
Date: 2017/10/17
Description:  .
***********************************************************/
void alarm_arg(void)
{
	//flag_command=0;
	switch(flag_function){
		case 0:
		UARTprintf("Relay #1 Select mode:\n\
		0 - Disable\n\
		1 - Hydrogen level\n\
		2 - Rate of Change\n\
		3 - OilTemp level\n\
		Select function: ");
		flag_function++;
			break;
		case 1:
		if(strlen(cmd_tmp)>0)
		{
      if(cmd_tmp[0]==0)
			{
			  memset(cmd_tmp,0,strlen(cmd_tmp)>0);
				UARTprintf("Relay #1 disable\n");
			}
		 flag_function++;	
		}
			break;
		default:
		flag_command=0;
			break;
	}
}

void config_arg(long int argc,void *cmd_arg)
{
	UARTprintf("config_arg");
}
void da_arg(long int argc,void *cmd_arg)
{
  UARTprintf("da_arg");
}
void db_arg(long int argc,void *cmd_arg)
{
  UARTprintf("db_arg");
}
void dx_arg(long int argc,void *cmd_arg)
{
  UARTprintf("dc_arg");
}
void gg_arg(long int argc,void *cmd_arg)
{
  UARTprintf("gg_arg");
}
void aop_arg(long int argc,void *cmd_arg)
{
  UARTprintf("aop_arg");
}
void aoerr_arg(long int argc,void *cmd_arg)
{
  UARTprintf("aoerro_arg");
}
void install_arg(long int argc,void *cmd_arg)
{
  UARTprintf("install_arg");
}
void date_arg(long int argc,void *cmd_arg)
{
	UARTprintf("date_arg");
}
void record_arg(long int argc,void *cmd_arg)
{
	UARTprintf("record_arg");
}
void clear_arg(long int argc,void *cmd_arg)
{
	UARTprintf("clear_arg");
}
void ci_arg(long int argc,void *cmd_arg)
{
	UARTprintf("clear_arg");
}
void setmid_arg(long int argc,void *cmd_arg)
{
	UARTprintf("clear_arg");
}
void cf_arg(long int argc,void *cmd_arg)
{
	UARTprintf("clear_arg");
}
	
	





