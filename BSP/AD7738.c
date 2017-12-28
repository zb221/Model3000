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
#include "fitting.h"
#include "parameter.h"
#include "DAC8568.h"

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
#define FW	2

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

/*-------------------------Global variable region----------------------*/
float AD7738_resolution_NP25 = 8388607/2500;
float AD7738_resolution_NP_125 = 8388607/1250;
float AD7738_resolution_NP_0625 = 8388607/625;

float Current_of_Temperature_resistance = 5;
float Current_of_Hydrogen_Resistance = 0.75;

unsigned int Channel_OilTemp = 0;
unsigned int Channel_H2Resistor = 0;
unsigned int Channel_PcbTemp = 0;

/***********************************************************
Function:	 U23 AD7738 CS pin init.
Input:	none
Output: none
Author: zhuobin
Date: 2017/11/22
Description: P1.23 and P1.21 is output port.
***********************************************************/
void U23_AD7738_CS_INIT(void)
{	
	IO1DIR |= (1<<23); /*CS2->P1.23*/
	IO1SET |= (1<<23);
	
	IO1DIR|=(1<<21);	/*RDY2->P1.21*/		
	IO1SET|=(1<<21);
}

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
Function:	 AVERAGE for arry.
Input: float *arry.
Output: none
Author: zhuobin
Date: 2017/10/31
Description: .
***********************************************************/
float AVERAGE_F(float *p)
{
	unsigned int i = 0, number = 0;
	float sum = 0;

	if (p == Intermediate_Data.H2Resistor_Tmp_1){
		number = sizeof(Intermediate_Data.H2Resistor_Tmp_1)/sizeof(Intermediate_Data.H2Resistor_Tmp_1[0]);
		for (i=0;i<number;i++)
		{
			sum += Intermediate_Data.H2Resistor_Tmp_1[i];
		}
		sum = sum / number;
	}else if (p == Intermediate_Data.H2G_tmp){
		number = sizeof(Intermediate_Data.H2G_tmp)/sizeof(Intermediate_Data.H2G_tmp[0]);
		for (i=0;i<number;i++)
		{
			sum += Intermediate_Data.H2G_tmp[i];
		}
		sum = sum / number;
	}
	return sum;
}

/***********************************************************
Function:	 sort for arry.
Input: float *arry.
Output: none
Author: zhuobin
Date: 2017/11/13
Description: .
***********************************************************/
void sortB(float *arry)
{
	unsigned int i = 0, j = 0, length = 0;
	float temp = 0;

	if (arry == Intermediate_Data.H2Resistor_Tmp_2){
		length = sizeof(Intermediate_Data.H2Resistor_Tmp_2)/sizeof(Intermediate_Data.H2Resistor_Tmp_2[0]);
//		for(i = 0; i < length; i++)
//		{
//		    UARTprintf("%.3f\n",Intermediate_Data.H2Resistor_Tmp_2[i]);
//		}
//		UARTprintf("\n");
		for(i = 0; i < length; ++i)
		{
			for(j = i + 1; j < length; ++j){

				if(Intermediate_Data.H2Resistor_Tmp_2[j] < Intermediate_Data.H2Resistor_Tmp_2[i]){

					temp = Intermediate_Data.H2Resistor_Tmp_2[i];

					Intermediate_Data.H2Resistor_Tmp_2[i] = Intermediate_Data.H2Resistor_Tmp_2[j];

					Intermediate_Data.H2Resistor_Tmp_2[j] = temp;

				}
			}
		}
//		for(i = 0; i < length; i++)
//		{
//		    UARTprintf("%.3f\n",Intermediate_Data.H2Resistor_Tmp_2[i]);
//		}
	}
}

/***********************************************************
Function:	 filter for arry.
Input: *arry.
Output: none
Author: zhuobin
Date: 2017/11/01
Description: .
***********************************************************/
void filterA(float *arry)
{
	unsigned int i = 0, number = 0, effective = 1;
	float sum = 0;

	sortB(arry);

	if (arry == Intermediate_Data.H2Resistor_Tmp_2){
		number = sizeof(Intermediate_Data.H2Resistor_Tmp_2)/sizeof(Intermediate_Data.H2Resistor_Tmp_2[0]);
		for (i=(number/2)-effective;i<(number/2)+effective;i++)
		{
			sum += Intermediate_Data.H2Resistor_Tmp_2[i];

		}

		if (((output_data.MODEL_TYPE == 1)||(output_data.MODEL_TYPE == 2))&&(Intermediate_Data.Start_print_H2R == 1)){
		  output_data.H2Resistor = sum / (2*effective);
		}
		if ((output_data.MODEL_TYPE == 3)&&(Intermediate_Data.Start_print_calibrate_H2R == 2)){
		  output_data.H2Resistor = sum / (2*effective);
		}
	}
}

