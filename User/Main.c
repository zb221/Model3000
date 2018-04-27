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
#include "AD420.h"

/*-------------------------Global variable region----------------------*/
extern unsigned char flag_command;
extern unsigned char flag_function;
extern unsigned char rcv_buf[60];
extern unsigned char rcv_char[60];
extern volatile unsigned char rcv_new;
extern unsigned int rcv_cnt;
extern unsigned int rcv_char_cnt;
extern unsigned char cmd_tmp[CMD_LEN];
extern unsigned char cmd_buf[CMD_LEN];
extern unsigned char flag_screen;
extern unsigned char a;
extern REALTIMEINFO CurrentTime;
extern unsigned char flag_mode;
extern unsigned char rcv_char_flag;

static unsigned int print_count = 0; /* Countdown to each state */
unsigned char print_time = 30; /* console print cycle */
unsigned char startup_time = 60;
unsigned char cal_flag = 0;

const char print_menu[] = 
	"\n"
	"TimeStamp               PcbTemp         H2AG.ppm    OilTemp    H2DG.ppm    H2G.ppm    H2SldAv    DayROC    WeekROC    MonthROC    Message    \r\n";
const char debug_menu[] =
	"\n"
	"TimeStamp               PcbTemp         H2AG.ppm        OilTemp  H2DG.ppm   H2G.ppm   H2SldAv    DayROC   WeekROC  MonthROC    SensorTemp    H2Resistor    TemResistor    Message    \r\n";\
const char calibrate_menu[] =
	"\n"
	"TimeStamp               PcbTemp        H2AG.ppm            SensorTemp    H2Resistor    TemResistor        Message    \r\n";\

char message0[] = "rpt";
char message1[] = "wait";
char message2[] = "woff";
char message3[] = "avg";
char message4[] = "OVT";
char message5[] = "htr_off";
char message6[] = "warm_up";
char message7[] = "ramp_up";
char message8[] = "ramp_down";
char message9[] = "PF";
char message10[] = "R1";
char message11[] = "R2";
char message12[] = "R3";


