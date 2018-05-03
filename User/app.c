/***********************************************
@		Description: This file is APP for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/10.
***********************************************/
#include <LPC21xx.H>                     /* LPC21xx definitions               */
#include "app.h"
#include "Peripherals_LPC2194.h"
#include "parameter.h"
#include "AD7738.h"
#include "AD420.h"
#include "e25LC512.h"
#include "DAC8568.h"

unsigned char Heating_R_failure = 0;
/***********************************************************
Function:	LED status.
Input:	none
Output: none
Author: zhuobin
Date: 2017/11/23
Description: 
***********************************************************/
void LED_status(void)
{
	static unsigned char LED_BLUE_FLAG = 0, LED_RED_FLAG = 0;
	
	if (LED_BLUE_FLAG < 200){
		LED_BLUE_FLAG++;
		LED_RED_FLAG++;
	}else{
	  LED_BLUE_FLAG = 0;
		LED_RED_FLAG = 0;
	}
	
	if (LED_BLUE_FLAG%2 == 0)
		LED_BLUE_SET
	else
	  LED_BLUE_CLR
	
	LED_RED_CLR

	if (output_data.temperature == 0){
		LED_RED_CLR
    LED_BLUE_SET
	}
	if ((Heating_R_failure == 1) && (Intermediate_Data.Power_On == 1)){
//		UARTprintf("test1\n");
		AD420_OUTPUT_SET((65535.0/20.0)*3.0); /*output error current 3mA*/
		LED_RED_SET
	  LED_BLUE_SET
	}
	
	if (output_data.OilTemp >= run_parameter.OilTemp_Alarm_celsius.hilo || output_data.DayROC >= run_parameter.h2_ppm_alarm_low_l16.hilo || output_data.H2DG >= run_parameter.h2_ppm_alert_low_l16.hilo) /* OilTemp  too high, DGA over, DGA dayROC over */
	{
		if (LED_RED_FLAG%2 == 0)
			LED_RED_SET
		else
			LED_RED_CLR
		LED_BLUE_SET
	}
	
	if (Intermediate_Data.Power_On == 1){
		if ((output_data.OilTemp<(-20)) || (output_data.OilTemp>105) || (output_data.PcbTemp<30) || (output_data.PcbTemp>50)){
//			UARTprintf("test2\n");
				AD420_OUTPUT_SET((65535.0/20.0)*3.0); /*output error current 3mA*/
		}
	}
}

/***********************************************************
Function:	relay status.
Input:	none
Output: none
Author: zhuobin
Date: 2017/11/23
Description: 
***********************************************************/
void relay_status(void)
{
	if (output_data.H2DG >= run_parameter.h2_ppm_alert_low_l16.hilo)
	  Relay1_Pin_SET
	else
  	Relay1_Pin_CLR
	
	if (output_data.DayROC >= run_parameter.h2_ppm_alarm_low_l16.hilo)
    Relay2_Pin_SET
	else
  	Relay2_Pin_CLR

	if (output_data.OilTemp >= run_parameter.OilTemp_Alarm_celsius.hilo){
	  Relay3_Pin_SET
		run_parameter.status_flag.ubit.senser_state0=0;
		run_parameter.status_flag.ubit.senser_state1=0;
		run_parameter.status_flag.ubit.senser_state2=1;
	}
	else
	  Relay3_Pin_CLR
}