/***********************************************************
Function:	 get Parameter form Temperature_of_resistance .
Input: three char data of 24bit data.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void Temperature_of_resistance_Parameter(void)
{	
	static unsigned int number = 0;
	static unsigned char flag = 0, run_type = 0;
	
	if (flag == 0){
	    Line_Fit(Intermediate_Data.Temp_R, Intermediate_Data.Temp);
	    flag = 1;
	}

	output_data.TempResistor = (Channel_OilTemp/AD7738_resolution_NP25-2500)/Current_of_Temperature_resistance;

	switch (output_data.temperature){
		case 0:
	    output_data.SensorTemp = Intermediate_Data.Temp_R_K*output_data.TempResistor + Intermediate_Data.Temp_R_B;
			output_data.OilTemp = output_data.SensorTemp;
			run_parameter.status_flag.ubit.senser_state0=0;
			run_parameter.status_flag.ubit.senser_state1=1;
			run_parameter.status_flag.ubit.senser_state2=0;
			break;
		
		case 50:
			if (output_data.MODEL_TYPE == 3){
				output_data.SensorTemp = Intermediate_Data.Temp_R_K*output_data.TempResistor + Intermediate_Data.Temp_R_B;
				break;
			}
		case 70:
	    output_data.SensorTemp = Intermediate_Data.Temp_R_K*output_data.TempResistor + Intermediate_Data.Temp_R_B;
		  if ((output_data.SensorTemp > (output_data.temperature + 2)) && Intermediate_Data.wait_1min == 1){
				output_data.OilTemp = output_data.SensorTemp;
				output_data.MODEL_TYPE = 4;
				DAC8568_INIT_SET(0,0*65536/5);	/* The oil temperature exceeds the working temperature*/
				Intermediate_Data.Operat_temp_alarm = 1;
			}else{
        output_data.MODEL_TYPE = 1;
			}
			if (output_data.OilTemp < output_data.temperature){
			  Intermediate_Data.Operat_temp_alarm = 0;
			}
			break;
		
		default:
			break;
	}
	Intermediate_Data.OilTemp_Tmp[number++] = Intermediate_Data.Temp_R_K*output_data.TempResistor + Intermediate_Data.Temp_R_B;
//	UARTprintf("%.4f	",Intermediate_Data.OilTemp_Tmp[number-1]);
	if (number == sizeof(Intermediate_Data.OilTemp_Tmp)/sizeof(Intermediate_Data.OilTemp_Tmp[0])){
		number = 0;
	}
}

