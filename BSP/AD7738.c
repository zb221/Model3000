/***********************************************
@		Description: This file is about AD7738 data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions               */
#include <stdio.h>
#include <math.h>
#include "Peripherals_LPC2194.h"
#include "AD7738.h"
#include "Cubic.h"

/***********************************************************
Description: Global variable region.
Author: zhuobin
Date: 2017/10/10
***********************************************************/
#define IO_PORT_REG          0x01

#define ADC_STATUS_REG    0x04

enum {
	channel_data_0 = 0x08,
	channel_data_1,
	channel_data_2,
	channel_data_3,
	channel_data_4,
	channel_data_5,
	channel_data_6,
	channel_data_7
}CHANNEL_DATA_REG_ADDR;

enum {
	channel_status_0 = 0x20,
	channel_status_1,
	channel_status_2,
	channel_status_3,
	channel_status_4,
	channel_status_5,
	channel_status_6,
	channel_status_7
}CHANNEL_STATUS_REG_ADDR;

/*-----------------------Channel Setup---------------------------*/
enum {
	channel_setup_0 = 0x28,
	channel_setup_1,
	channel_setup_2,
	channel_setup_3,
	channel_setup_4,
	channel_setup_5,
	channel_setup_6,
	channel_setup_7
}CHANNEL_SETUP_REG_ADDR;
#define AINx_AINx                        (0x3<<5)
#define AINx_AINCOM                   (0x0<<5)
#define Status_Option                   (0<<4)
#define Channel_Continuous_conversion_enable (1<<3)
#define Channel_Continuous_conversion_disable (0<<3)
#define NP_125  0x00
#define P_125   0x01
#define NP_0625 0x02
#define P_0625  0x03
#define NP_25   0x04
#define P_25    0x05

/*----------------Channel Conversion Time Registers--------------*/
enum {
	channel_conv_time_0 = 0x30,
	channel_conv_time_1,
	channel_conv_time_2,
	channel_conv_time_3,
	channel_conv_time_4,
	channel_conv_time_5,
	channel_conv_time_6,
	channel_conv_time_7
}CHANNEL_CONV_TIME_REG_ADDR;
#define Chop_Enable		(TURE<<7)
#define FW	17

/*-----------------Mode Register-----------------------------*/
enum {
	channel_mode_0 = 0x38,
	channel_mode_1,
	channel_mode_2,
	channel_mode_3,
	channel_mode_4,
	channel_mode_5,
	channel_mode_6,
	channel_mode_7
}CHANNEL_MODE_REG_ADDR;
#define Idle_Mode							(0<<5)
#define Continues_Conversion_Mode			(1<<5)
#define Single_Conversion_Mode				(2<<5)
#define Power_Down_Mode					(3<<5)
#define ADC_Zero_Scale_Self_Calibration		(4<<5)
#define Channel_Zero_Scale_System_Calibration	(6<<5)
#define Channel_Full_Scale_System_Calibration		(7<<5)
#define BIT24		(1<<1)
#define BIT16		(0<<1)

float AD7738_resolution = 8388607/2500;
float AD7738_resolution_NP_125 = 8388607/1250;
float AD7738_resolution_NP_0625 = 8388607/625;

float Current_of_Temperature_resistance = 5;
float Current_of_Hydrogen_Resistance = 0.75;

/*-------------------------Global variable region----------------------*/
unsigned char data0 = 0, data1 = 0, data2 = 0;
float OilTemp = 0;
float PcbTemp = 0;
float H2_SENSE_Resistance = 0;

/***********************************************************
Function:	 AD7738 CS pin init.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void AD7738_CS_INIT(void)
{
	PINSEL1 = PINSEL1 & (~(0x03 << 8));       /*CS1/SSEL1->P0.20*/	
	IODIR0 = IODIR0 | 0x1<<20;								
	IOSET0 = IOSET0 | 0x1<<20;
	
	PINSEL0 = PINSEL0 & (~(0x03 << 30));     /*RDY1->P0.15*/
	IODIR0 = IODIR0 | 0x0<<15;								
 	IOCLR0 = IOCLR0 | 0x1<<15;
}