/***********************************************************
Function:	Heating resistance self-check.
Input:	none
Output: U23-AIN7 voltage
Author: zhuobin
Date: 2017/11/22
Description: device checkself for connectivity of the sensor .....
***********************************************************/
float Heat_R_checkself(void)
{
    unsigned char flag = 0;
    unsigned char data0 = 0, data1 = 0, data2 = 0;	/* The raw data output from ADC */
	
    IOCLR1 = IOCLR1 | 0x1<<23; /*CS2->P1.23 set low*/
    SPI0_SendDate(0<<6|(0x3F & 0x2F));
    SPI0_SendDate(0x04);
    IOSET1 = IOSET1 | 0x1<<23; /*CS2->P1.23 set hight*/

    IOCLR1 = IOCLR1 | 0x1<<23; /*CS2->P1.23 set low*/
    SPI0_SendDate(0<<6|(0x3F & 0x37));
    SPI0_SendDate(1<<7|2);
    IOSET1 = IOSET1 | 0x1<<23; /*CS2->P1.23 set hight*/

    IOCLR1 = IOCLR1 | 0x1<<23; /*CS2->P1.23 set low*/
    SPI0_SendDate(0<<6|(0x3F & 0x3F));
    SPI0_SendDate(2<<5|1<<4|0<<3|0<<2|1<<1|1);
    IOSET1 = IOSET1 | 0x1<<23; /*CS2->P1.23 set hight*/
	
    flag = 0;
    while(!((flag>>7)&0x1)){
			IOCLR1 = IOCLR1 | 0x1<<23; /*CS2->P1.23 set low*/
			SPI0_SendDate(1<<6|(0x3F & 0x04));
			flag = SPI0_SendDate(0x00);
			IOSET1 = IOSET1 | 0x1<<23; /*CS2->P1.23 set hight*/
    }
		
		IOCLR1 = IOCLR1 | 0x1<<23; /*CS2->P1.23 set low*/
    SPI0_SendDate(1<<6|(0x3F & 0x0F));
    data0 = SPI0_SendDate(0xFF);
    data1 = SPI0_SendDate(0xFF);
    data2 = SPI0_SendDate(0xFF);
	
    IOSET1 = IOSET1 | 0x1<<23; /*CS2->P1.23 set hight*/
		return (float)(data0<<16|data1<<8|data2)/(8388607/2500);
}

/***********************************************************
Function:	device checkself.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/17
Description: device checkself for connectivity of the sensor .....
***********************************************************/
void device_checkself(void)
{
    Intermediate_Data.Heat_V = Heat_R_checkself();
	  if (output_data.MODEL_TYPE == 2)
		    UARTprintf("U23-AIN7 = %.3f\n",Intermediate_Data.Heat_V);
	  Heating_R_failure = 0;
	  if (Intermediate_Data.Heat_V<200 && output_data.temperature>30){
//					UARTprintf("%.3f Heating resistance self-check error.\n",Intermediate_Data.Heat_V);
					Heating_R_failure = 1;
		}
		if ((output_data.temperature == 50)||(output_data.temperature == 70)){
//			if (Intermediate_Data.Heat_V>940)
//				DAC8568_SET(0x0,0x3,0x2,0,0);		       /* when > 400 mA, DAC-C set 0 */
//			else
//				DAC8568_SET(0x0,0x3,0x2,2.35*65536/5,0);
	  }
		if ((output_data.MODEL_TYPE == 1)||(output_data.MODEL_TYPE == 2)){
		e2prom512_read((unsigned char*)&run_parameter.Piecewise_point0.ubit.hi,4,243*2);
		e2prom512_read((unsigned char*)&run_parameter.Piecewise_point3.ubit.hi,4,249*2);
			
//    UARTprintf("%d \n",(run_parameter.Piecewise_point0.ubit.hi<<16 | run_parameter.Piecewise_point0.ubit.lo));
//    UARTprintf("%f \n",(run_parameter.Piecewise_point0.ubit.hi<<16 | run_parameter.Piecewise_point0.ubit.lo));
		
		if ((output_data.H2Resistor < (float)(run_parameter.Piecewise_point0.ubit.hi<<16 | run_parameter.Piecewise_point0.ubit.lo)/1000.0-1000) 
			|| (output_data.H2Resistor > (float)(run_parameter.Piecewise_point3.ubit.hi<<16 | run_parameter.Piecewise_point3.ubit.lo)/1000.0+1000))
			Heating_R_failure = 1;
		
		if ((output_data.TempResistor < (-40.0 - Intermediate_Data.Temp_R_B)/Intermediate_Data.Temp_R_K) 
		|| (output_data.TempResistor > (105.0 - Intermediate_Data.Temp_R_B)/Intermediate_Data.Temp_R_K))
			Heating_R_failure = 1;
	}
		
}

/***********************************************************
Function:	Calculate H2 rate.
Input:	none
Output: none
Author: zhuobin
Date: 2017/12/4
Description: Calculate daily change rate, weekly change rate, monthly change rate.
***********************************************************/
float day_ref[6] = {0};
float week_ref[6] = {0};
float month_ref[7] = {0};