/***********************************************************
Function:	 get Parameter form Hydrogen_Resistance .
Input: three char data of 24bit data.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void Hydrogen_Resistance_Parameter(void)
{
	static unsigned int number1 = 0;
	
	Intermediate_Data.H2Resistor_Tmp[number1++] = (Channel_H2Resistor/AD7738_resolution_NP25-2500)/Current_of_Hydrogen_Resistance;
//	UARTprintf("%.4f\n",Intermediate_Data.H2Resistor_Tmp[number1-1]);
	if (number1 == sizeof(Intermediate_Data.H2Resistor_Tmp)/sizeof(Intermediate_Data.H2Resistor_Tmp[0])){
		number1 = 0;
	}

}

/***********************************************************
Function:	 PCB TEMP.
Input: three char data of 24bit data.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void PCB_temp_Parameter(void)
{
	/* m*t*t+n*t+1-R/1000=0  n=0.0038623139728, m=-0.00000065314932626*/
	float n = 0.0038623139728, m = -0.00000065314932626, R = 0, PT1000_current = 0.125;
	
	R = (Channel_PcbTemp/AD7738_resolution_NP25-2500)/PT1000_current;
	output_data.PcbTemp = (-n + sqrt(n*n-4*m*(1-R/1000)))/(2*m);
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
	static unsigned int number2 = 0;
	unsigned char flag = 0;
	unsigned int temp = 0, count1 = 0, one_time = 4;
	unsigned char data0 = 0, data1 = 0, data2 = 0;	/* The raw data output from ADC */

	AD7738_write(channel_setup_0 + channel,0<<7|AINx_AINx|0<<4|Channel_Continuous_conversion_disable|NP_25);	/*Channel Setup Registers:BUF_OFF<<7|COM1|COM0|Stat|Channel_CCM|RNG2_0*/
	AD7738_write(channel_conv_time_0 + channel,Chop_Enable|FW);	/*channel coversion time*/
	for (count1=0;count1<one_time;count1++){
		AD7738_write(channel_mode_0 + channel,Single_Conversion_Mode|1<<4|0<<3|0<<2|BIT24|1);
		flag = 0;
		while(!((flag>>channel)&0x1)){
			AD7738_read(ADC_STATUS_REG,&flag);
		}
		
		AD7738_read_channel_data(channel_data_0 + channel,&data0,&data1,&data2);
		temp += (data0<<16|data1<<8|data2);
	}
	/*---------------------control the temp of sense-------------------------*/
	temp = temp/one_time;
	data0 = (temp>>16)&0xff;
	data1 = (temp>>8)&0xff;
	data2 = (temp>>0)&0xff;

	switch (channel){
		case 1:
		Channel_OilTemp = (data0<<16|data1<<8|data2);
		Temperature_of_resistance_Parameter();
		break;

		case 2:
 		Channel_H2Resistor = (data0<<16|data1<<8|data2);
		Hydrogen_Resistance_Parameter();
		
		Line_Fit(Intermediate_Data.OilTemp_Tmp,Intermediate_Data.H2Resistor_Tmp);
		Intermediate_Data.H2Resistor_Tmp_1[number2++] = Intermediate_Data.H2Resistor_OilTemp_K*output_data.temperature + Intermediate_Data.H2Resistor_OilTemp_B;
		if (number2 == sizeof(Intermediate_Data.H2Resistor_Tmp_1)/sizeof(Intermediate_Data.H2Resistor_Tmp_1[0])){
		    number2 = 0;
		}
		break;

		case 3:
 		Channel_PcbTemp = (data0<<16|data1<<8|data2);
		break;

		default: break;
	}

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
	static unsigned char number = 0;
	static unsigned int number3 = 0;
	
	switch (channel){
		case 1:
		break;

		case 2:
		Intermediate_Data.H2Resistor_Tmp_2[number3++] = AVERAGE_F(Intermediate_Data.H2Resistor_Tmp_1); /*H2Resistor 1*/
		if (number3 == sizeof(Intermediate_Data.H2Resistor_Tmp_2)/sizeof(Intermediate_Data.H2Resistor_Tmp_2[0])){
		    number3 = 0;
		}
		filterA(Intermediate_Data.H2Resistor_Tmp_2); /*H2Resistor 2*/
	
		if (number == 0)
		number = sizeof(Intermediate_Data.OHM)/sizeof(Intermediate_Data.OHM[0]);

		if (Intermediate_Data.Operat_temp_alarm == 0){
			if (output_data.temperature == 50 && Intermediate_Data.wait_1min == 1){
				if(output_data.H2Resistor < Intermediate_Data.OHM[0]){
					output_data.H2AG = Intermediate_Data.H2[0];
					output_data.H2AG1 = Intermediate_Data.H2[0];
				}else if (output_data.H2Resistor > Intermediate_Data.OHM[number-1]){
					output_data.H2AG = Intermediate_Data.H2[number-1];
					output_data.H2AG1 = Intermediate_Data.H2[number-1];
				}else{
					output_data.H2AG = Cubic_main(output_data.H2Resistor,Hydrogen_Res);  /*H2AG*/
					output_data.H2AG1 = quadratic_polynomial(output_data.H2Resistor);
				}
				run_parameter.status_flag.ubit.senser_state0=1;
				run_parameter.status_flag.ubit.senser_state1=0;
				run_parameter.status_flag.ubit.senser_state2=0;
			}else{
				run_parameter.status_flag.ubit.senser_state0=1;
				run_parameter.status_flag.ubit.senser_state1=1;
				run_parameter.status_flag.ubit.senser_state2=0;
			}
	  }else{
			run_parameter.status_flag.ubit.senser_state0=1;
			run_parameter.status_flag.ubit.senser_state1=0;
			run_parameter.status_flag.ubit.senser_state2=1;
		}
		output_data.H2DG = output_data.H2AG / 20;
		output_data.H2DG += (float)((run_parameter.h2_ppm_calibration_gas_h16.hilo << 16) | run_parameter.h2_ppm_calibration_gas_l16.hilo);
		output_data.H2G = output_data.H2DG;
		break;

		case 3:
		PCB_temp_Parameter();
		break;

		default: break;
	}
}