/***********************************************************
Function:	 AD7738 WRITE.
Input: Register data
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void AD7738_write(unsigned char Register,unsigned char data)
{
	IOCLR0 = IOCLR0 | 0x1<<20;                  /*CS1/SSEL1 set LOW*/
	
	SPI0_SendDate(0<<6|(0x3F & Register));

 	SPI0_SendDate(data);
	
	IOSET0 = IOSET0 | 0x1<<20;                /*CS1/SSEL1 set HIGHT*/
}

/***********************************************************
Function:	 AD7738 read.
Input: Register and data to accpet.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void AD7738_read(unsigned char Register,unsigned char *data)
{
	IOCLR0 = IOCLR0 | 0x1<<20;		/*CS1/SSEL1 set LOW*/
	
	SPI0_SendDate(1<<6|(0x3F & Register));
 	*data = SPI0_SendDate(0x00);
	
	IOSET0 = IOSET0 | 0x1<<20;           /*CS1/SSEL1 set HIGHT*/
}

/***********************************************************
Function:	 AD7738 read channel data.
Input: Register and three buffer to accpet 24bit data.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void AD7738_read_channel_data(unsigned char Register,unsigned char *buf0,unsigned char *buf1,unsigned char *buf2)
{
	*buf0 = 0;
	*buf1 = 0;
	*buf2 = 0;
	
	IOCLR0 = IOCLR0 | 0x1<<20;           /* CS1/SSEL1 set LOW*/

	SPI0_SendDate(1<<6|(0x3F & Register));

 	*buf0 = SPI0_SendDate(0xFF);

	*buf1 = SPI0_SendDate(0xFF);
	
	*buf2 = SPI0_SendDate(0xFF);
	
	IOSET0 = IOSET0 | 0x1<<20;          /* CS1/SSEL1 set HIGHT*/
}

/***********************************************************
Function:	 AD7738 set.
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void AD7738_SET(void)
{
	unsigned char IO_Port_Reg = 0;
	
	IOCLR0 = IOCLR0 | 0x1<<20;         /* CS1/SSEL1 set LOW */
	SPI0_SendDate(0x00);               /* RESET AD7738 */
	SPI0_SendDate(0xFF);
	SPI0_SendDate(0xFF);
	SPI0_SendDate(0xFF);
	SPI0_SendDate(0xFF);
	IOSET0 = IOSET0 | 0x1<<20;       /* CS1/SSEL1 set HIGHT */
	
	DelayNS(100);
	
/*-----------------------------------------------set common register of AD7738-----------------------------------------------------*/
	AD7738_read(IO_PORT_REG,&IO_Port_Reg);
	AD7738_write(IO_PORT_REG,IO_Port_Reg & (~(1<<3)));		/*0: the RDY pin will only go low if any, 1: the RDY pin will only go low if all enabled channels have unread data*/

/*-----------------------------------------------set channel 1 register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_setup_1,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_enable|NP_25);	/*Channel_1 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_1,Chop_Enable|FW);	/*channel coversion time*/

/*-----------------------------------------------set channel 2 register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_setup_2,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_enable|NP_25);	/*Channel_2 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_2,Chop_Enable|FW);	/*channel coversion time*/

/*-----------------------------------------------set channel 3 register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_setup_3,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_enable|NP_25);	/*Channel_3 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_3,Chop_Enable|FW);	/*channel coversion time*/

/*-----------------------------------------------set Mode register of AD7738-----------------------------------------------------*/
	AD7738_write(channel_mode_1,Continues_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);		/*Mode Register: Mod2_0|CLKDIS|DUMP|CONT_RD|24_16|CLAMP*/
	AD7738_write(channel_mode_2,Continues_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);		/*Mode Register: Mod2_0|CLKDIS|DUMP|CONT_RD|24_16|CLAMP*/
	AD7738_write(channel_mode_3,Continues_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);		/*Mode Register: Mod2_0|CLKDIS|DUMP|CONT_RD|24_16|CLAMP*/

}