void read_Piecewise_point_Sensor_Fit_Para(void)
{
  long long int test1 = 0;
  long long int test2 = 0;	
  long long int test3 = 0;	
	
	long long int test4 = 0;
	long long int test5 = 0;	
	long long int test6 = 0;	

	long long int test7 = 0;
	long long int test8 = 0;	
	long long int test9 = 0;	
	
	e2prom512_read((unsigned char*)&run_parameter.Piecewise_point0.ubit.hi,4,243*2);
	e2prom512_read((unsigned char*)&run_parameter.Piecewise_point1.ubit.hi,4,245*2);
	e2prom512_read((unsigned char*)&run_parameter.Piecewise_point2.ubit.hi,4,247*2);
	e2prom512_read((unsigned char*)&run_parameter.Piecewise_point3.ubit.hi,4,249*2);
	
	e2prom512_read((unsigned char*)&run_parameter.Sensor_Fit_Para_A.A.A1,24,19*2);
	e2prom512_read((unsigned char*)&run_parameter.Sensor_Fit_Para_B.B.B1,24,179*2);
	e2prom512_read((unsigned char*)&run_parameter.Sensor_Fit_Para_C.C.C1,24,231*2);
	
	test1 = (( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[2]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[3]);
	
	test2 = (( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[6]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[7]);
	
	test3 = (( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[10]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[11]);
	
	test4 = (( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[2]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[3]);
	
  test5 = (( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[6]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[7]);
		
  test6 = (( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[10]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[11]);
	
	test7 = (( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[2]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[3]);
	
  test8 = (( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[6]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[7]);
			
  test9 = (( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[10]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[11]);
	
	if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
	UARTprintf("point0=%.3f\n",(float)(run_parameter.Piecewise_point0.ubit.hi<<16 | run_parameter.Piecewise_point0.ubit.lo)/1000.0);
	UARTprintf("point1=%.3f\n",(float)(run_parameter.Piecewise_point1.ubit.hi<<16 | run_parameter.Piecewise_point1.ubit.lo)/1000.0);
	UARTprintf("point2=%.3f\n",(float)(run_parameter.Piecewise_point2.ubit.hi<<16 | run_parameter.Piecewise_point2.ubit.lo)/1000.0);
	UARTprintf("point3=%.3f\n",(float)(run_parameter.Piecewise_point3.ubit.hi<<16 | run_parameter.Piecewise_point3.ubit.lo)/1000.0);


	UARTprintf("a1=%.7f\n",(double)test1/1000000000.0);
	UARTprintf("b1=%.7f\n",(double)test2/1000000000.0);
	UARTprintf("c1=%.7f\n",(double)test3/1000000000.0);

	UARTprintf("a2=%.7f\n",(double)test4/1000000000.0);
	UARTprintf("b2=%.7f\n",(double)test5/1000000000.0);
	UARTprintf("c2=%.7f\n",(double)test6/1000000000.0);

	UARTprintf("a3=%.7f\n",(double)test7/1000000000.0);
	UARTprintf("b3=%.7f\n",(double)test8/1000000000.0);
	UARTprintf("c3=%.7f\n",(double)test9/1000000000.0);
	}
	
}

void reboot(void)
{
	if (run_parameter.reboot == 1){
	    UARTprintf("SYSTEM REBOOT\n");

		  IOCLR0 = IOCLR0 | 0x1<<29;
		  DelayNS(200);
			IOSET0 = IOSET0 | 0x1<<29;
		  run_parameter.reboot = 2;
	}
}

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
	unsigned char numb = 0;
	unsigned int temp_tmp = 0;
	float Temp[4] = {10,30,50,70};                            /* hhl */
	float Temp_R[4] = {107.506,114.057,120.848,128.008};        /* hhl */
	float DAC_Din[5] =  {39577,39677,39877,40000,40200};      /* hhl */
	float Din_temp[5] = {45.8,50.8,61.1,66.9,76.9};           /* hhl */
	
	float PCB_TEMP_Din[3] =  {11336,11536,11636};
	float PCB_TEMP_SET[3] = {37.5,42.6,44.9};
	
//	float Temp[4] = {10,30,50,70};                          /* new sense */
//	float Temp_R[4] = {93.661,100.345,107.373,114.696};     /* new sense */
//	float DAC_Din[5] =  {39777,39877,40000,40200,40300};    /* new sense */
//	float Din_temp[5] = {45.7,49.4,55,63.9,68.3};           /* new sense */
	
	/*The relationship between H2 and H2_resistance*/
	float H2[13] = {50,100,200,400,800,1600,3000,5000,10000,30000,40000,60000,100000};  /* new sense */
	float OHM[13] = {957.362,957.512,957.932,958.498,959.230,960.226,961.227,962.232,964.148,968.251,969.743,971.975,975.627};  /* new sense */

//	float H2[13] = {50,100,200,400,800,1600,3000,5000,8000,30000,40000,80000,100000};
//	float OHM[13] = {948.391,948.653,948.952,949.426,950.044,950.848,951.771,952.573,953.628,957.897,959.167,963.071,964.484};
	
	print_count = 60 / print_time;
	
	output_data.MODEL_TYPE = 1;/*1->normal model; 2->debug model; 3->calibrate model*/
	output_data.temperature = 0;
	output_data.PCB_temp = 40;
	output_data.PcbTemp = 0;
	output_data.OilTemp = 0;
	output_data.TempResistor = 0;
	output_data.H2Resistor = 0;
	output_data.H2R = 0;

	output_data.H2AG = 0;
	output_data.H2AG1 = 0;
	output_data.H2DG = 0;
	output_data.H2G = 0;
	output_data.H2SldAv = 0;
	output_data.DayROC = 0;
	output_data.WeekROC = 0;
	output_data.MonthROC = 0;
	output_data.SensorTemp = 0;
	
	Intermediate_Data.Heat_V = 0;
	
	Intermediate_Data.flag1 = 0;
	Intermediate_Data.flag2 = 0;
	Intermediate_Data.flag3 = 0;
	Intermediate_Data.flag4 = 0;
	Intermediate_Data.flag5 = 0;
	
  Intermediate_Data.Start_day = 0;
  Intermediate_Data.Start_week = 0;
	Intermediate_Data.Start_month = 0;
	
	Intermediate_Data.Power_On = 0;
	Intermediate_Data.Start_print_H2R = 0;
	Intermediate_Data.Start_print_calibrate_H2R = 0;
	Intermediate_Data.wait_1min = 1;
	Intermediate_Data.wait_1min_oil = 0;
	Intermediate_Data.current_cal = 0;
	
	Intermediate_Data.H2Resistor_OilTemp_K = 0;
	Intermediate_Data.H2Resistor_OilTemp_B = 0;

	Intermediate_Data.Din_temp_DAC_Din_K = 0;
	Intermediate_Data.Din_temp_DAC_Din_B = 0;

	Intermediate_Data.PCB_TEMP_Din_K = 0;
	Intermediate_Data.PCB_TEMP_Din_B = 0;

	Intermediate_Data.Temp_R_K = 0;
	Intermediate_Data.Temp_R_B = 0;
	
	Intermediate_Data.da_H2ppm = 0;
	Intermediate_Data.db_H2ppm = 0;
	
	Intermediate_Data.unready_current = 0;

	Intermediate_Data.M25P16_Data_Addr = 0;
//	Intermediate_Data.sector = 0;
//	Intermediate_Data.page = 0;
	Intermediate_Data.Alarm_page = 0;
	
	Intermediate_Data.count6 = 0;
  Intermediate_Data.count7 = 0;
	
	Intermediate_Data.Operat_temp_alarm = 0;
	Intermediate_Data.Oiltemp_Cal_flag = 0;
	Intermediate_Data.Oiltemp_Cal_OK = 0;
	Intermediate_Data.intercept = 0;
	Intermediate_Data.Oiltemp_Over = 0;
	
	run_parameter.reboot = 0;

  /* read eeprom init data*/
	e2prom512_read(&run_parameter.h2_ppm_calibration_gas_h16.ubit.lo,4,126*2);
	e2prom512_read(&output_data.MODEL_TYPE,1,160*2);
	read_Piecewise_point_Sensor_Fit_Para();
	
  /*copy Temp-Temp_R Temp-DAC_Din*/
	memcpy(Intermediate_Data.Temp,Temp,sizeof(float)*sizeof(Temp)/sizeof(Temp[0]));
	memcpy(Intermediate_Data.Temp_R,Temp_R,sizeof(float)*sizeof(Temp_R)/sizeof(Temp_R[0]));
	memcpy(Intermediate_Data.DAC_Din,DAC_Din,sizeof(float)*sizeof(DAC_Din)/sizeof(DAC_Din[0]));
	memcpy(Intermediate_Data.Din_temp,Din_temp,sizeof(float)*sizeof(Din_temp)/sizeof(Din_temp[0]));

	numb = sizeof(Intermediate_Data.Temp)/sizeof(Intermediate_Data.Temp[0]);
	for (unsigned i = 0;i < numb;i++){
		e2prom512_read((unsigned char*)&temp_tmp,4,(260+(i*2))*2);
		Intermediate_Data.Temp[i] = (float)temp_tmp/100.0;
		e2prom512_read((unsigned char*)&temp_tmp,4,(268+(i*2))*2);
		Intermediate_Data.Temp_R[i] = (float)temp_tmp/1000.0;
		if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
				UARTprintf("%.2f/%.3f\n",Intermediate_Data.Temp[i],Intermediate_Data.Temp_R[i]);
		}
	}
	/*cpy H2-OHM*/
	memcpy(Intermediate_Data.H2,H2,sizeof(float)*sizeof(H2)/sizeof(H2[0]));
	memcpy(Intermediate_Data.OHM,OHM,sizeof(float)*sizeof(OHM)/sizeof(OHM[0]));

	/*cpy DAC8568 Din data - PCB temp control*/
	memcpy(Intermediate_Data.PCB_TEMP_Din,PCB_TEMP_Din,sizeof(float)*sizeof(PCB_TEMP_Din)/sizeof(PCB_TEMP_Din[0]));
	memcpy(Intermediate_Data.PCB_TEMP_SET,PCB_TEMP_SET,sizeof(float)*sizeof(PCB_TEMP_SET)/sizeof(PCB_TEMP_SET[0]));

	memset(Intermediate_Data.H2G_tmp,0,sizeof(Intermediate_Data.H2G_tmp)/sizeof(Intermediate_Data.H2G_tmp[0]));
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
	ZIGBB_PWR();
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
//	UARTprintf("output_data.MODEL_TYPE=%d\n",output_data.MODEL_TYPE);
	DS1390_GetTime(&CurrentTime);
	UARTprintf("%d/%02d/%02d %02d:%02d:%02d",CurrentTime.SpecificTime.year+2000,CurrentTime.SpecificTime.month,
	CurrentTime.SpecificTime.day,CurrentTime.SpecificTime.hour,CurrentTime.SpecificTime.min,CurrentTime.SpecificTime.sec);

  if (output_data.MODEL_TYPE == 1)
    UARTprintf("	%7.3f       %10.3f    %7.3f    %8.3f   %8.3f   %8.3f  %8.3f   %8.3f    %8.3f    ",output_data.PcbTemp,output_data.H2AG,output_data.OilTemp,output_data.H2DG,output_data.H2G,output_data.H2SldAv,
		output_data.DayROC,output_data.WeekROC,output_data.MonthROC);
	else if (output_data.MODEL_TYPE == 2)
    UARTprintf("	%7.3f  %10.3f %10.3f  %7.3f  %8.3f  %8.3f  %8.3f  %8.3f  %8.3f  %8.3f      %8.3f      %8.3f       %8.3f    ",output_data.PcbTemp,output_data.H2AG,output_data.H2AG1,output_data.OilTemp,output_data.H2DG,output_data.H2G,output_data.H2SldAv,
		output_data.DayROC,output_data.WeekROC,output_data.MonthROC,output_data.SensorTemp,output_data.H2Resistor,output_data.TempResistor);
	else if (output_data.MODEL_TYPE == 3)
		UARTprintf("	%7.3f    %10.3f %10.3f      %7.3f      %8.3f        %7.3f        ",output_data.PcbTemp,output_data.H2AG,output_data.H2AG1,
	  output_data.SensorTemp,output_data.H2Resistor,output_data.TempResistor);
	
	if(output_data.temperature == 50)
	{
		if (Intermediate_Data.wait_1min == 1)
      UARTprintf(message1);
    else
			UARTprintf(message6);
		UARTprintf("%d",print_count--);

	}else{
		if (Intermediate_Data.wait_1min_oil == 1)
	    UARTprintf(message3);
		else
		  UARTprintf(message5);
    UARTprintf("%d",print_count--);
	}
	
	if (output_data.OilTemp >= run_parameter.OilTemp_Alarm_celsius.hilo){
	    UARTprintf(",R3");
	}

	if (output_data.DayROC >= run_parameter.h2_ppm_alarm_low_l16.hilo)
			UARTprintf(",R2");
	
	if (output_data.H2DG >= run_parameter.h2_ppm_alert_low_l16.hilo)
			UARTprintf(",R1");
	
	if (Intermediate_Data.Operat_temp_alarm == 1)
		;
	
	UARTprintf("\r\n");
}

void update_e2c(void)//run_parameter all save
{

	e2prom512_read(&run_parameter.unit_id.ubit.lo,1,150*2);
	e2prom512_read(&run_parameter.h2_ppm_report_low_h16.ubit.lo,4,141*2);
	e2prom512_read(&run_parameter.h2_ppm_report_high_h16.ubit.lo,4,143*2);
	e2prom512_read(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);
	e2prom512_read(&run_parameter.h2_ppm_alert_low_h16.ubit.lo,4,152*2);
	e2prom512_read(&run_parameter.h2_ppm_alarm_low_h16.ubit.lo,4,154*2);
	e2prom512_read(&run_parameter.OilTemp_Alarm_celsius.ubit.lo,2,156*2);
	e2prom512_read(&run_parameter.own_id.own_id_sstr.character1,20,201*2);
	e2prom512_read(&run_parameter.sub_station_id.sub_station_id_sstr.character1,20,211*2);
	e2prom512_read(&run_parameter.transformer_id.transformer_id_sstr.character1,20,221*2);
	
	e2prom512_read(&run_parameter.calibration_date.day,4,128*2);
	
}
void UpData_ModbBus(REALTIMEINFO *Time)
{
	unsigned char Temp;
	unsigned char Tens, units;
	REALTIMEINFO TimeBCD;   //BCD码时间		
/*-------------------------------------------------ModBus协议变量----------------------------------------*/
	run_parameter.h2_ppm_h16.hilo = (unsigned int)(output_data.H2DG) >> 16;  //H2AG->0、1
	run_parameter.h2_ppm_l16.hilo = (unsigned int)(output_data.H2DG) & 0xFFFF;	

	run_parameter.h2_ppm_max_h16.hilo = (unsigned int)(output_data.H2G) >> 16;  //N2 Air 氢 ->4、5
	run_parameter.h2_ppm_max_l16.hilo = (unsigned int)(output_data.H2G) & 0xFFFF;

	run_parameter.h2_ppm_dga_h16.hilo = (unsigned int)(output_data.H2DG) >> 16;  //油中氢 ->2、3
	run_parameter.h2_ppm_dga_l16.hilo = (unsigned int)(output_data.H2DG) & 0xFFFF;

	run_parameter.die_temperature_celsius.hilo = (unsigned int)(output_data.SensorTemp*100.F);//Sense温度

	////7
	run_parameter.pcb_temperature_celsius.hilo = (unsigned int)(output_data.PcbTemp*100.F);//PCB温度	

	////8
	run_parameter.oil_temperature_celsius.hilo = (unsigned int)(output_data.OilTemp*100.F);//油温
	////9、10

	////11、12
	run_parameter.h2_ppm_24hour_average_h16.hilo = (unsigned int)(output_data.H2SldAv) >> 16;//24h变化率
	run_parameter.h2_ppm_24hour_average_l16.hilo = (unsigned int)(output_data.H2SldAv) & 0xFFFF;

	////13、14
	run_parameter.h2_ppm_DRC_h16.hilo = (unsigned int)(output_data.DayROC) >> 16;//天变化率
	run_parameter.h2_ppm_DRC_l16.hilo = (unsigned int)(output_data.DayROC) & 0xFFFF;

	////15、16
	run_parameter.h2_ppm_WRC_h16.hilo = (unsigned int)(output_data.WeekROC) >> 16;//周变化率
	run_parameter.h2_ppm_WRC_l16.hilo = (unsigned int)(output_data.WeekROC) & 0xFFFF;

	////17、18
	run_parameter.h2_ppm_MRC_h16.hilo = (unsigned int)(output_data.MonthROC) >> 16;//月变化率
	run_parameter.h2_ppm_MRC_l16.hilo = (unsigned int)(output_data.MonthROC) & 0xFFFF;
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
	
	run_parameter.reserved_parameter35 = (0x20<<8 | TimeBCD.SpecificTime.year);//?
	run_parameter.reserved_parameter341 = (TimeBCD.SpecificTime.month<<8 | TimeBCD.SpecificTime.day);
	run_parameter.reserved_parameter36 = TimeBCD.SpecificTime.hour;
	run_parameter.reserved_parameter37 = (TimeBCD.SpecificTime.min<<8 | TimeBCD.SpecificTime.sec);

	if(Runtimes >= 0xFFFFFFFFFFFFFFFF)
		Runtimes=0;

	run_parameter.run_time_in_secends_hh32.hilo=((Runtimes>>48)&0xFFFF);
	run_parameter.run_time_in_secends_h32.hilo=((Runtimes>>32)&0xFFFF);
	run_parameter.run_time_in_secends_ll32.hilo=((Runtimes>>16)&0xFFFF);
	run_parameter.run_time_in_secends_l32.hilo=Runtimes&0xFFFF;

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
	short val1 = 0, val2 = 0;
	FrecInit();
	init_peripherals();
	init_Global_Variable();
	Init_ModBus();

  DAC8568_INIT_SET(output_data.temperature,2*65536/5);	/* Set Senseor temperature :DOUT-C = xV*65536/5 */
	DAC8568_PCB_TEMP_SET(output_data.PCB_temp,0x1000);    /* Set PCB default temperature */
	M25P16_erase_map(31*0x10000,SE);
	
	PINSEL1 = PINSEL1 & (~(0x03 << 26));        /*P0.29*/	
	IODIR0 = IODIR0 | 0x1<<29;
	IOSET0 = IOSET0 | 0x1<<29;		

	while (1)  
	{
    if (rcv_char_flag == 1){
			UART0_SendData(rcv_char,rcv_char_cnt);
			memset(rcv_char,0,sizeof(rcv_char));
			rcv_char_cnt = 0;
			rcv_char_flag = 0;
		}
		
		if(rcv_new==1)
		{
      a = get_true_char_stream(cmd_tmp,rcv_buf);
      memset(rcv_buf,0,sizeof(rcv_buf));
			rcv_cnt = 0;
      rcv_new = 0;

			if (flag_screen == 0){
			if ((findcmd(cmd_tmp) == 0)&&(a != 0)){
					UARTprintf("There is no corresponding command. Please check the input command.\n");
				}
			}
		}

		switch(flag_command){
			case 0:
				if(findcmdfunction(cmd_tmp) == 1){
					a = 0;
					flag_screen = 1;
          if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
		  			UARTprintf("Close the echo\n\n");
          }
				}
				memset(cmd_tmp,0,sizeof(cmd_tmp));
				break;
			
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
			
			case 19:
				firmware_arg();
				break;
			
			default:
				memset(cmd_tmp,0,sizeof(cmd_tmp));
				a = 0;
				break;
		}

		switch (Intermediate_Data.flag1)  {
			case 1:
			output_data.temperature = 0;
			DAC8568_INIT_SET(output_data.temperature,0);
			Intermediate_Data.flag1 = 0;
			print_count = 3*60 / print_time;
      if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
			  UARTprintf("1-4min capture 3min oil temp\n");	
      }
			break;

			case 2:
			output_data.temperature = 50;
			DAC8568_INIT_SET(output_data.temperature,2.35*65536/5);
			Intermediate_Data.flag1 = 0;
			print_count = 60*60 / print_time;
      if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
	  		UARTprintf("4-1H4min set 50 temp, keep 1H\n");
      }
			break;

			case 3:
			output_data.temperature = 0;
			DAC8568_INIT_SET(output_data.temperature,0);
			Intermediate_Data.flag1 = 0;
			print_count = 3*60 / print_time;
      if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
			  UARTprintf("1H4min-1H7min stop heating, capture 3min oil temp\n");
      }
			break;

			case 4:
			output_data.temperature = 50;
			DAC8568_INIT_SET(output_data.temperature,2.35*65536/5);
			Intermediate_Data.flag1 = 0;
			print_count = 60*60 / print_time;
      if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
		  	UARTprintf("1H7min-2H7min set 50 temp and keep 1H\n");
      }
			break;

			case 5:
			output_data.temperature = 0;
			DAC8568_INIT_SET(output_data.temperature,0);
			Intermediate_Data.flag1 = 0;
			print_count = 3*60 / print_time;
      if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
		  	UARTprintf("2H7min-2H10min stop heating and capture oil temp 3min\n");
      }
			break;

			case 6:
			output_data.temperature = 70;
			DAC8568_INIT_SET(output_data.temperature,2.35*65536/5);
			Intermediate_Data.flag1 = 0;
			print_count = 120*60 / print_time;
      if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
	   		UARTprintf("2H10min-4H10min set 70 temp and keep 2H\n");
      }
			break;

			case 7:
			output_data.temperature = 50;
			DAC8568_INIT_SET(output_data.temperature,2.35*65536/5);
			Intermediate_Data.flag1 = 0;
			print_count = 30*60 / print_time;
      if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
	   		UARTprintf("4H10min-4H40min set 50 temp and keep 0.5H\n");
      }
			break;

			default:
			break;
		}

		switch (Intermediate_Data.flag2)  {
			case 1:
			device_checkself();
			relay_status();
			LED_status();
			
			UpData_ModbBus(&CurrentTime);
			update_e2c();
			Intermediate_Data.flag2 = 0;
			if ((cal_flag == 0)&&(Intermediate_Data.count7 == 1)){
				output_data.temperature = 50;
				DAC8568_INIT_SET(output_data.temperature,2*65536/5);	/* Set Senseor temperature :DOUT-C = xV*65536/5 */
				Intermediate_Data.Start_print_calibrate_H2R = 1;
				cal_flag = 1;
			}
			break;

			case 2:
			Intermediate_Data.flag2 = 0;
      if (Intermediate_Data.Power_On == 0)		
			    UARTprintf("System startup wait time %d\n",--startup_time);
			if (startup_time == 2){
				--startup_time;
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
			}
			break;

			default:
			break;
		}

		if (Intermediate_Data.flag3 == 1)
		{
			Intermediate_Data.flag3 = 0;
			H2SldAv_24H2G();
			M25P16_Data_Records();
			if (output_data.OilTemp >= run_parameter.OilTemp_Alarm_celsius.hilo || output_data.DayROC >= run_parameter.h2_ppm_alarm_low_l16.hilo || output_data.H2DG >= run_parameter.h2_ppm_alert_low_l16.hilo)
				M25P16_Alarm_Log_Records();
      if (output_data.MODEL_TYPE == 2 || output_data.MODEL_TYPE == 3){
		    UARTprintf("save data into flash\n");			
      }
		}

		if (Intermediate_Data.flag4 == 1)
 		{
 			/*30S command_print*/
			ADC7738_acquisition_output(1);
			ADC7738_acquisition_output(3);
			Calculate_H2_rate();
			
			if (Intermediate_Data.unready_current == 0){
				if (Intermediate_Data.current_cal == 0)
				  AD420_OUTPUT_SET((65535.0/20.0)*2);
				run_parameter.status_flag.ubit.senser_state0=0;
				run_parameter.status_flag.ubit.senser_state1=0;
				run_parameter.status_flag.ubit.senser_state2=0;
			}
			if (output_data.temperature == 50 && Intermediate_Data.wait_1min == 1){
				if (Intermediate_Data.current_cal == 0){
					e2prom512_read((unsigned char*)&val1,2,118*2);
					e2prom512_read((unsigned char*)&val2,2,119*2);
//					UARTprintf("val1 = %d, h2_ppm_out_current_high = %d,h2_ppm_out_current_low = %d\n",val1,run_parameter.h2_ppm_out_current_high.hilo,run_parameter.h2_ppm_out_current_low.hilo);
//			   UARTprintf("%f\n",((-(float)val1/100.0)+(float)run_parameter.h2_ppm_out_current_low.hilo/100.0+(((float)(run_parameter.h2_ppm_out_current_high.hilo - run_parameter.h2_ppm_out_current_low.hilo)/100.0)/5000.0)*output_data.H2DG));
          if (((-(float)val1/100.0)+(float)run_parameter.h2_ppm_out_current_low.hilo/100.0+(((float)(run_parameter.h2_ppm_out_current_high.hilo - run_parameter.h2_ppm_out_current_low.hilo)/100.0)/5000.0)*output_data.H2DG) <= 20)
					AD420_OUTPUT_SET((65535.0/20.0)*((-(float)val1/100.0)+(float)run_parameter.h2_ppm_out_current_low.hilo/100.0+(((float)(run_parameter.h2_ppm_out_current_high.hilo - run_parameter.h2_ppm_out_current_low.hilo)/100.0)/5000.0)*output_data.H2DG));
          else
					AD420_OUTPUT_SET((65535.0/20.0)*20);
				}
		  }

			Intermediate_Data.flag4 = 0;
			if(flag_screen==0)
			{
				if (Intermediate_Data.Power_On == 1)
			      command_print();
			}
		}
		ADC7738_acquisition(1);
		ADC7738_acquisition(2);
		ADC7738_acquisition(3);
    ADC7738_acquisition_output(2);
		
		if(user_parameter.flag.ubit.recept_ok==1)
		{			
			Data_Ack_Processor();
		}
		if(user_parameter.flag.ubit.recept_write==1)
		{
			RW_ModBus_Data();
		}
   reboot();		
	}
}