void Calculate_H2_rate(void)
{
	static unsigned char day_ref_number = 0, week_ref_number = 0, month_ref_number = 0, start = 0;
		
	if (Intermediate_Data.Start_day == 1){
		if (start == 0){
			start = 1;
			output_data.DayROC = 0;
			day_ref[sizeof(day_ref)/sizeof(day_ref[0])-1] = output_data.H2DG;
			week_ref[sizeof(week_ref)/sizeof(week_ref[0])-1] = output_data.H2DG;
			month_ref[sizeof(month_ref)/sizeof(month_ref[0])-1] = output_data.H2DG;
		}
		if (Intermediate_Data.flag5 == 1){
		  Intermediate_Data.flag5 = 0;
      
			day_ref[day_ref_number++] = output_data.H2DG;
			output_data.DayROC = output_data.H2DG - day_ref[sizeof(day_ref)/sizeof(day_ref[0])-1];
			if (day_ref_number == sizeof(day_ref)/sizeof(day_ref[0])){
				day_ref_number = 0;
			}	
		}
	}

	if (Intermediate_Data.Start_week == 1){
		output_data.WeekROC = output_data.DayROC;
		if (Intermediate_Data.flag5 == 2){
			Intermediate_Data.flag5 = 0;
			
			week_ref[week_ref_number++] = output_data.H2DG;
		  output_data.WeekROC = output_data.H2DG - week_ref[sizeof(week_ref)/sizeof(week_ref[0])-1];
			if (week_ref_number == sizeof(week_ref)/sizeof(week_ref[0])){
				week_ref_number = 0;
			}
		}
	}
	
	if (Intermediate_Data.Start_month == 1){
    output_data.MonthROC = output_data.WeekROC;
		if (Intermediate_Data.flag5 == 3){
      Intermediate_Data.flag5 = 0;
			
			month_ref[month_ref_number++] = output_data.H2DG;
		  output_data.MonthROC = output_data.H2DG - month_ref[sizeof(month_ref)/sizeof(month_ref[0])-1];
			if (month_ref_number == sizeof(month_ref)/sizeof(month_ref[0])){
				month_ref_number = 0;
			}
		}
	}
	
}

/***********************************************************
Function:	H2SldAv from 24H H2G.
Input:	none
Output: none
Author: zhuobin
Date: 2017/12/4
Description: .
***********************************************************/
void H2SldAv_24H2G(void)
{
	static unsigned char number = 0, flag = 0;
	static float H2SldAv1 = 0;
  
	Intermediate_Data.H2G_tmp[number++] = output_data.H2G;
	
	if (flag == 1){
		output_data.H2SldAv = AVERAGE_F(Intermediate_Data.H2G_tmp);
	}else if (number <= sizeof(Intermediate_Data.H2G_tmp)/sizeof(Intermediate_Data.H2G_tmp[0]) && flag == 0){
	  H2SldAv1 += Intermediate_Data.H2G_tmp[number-1];
		output_data.H2SldAv = H2SldAv1 / number;
	}
	
	if (number == sizeof(Intermediate_Data.H2G_tmp)/sizeof(Intermediate_Data.H2G_tmp[0])){
		number = 0;
	  flag = 1;
	}
	
}

