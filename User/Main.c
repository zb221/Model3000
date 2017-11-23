/***********************************************
@		Description: This file is main input for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/10.
***********************************************/
#define VARIABLE_GLOBALS
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
extern unsigned char a;
extern REALTIMEINFO CurrentTime;
extern unsigned char flag_mode;

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
Function:	INIT Global variable region.
Input:	none
Output: none
Author: zhuobin
Date: 2017/11/21
Description: all Global variable region init should add here.
***********************************************************/
void init_Global_Variable(void)
{
//	float Temp[4] = {10,30,50,70};                            /* zsy */
//	float Temp_R[4] = {92.130,97.952,104.021,110.174};        /* zsy */
//	float DAC_Din[5] =  {39577,39677,39877,40000,40200};      /* zsy */
//	float Din_temp[5] = {45.8,50.8,61.1,66.9,76.9};           /* zsy */
	float PCB_TEMP_Din[3] =  {11336,11536,11636};
	float PCB_TEMP_SET[3] = {37.5,42.6,44.9};
	float Temp[4] = {10,30,50,70};                          /* new sense */
	float Temp_R[4] = {93.661,100.345,107.373,114.696};     /* new sense */
	float DAC_Din[5] =  {39777,39877,40000,40200,40300};    /* new sense */
	float Din_temp[5] = {45.7,49.4,55,63.9,68.3};           /* new sense */
	
	/*The relationship between H2 and H2_resistance*/
	unsigned int H2[15] = {1,2,3,4,5,6,8,10,15,20,30,40,60,80,100};
	float OHM[15] = {607.732,608.799,609.422,611.832,615.097,617.59,619.734,621.5,625.587,628.711,634.544,639.727,648.659,656.587,663.789};
	
	output_data.MODEL_TYPE = 1;/*1->normal model; 2->debug model; 3->calibrate model*/
	output_data.temperature = 0;
	output_data.PCB_temp = 40;
	output_data.PcbTemp = 0;
	output_data.OilTemp = 0;
	output_data.TempResistor = 0;
	output_data.H2Resistor = 0;

	output_data.H2AG = 0;
	output_data.H2DG = 0;
	output_data.H2G = 0;
	output_data.H2SldAv = 0;
	output_data.DayROC = 0;
	output_data.WeekROC = 0;
	output_data.MonthROC = 0;
	output_data.SensorTemp = 0;
	
	Intermediate_Data.flag1 = 0;
	Intermediate_Data.flag2 = 0;
	Intermediate_Data.flag3 = 0;
	Intermediate_Data.flag4 = 0;
	
	Intermediate_Data.H2Resistor_OilTemp_K = 0;
	Intermediate_Data.H2Resistor_OilTemp_B = 0;

	Intermediate_Data.Din_temp_DAC_Din_K = 0;
	Intermediate_Data.Din_temp_DAC_Din_B = 0;

	Intermediate_Data.PCB_TEMP_Din_K = 0;
	Intermediate_Data.PCB_TEMP_Din_B = 0;

	Intermediate_Data.Temp_R_K = 0;
	Intermediate_Data.Temp_R_B = 0;

  /*copy Temp-Temp_R Temp-DAC_Din*/
	memcpy(Intermediate_Data.Temp,Temp,sizeof(float)*sizeof(Temp)/sizeof(Temp[0]));
	memcpy(Intermediate_Data.Temp_R,Temp_R,sizeof(float)*sizeof(Temp_R)/sizeof(Temp_R[0]));
	memcpy(Intermediate_Data.DAC_Din,DAC_Din,sizeof(float)*sizeof(DAC_Din)/sizeof(DAC_Din[0]));
	memcpy(Intermediate_Data.Din_temp,Din_temp,sizeof(float)*sizeof(Din_temp)/sizeof(Din_temp[0]));
	
	/*cpy DAC8568 Din data - PCB temp control*/
	memcpy(Intermediate_Data.H2,H2,sizeof(unsigned int)*sizeof(H2)/sizeof(H2[0]));
	memcpy(Intermediate_Data.OHM,OHM,sizeof(float)*sizeof(OHM)/sizeof(OHM[0]));

	/*cpy DAC8568 Din data - PCB temp control*/
	memcpy(Intermediate_Data.PCB_TEMP_Din,PCB_TEMP_Din,sizeof(float)*sizeof(PCB_TEMP_Din)/sizeof(PCB_TEMP_Din[0]));
	memcpy(Intermediate_Data.PCB_TEMP_SET,PCB_TEMP_SET,sizeof(float)*sizeof(PCB_TEMP_SET)/sizeof(PCB_TEMP_SET[0]));

}

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
	U23_AD7738_CS_INIT();
	AD7738_CS_INIT();
	DAC8568_CS_INIT();
	Initial_DS1390();
	Initial_e2prom();
	Init_M25P16();
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

  if (output_data.MODEL_TYPE == 1)
    UARTprintf("	%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f ",output_data.PcbTemp,output_data.H2AG,output_data.OilTemp,output_data.H2DG,output_data.H2G,output_data.H2SldAv,
		output_data.DayROC,output_data.WeekROC,output_data.MonthROC,output_data.SensorTemp,output_data.H2Resistor,output_data.TempResistor);
	else if (output_data.MODEL_TYPE == 2)
    UARTprintf("	%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f ",output_data.PcbTemp,output_data.H2AG,output_data.OilTemp,output_data.H2DG,output_data.H2G,output_data.H2SldAv,
		output_data.DayROC,output_data.WeekROC,output_data.MonthROC,output_data.SensorTemp,output_data.H2Resistor,output_data.TempResistor);
	
	if(output_data.temperature>=50)
	{
	UARTprintf(message_menu[1]);
	}
	else
	{
	UARTprintf(message_menu[0]);	
	}
	UARTprintf("\r\n");
}
void update_e2c(void)//run_parameter all save
{
 unsigned char *p;
 p=&(run_parameter.h2_ppm_h16.ubit.lo);
//	  e2prom512_write(p,128,0);
//		e2prom512_write(p+128,128,128);
//		e2prom512_write(p+256,128,256);
//		e2prom512_write(p+384,128,384);

	e2prom512_read(p+150*2,2,150*2);
	e2prom512_read(p+141*2,4,141*2);
	e2prom512_read(p+143*2,4,143*2);
	e2prom512_read(p+145*2,8,145*2);
	e2prom512_read(p+152*2,4,152*2);
	e2prom512_read(p+154*2,4,154*2);
	e2prom512_read(p+156*2,2,156*2);
	e2prom512_read(p+201*2,20,201*2);
	e2prom512_read(p+211*2,20,211*2);
	e2prom512_read(p+221*2,20,221*2);
//	for(i=0;i<512;i++)//test save accuracy
//	{
//	 UARTprintf("buffer %d %x\n",i,buffer[i]);
//	}
	
	
}
void UpData_ModbBus(REALTIMEINFO *Time)
{
	unsigned char Temp;
	unsigned char Tens, units;
	REALTIMEINFO TimeBCD;   //BCD码时间		
/////////////////////////////////////////////////////ModBus协议变量////////////////////////////////////////////////////////////////////

////0、1
run_parameter.h2_ppm_h16.hilo=0;//油中氢(带小数点)
run_parameter.h2_ppm_l16.hilo=400;	
//run_parameter.h2_ppm_l16.hilo=300;	
////4、5
run_parameter.h2_ppm_max_h16.hilo=0;//N2 Air 氢
run_parameter.h2_ppm_max_l16.hilo=800;
//run_parameter.h2_ppm_max_l16.hilo=300;
////2、3	
run_parameter.h2_ppm_dga_h16.hilo=0;//油中氢
run_parameter.h2_ppm_dga_l16.hilo=500;
//run_parameter.h2_ppm_dga_l16.hilo=300;
////6
run_parameter.die_temperature_celsius.hilo=50*100.F;//Sense温度

////7
run_parameter.pcb_temperature_celsius.hilo=50*100.F;//PCB温度	

////8
run_parameter.oil_temperature_celsius.hilo=50*100.F;//油温
////9、10

////11、12
run_parameter.h2_ppm_24hour_average_h16.hilo=10;//24h变化率
run_parameter.h2_ppm_24hour_average_l16.hilo=80;

////13、14
run_parameter.h2_ppm_DRC_h16.hilo=20;//天变化率
run_parameter.h2_ppm_DRC_l16.hilo=30;

////15、16
run_parameter.h2_ppm_WRC_h16.hilo=40;//周变化率
run_parameter.h2_ppm_WRC_l16.hilo=50;

////17、18
run_parameter.h2_ppm_MRC_h16.hilo=60;//月变化率
run_parameter.h2_ppm_MRC_l16.hilo=70;
//19-30保留

	DS1390_GetTime(&CurrentTime);
	Temp = Time->SpecificTime.sec;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.sec = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.min;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.min = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.hour;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.hour = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.week;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.week = ((Tens << 4) & 0xF0) | (units & 0x0F);	
	
	Temp = Time->SpecificTime.day;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.day = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.month;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.month = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.year;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.year = ((Tens << 4) & 0xF0) | (units & 0x0F);
	
	run_parameter.reserved_parameter35=(0x20<<8 | TimeBCD.SpecificTime.year);//?
	run_parameter.reserved_parameter341=(TimeBCD.SpecificTime.month<<8 | TimeBCD.SpecificTime.day);
	run_parameter.reserved_parameter36=TimeBCD.SpecificTime.hour;
	run_parameter.reserved_parameter37=(TimeBCD.SpecificTime.min<<8 | TimeBCD.SpecificTime.sec);
//	run_parameter.reserved_parameter35=CurrentTime.SpecificTime.year;//?
//	run_parameter.reserved_parameter341=CurrentTime.SpecificTime.month<<8 |CurrentTime.SpecificTime.day;
//	run_parameter.reserved_parameter36=CurrentTime.SpecificTime.hour;
//	run_parameter.reserved_parameter37=CurrentTime.SpecificTime.min<<8 | CurrentTime.SpecificTime.sec;

if(Runtimes>=0xFFFFFFFFFFFFFFFF)Runtimes=0;
run_parameter.run_time_in_secends_hh32.hilo=((Runtimes>>48)&0xFFFF);
run_parameter.run_time_in_secends_h32.hilo=((Runtimes>>32)&0xFFFF);
run_parameter.run_time_in_secends_ll32.hilo=((Runtimes>>16)&0xFFFF);
run_parameter.run_time_in_secends_l32.hilo=Runtimes&0xFFFF;

//run_parameter.h2_ppm_out_current_low.hilo=(unsigned short)(cmd_ConfigData.LowmA*100.F);
//run_parameter.h2_ppm_out_current_high.hilo=(unsigned short)(cmd_ConfigData.HighmA*100.F);
//run_parameter.h2_ppm_error_out_current.hilo=(unsigned short)(cmd_ConfigData.ErrmA*100.F);
//run_parameter.h2_ppm_no_ready_out_current.hilo=(unsigned short)(cmd_ConfigData.NotRmA*100.F);
//e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);
update_e2c();
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
	unsigned short i;
	unsigned char buffer[512];
  
	FrecInit();
  
	init_Global_Variable();
	init_peripherals();
	Init_ModBus();

	if (output_data.MODEL_TYPE == 1)
	    UARTprintf(print_menu);
	else if (output_data.MODEL_TYPE == 2)
		UARTprintf(debug_menu);
	
	DAC8568_INIT_SET(output_data.temperature,2*65536/5);	/*DOUT-C = xV*65536/5*/
	DAC8568_PCB_TEMP_SET(output_data.PCB_temp,0x1000);

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

		switch (Intermediate_Data.flag1)  {
			case 1:
			/* 1-4min capture 3min oil temp */
			Intermediate_Data.flag1 = 0;
			if(flag_screen==0)
			{
			UARTprintf("1-4min capture 3min oil temp\n");	
			}
			break;

			case 2:
			/* 4-1H4min set 50 temp, keep 1H  */
			output_data.temperature = 50;
			DAC8568_INIT_SET(output_data.temperature,2*65536/5);
			Intermediate_Data.flag1 = 0;
			UARTprintf("4-1H4min set 50 temp, keep 1H\n");
			break;

			case 3:
			/* 1H4min-1H7min stop heating, capture 3min oil temp */
			output_data.temperature = 0;
			DAC8568_INIT_SET(output_data.temperature,0);
			Intermediate_Data.flag1 = 0;
			UARTprintf("1H4min-1H7min stop heating, capture 3min oil temp\n");
			break;

			case 4:
			/* 1H7min-2H7min set 50 temp and keep 1H */
			output_data.temperature = 50;
			DAC8568_INIT_SET(output_data.temperature,2*65536/5);
			Intermediate_Data.flag1 = 0;
			UARTprintf("1H7min-2H7min set 50 temp and keep 1H\n");
			break;

			case 5:
			/* 2H7min-2H10min stop heating and capture oil temp 3min */
			output_data.temperature = 0;
			DAC8568_INIT_SET(output_data.temperature,0);
			Intermediate_Data.flag1 = 0;
			UARTprintf("2H7min-2H10min stop heating and capture oil temp 3min\n");
			break;

			case 6:
			/* 2H10min-3H40min set 70 temp and keep 1.5H */
			output_data.temperature = 70;
			DAC8568_INIT_SET(output_data.temperature,2*65536/5);
			Intermediate_Data.flag1 = 0;
			UARTprintf("2H10min-3H40min set 70 temp and keep 1.5H\n");
			break;

			case 7:
			/* 3H40min-4H10min set 50 temp and keep 0.5H */
			output_data.temperature = 50;
			DAC8568_INIT_SET(output_data.temperature,2*65536/5);
			Intermediate_Data.flag1 = 0;
			UARTprintf("3H40min-4H10min set 50 temp and keep 0.5H\n");
			break;

			case 8:
			/* 4H10min-4H13min stop heating and capture oil temp 3min */
			output_data.temperature = 0;
			DAC8568_INIT_SET(output_data.temperature,0);
			Intermediate_Data.flag1 = 0;
			UARTprintf("4H10min-4H13min stop heating and capture oil temp 3min\n");
			break;

			default:                                 /* Error Handling              */
			break;
		}

		switch (Intermediate_Data.flag2)  {
			case 1:
			device_checkself();
			relay_status();
			LED_status();
			command_print();
			UpData_ModbBus(&CurrentTime);
			update_e2c();
			Intermediate_Data.flag2 = 0;
			break;

			case 2:
			Intermediate_Data.flag2 = 0;
			break;

			default:                                 /* Error Handling              */
			break;
		}

		if (Intermediate_Data.flag3 == 1)
		{
			/*30min FLASH*/
			if(flag_screen==0)
			{
			UARTprintf("30min FLASH\n");			
			}
			Intermediate_Data.flag3 = 0;
		}
		
		if (Intermediate_Data.flag4 == 1)
 		{
 			/*2S command_print*/
 			ADC7738_acquisition_output(1);
 			ADC7738_acquisition_output(2);
 			ADC7738_acquisition_output(3);
			Intermediate_Data.flag4 = 0;
			M25P16_Data_Records();            /*save data into flash*/
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
			//after writing save to e2c	
		}	
	}
}