/***********************************************************
Function:	 get Parameter form Temperature_of_resistance .
Input: three char data of 24bit data.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void Temperature_of_resistance_Parameter(unsigned char A,unsigned char B,unsigned char C)
{	
	float resistance = 0;

	float Temp_resistance_slope = 0, x = 0, y = 0;

	Linear_slope(&Temp_resistance_slope, &x, &y, Temp_Res);
	
	resistance = ((A<<16|B<<8|C)/AD7738_resolution-2500)/Current_of_Temperature_resistance;

	OilTemp = (resistance-(y-Temp_resistance_slope*x))/Temp_resistance_slope;
}

/***********************************************************
Function:	 get Parameter form Hydrogen_Resistance .
Input: three char data of 24bit data.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void Hydrogen_Resistance_Parameter(unsigned char A,unsigned char B,unsigned char C)
{
	H2_SENSE_Resistance = ((A<<16|B<<8|C)/AD7738_resolution-2500)/Current_of_Hydrogen_Resistance;
	
//	Temp = Cubic_main(resistance,Hydrogen_Res);
//	R = Hydrogen_resistance_slope*temperature + (645-65*Hydrogen_resistance_slope);
}

/***********************************************************
Function:	 PCB TEMP.
Input: three char data of 24bit data.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void PCB_temp_Parameter(unsigned char A,unsigned char B,unsigned char C)
{
	/* m*t*t+n*t+1-R/1000=0  n=0.0038623139728, m=-0.00000065314932626*/
	float n = 0.0038623139728, m = -0.00000065314932626, R = 0, PT1000_current = 0.125;
	
	R = ((data0<<16|data1<<8|data2)/AD7738_resolution-2500)/PT1000_current;
	PcbTemp = (-n + sqrt(n*n-4*m*(1-R/1000)))/(2*m);
}

/***********************************************************
Function:	 AD7738 acquisition channel DATA.
Input: channel.
Output: none
Author: zhuobin
Date: 2017/10/10
Description:  accept three char data.
***********************************************************/
void ADC7738_acquisition(unsigned char channel)
{
	unsigned char flag = 0;
	unsigned int temp = 0, count1 = 0;

	for (count1=0;count1<10;count1++){
		/*--------------------------------------------set channel register of AD7738--------------------------------------------*/
		AD7738_write(channel_setup_0 + channel,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_disable|NP_25);	/*Channel_1 Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
		AD7738_write(channel_conv_time_0 + channel,Chop_Enable|FW);	/*channel coversion time*/
		AD7738_write(channel_mode_0 + channel,Single_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);

		flag = 0;
		while(!((flag>>channel)&0x1)){
			AD7738_read(ADC_STATUS_REG,&flag);
		}
		
		AD7738_read_channel_data(channel_data_0 + channel,&data0,&data1,&data2);
		temp += (data0<<16|data1<<8|data2);
	}
	/*---------------------control the temp of sense-------------------------*/
	temp = temp/10;
	data0 = (temp>>16)&0xff;
	data1 = (temp>>8)&0xff;
	data2 = (temp>>0)&0xff;

}

/***********************************************************
Function:	 AD7738 acquisition channel DATA and Calculated output.
Input: none.
Output: none
Author: zhuobin
Date: 2017/10/10
Description:  .
***********************************************************/
void ADC7738_acquisition_output(unsigned char channel)
{
	switch (channel){
		case 1:
		Temperature_of_resistance_Parameter(data0,data1,data2);
		break;

		case 2:
		Hydrogen_Resistance_Parameter(data0,data1,data2);
		break;

		case 3:
		PCB_temp_Parameter(data0,data1,data2);
		break;

		default: break;
	}
}