void E2C_Sensor_Fit_Para(void)
{
	int i = 0;
  long long int test1 = 0;
  long long int test2 = 0;	
  long long int test3 = 0;	
	
	long long int test4 = 0;
	long long int test5 = 0;	
	long long int test6 = 0;	

	long long int test7 = 0;
	long long int test8 = 0;	
	long long int test9 = 0;	
	
//	test1 = 123456789000000;
//	test2 = -23456789000000;	
//	test3 = 335792468000000;

//	test4 = 423456789000000;
//	test5 = -53456789000000;	
//	test6 = 635792468000000;

//	test7 = 723456789000000;
//	test8 = -83456789000000;	
//	test9 = 935792468000000;	
//	
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[0] = ((test1>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[1] = ((test1>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[2] = ((test1>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[3] = test1&0xFFFF;
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[4] = ((test2>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[5] = ((test2>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[6] = ((test2>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[7] = test2&0xFFFF;
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[8] = ((test3>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[9] = ((test3>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[10] = ((test3>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_A.Fit_Para_A[11] = test3&0xFFFF;
//	
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[0] = ((test4>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[1] = ((test4>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[2] = ((test4>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[3] = test4&0xFFFF;
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[4] = ((test5>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[5] = ((test5>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[6] = ((test5>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[7] = test5&0xFFFF;
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[8] = ((test6>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[9] = ((test6>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[10] = ((test6>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_B.Fit_Para_B[11] = test6&0xFFFF;
//	
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[0] = ((test7>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[1] = ((test7>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[2] = ((test7>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[3] = test7&0xFFFF;
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[4] = ((test8>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[5] = ((test8>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[6] = ((test8>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[7] = test8&0xFFFF;
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[8] = ((test9>>48)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[9] = ((test9>>32)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[10] = ((test9>>16)&0xFFFF);
//	run_parameter.Sensor_Fit_Para_C.Fit_Para_C[11] = test9&0xFFFF;
	
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


	e2prom512_write((unsigned char*)&run_parameter.Sensor_Fit_Para_A.A.A1,24,19*2);
	e2prom512_write((unsigned char*)&run_parameter.Sensor_Fit_Para_B.B.B1,24,179*2);
	e2prom512_write((unsigned char*)&run_parameter.Sensor_Fit_Para_C.C.C1,24,231*2);

	e2prom512_read((unsigned char*)&run_parameter.Sensor_Fit_Para_A.A.A1,24,19*2);
	e2prom512_read((unsigned char*)&run_parameter.Sensor_Fit_Para_B.B.B1,24,179*2);
	e2prom512_read((unsigned char*)&run_parameter.Sensor_Fit_Para_C.C.C1,24,231*2);
	
	if (((test1 == (( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[2]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[3]))&&
    (test2 == (( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[6]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[7]))&&
    (test3 == (( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[10]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[11])))&&
			
		((test4 == (( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[2]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[3]))&&
    (test5 == (( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[6]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[7]))&&
    (test6 == (( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[10]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[11])))&&
			
		((test7 == (( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[2]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[3]))&&
    (test8 == (( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[6]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[7]))&&
    (test9 == (( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[10]<<16
		| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[11]))))
			run_parameter.Sensor_Fit_Para_Done = 251;
		else
			run_parameter.Sensor_Fit_Para_Done = 0;

//	UARTprintf("test1=%lld\n",test1);
//	UARTprintf("test2=%lld\n",test2);
//	UARTprintf("test3=%lld\n",test3);
//	UARTprintf("test4=%lld\n",test4);
//	UARTprintf("test5=%lld\n",test5);
//	UARTprintf("test6=%lld\n",test6);
//	UARTprintf("test7=%lld\n",test7);
//	UARTprintf("test8=%lld\n",test8);
//	UARTprintf("test9=%lld\n",test9);
		
	UARTprintf("a1=%.7f\n",(double)test1/1000000000.0);
	UARTprintf("b1=%.7f\n",(double)test2/1000000000.0);
	UARTprintf("c1=%.7f\n",(double)test3/1000000000.0);

	UARTprintf("a2=%.7f\n",(double)test4/1000000000.0);
	UARTprintf("b2=%.7f\n",(double)test5/1000000000.0);
	UARTprintf("c2=%.7f\n",(double)test6/1000000000.0);

	UARTprintf("a3=%.7f\n",(double)test7/1000000000.0);
	UARTprintf("b3=%.7f\n",(double)test8/1000000000.0);
	UARTprintf("c3=%.7f\n",(double)test9/1000000000.0);
		
//	UARTprintf("a1=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[2]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[3])/10000000.0);
//	UARTprintf("b1=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[6]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[7])/10000000.0);
//	UARTprintf("c1=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[10]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_A.Fit_Para_A[11])/10000000.0);

//	UARTprintf("a2=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[2]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[3])/10000000.0);
//	UARTprintf("b2=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[6]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[7])/10000000.0);
//	UARTprintf("c2=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[10]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_B.Fit_Para_B[11])/10000000.0);

//	UARTprintf("a3=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[0]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[1]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[2]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[3])/10000000.0);
//	UARTprintf("b3=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[4]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[5]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[6]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[7])/10000000.0);
//	UARTprintf("c3=%.7f\n",(double)(( long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[8]<<48 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[9]<<32 | (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[10]<<16
//	| (unsigned long long int)run_parameter.Sensor_Fit_Para_C.Fit_Para_C[11])/10000000.0);
		
}

void E2C_Piecewise_point(void)
{
	unsigned int point0 = 0;
	unsigned int point1 = 0;
	unsigned int point2 = 0;
	unsigned int point3 = 0;
	
	point0 = run_parameter.Piecewise_point0.ubit.hi<<16 | run_parameter.Piecewise_point0.ubit.lo;
	point1 = run_parameter.Piecewise_point1.ubit.hi<<16 | run_parameter.Piecewise_point1.ubit.lo;
	point2 = run_parameter.Piecewise_point2.ubit.hi<<16 | run_parameter.Piecewise_point2.ubit.lo;
	point3 = run_parameter.Piecewise_point3.ubit.hi<<16 | run_parameter.Piecewise_point3.ubit.lo;
	
	e2prom512_write((unsigned char*)&run_parameter.Piecewise_point0.ubit.hi,4,243*2);
	e2prom512_write((unsigned char*)&run_parameter.Piecewise_point1.ubit.hi,4,245*2);
	e2prom512_write((unsigned char*)&run_parameter.Piecewise_point2.ubit.hi,4,247*2);
	e2prom512_write((unsigned char*)&run_parameter.Piecewise_point3.ubit.hi,4,249*2);
	
	e2prom512_read((unsigned char*)&run_parameter.Piecewise_point0.ubit.hi,4,243*2);
	e2prom512_read((unsigned char*)&run_parameter.Piecewise_point1.ubit.hi,4,245*2);
	e2prom512_read((unsigned char*)&run_parameter.Piecewise_point2.ubit.hi,4,247*2);
	e2prom512_read((unsigned char*)&run_parameter.Piecewise_point3.ubit.hi,4,249*2);

	if ((point0 == (run_parameter.Piecewise_point0.ubit.hi<<16 | run_parameter.Piecewise_point0.ubit.lo))
		 &&(point1 == (run_parameter.Piecewise_point1.ubit.hi<<16 | run_parameter.Piecewise_point1.ubit.lo))
	   &&(point2 == (run_parameter.Piecewise_point2.ubit.hi<<16 | run_parameter.Piecewise_point2.ubit.lo))
	   &&(point3 == (run_parameter.Piecewise_point3.ubit.hi<<16 | run_parameter.Piecewise_point3.ubit.lo)))
      run_parameter.Block_mark_Done = 202;
	else 
		  run_parameter.Block_mark_Done = 0;
	
//	UARTprintf("run_parameter.Block_mark_Done=%d\n",run_parameter.Block_mark_Done);
//	
//  UARTprintf("point0=%d\n",(run_parameter.Piecewise_point0.ubit.hi<<16 | run_parameter.Piecewise_point0.ubit.lo));
//  UARTprintf("point1=%d\n",(run_parameter.Piecewise_point1.ubit.hi<<16 | run_parameter.Piecewise_point1.ubit.lo));
//  UARTprintf("point2=%d\n",(run_parameter.Piecewise_point2.ubit.hi<<16 | run_parameter.Piecewise_point2.ubit.lo));
//  UARTprintf("point3=%d\n",(run_parameter.Piecewise_point3.ubit.hi<<16 | run_parameter.Piecewise_point3.ubit.lo));
	
	UARTprintf("point0=%.3f\n",(float)(run_parameter.Piecewise_point0.ubit.hi<<16 | run_parameter.Piecewise_point0.ubit.lo)/1000.0);
	UARTprintf("point1=%.3f\n",(float)(run_parameter.Piecewise_point1.ubit.hi<<16 | run_parameter.Piecewise_point1.ubit.lo)/1000.0);
	UARTprintf("point2=%.3f\n",(float)(run_parameter.Piecewise_point2.ubit.hi<<16 | run_parameter.Piecewise_point2.ubit.lo)/1000.0);
	UARTprintf("point3=%.3f\n",(float)(run_parameter.Piecewise_point3.ubit.hi<<16 | run_parameter.Piecewise_point3.ubit.lo)/1000.0);
}
