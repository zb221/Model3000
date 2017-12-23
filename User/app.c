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
	if (Heating_R_failure == 1){
		AD420_OUTPUT_SET((65535.0/20.0)*3.0); /*output error current 3mA*/
		LED_RED_SET
	  LED_BLUE_SET
	}
	
	if (output_data.OilTemp >= run_parameter.OilTemp_Alarm_celsius.hilo || output_data.DayROC >= run_parameter.h2_ppm_alarm_low_l16.hilo || output_data.H2DG >= run_parameter.h2_ppm_alert_low_l16.hilo) /* OilTemp  too high, DGA over, DGA dayROC over */
	{
		AD420_OUTPUT_SET((65535.0/20.0)*3.0); /*output error current 3mA*/
		if (LED_RED_FLAG%2 == 0)
			LED_RED_SET
		else
			LED_RED_CLR
		LED_BLUE_SET
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

	if (output_data.OilTemp >= run_parameter.OilTemp_Alarm_celsius.hilo)
	  Relay3_Pin_SET
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
	  Heating_R_failure = 0;
	  if (Intermediate_Data.Heat_V<200 && output_data.temperature>30){
//					UARTprintf("%.3f Heating resistance self-check error.\n",Intermediate_Data.Heat_V);
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
