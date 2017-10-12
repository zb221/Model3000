/***********************************************
@		Description: This file is about DAC8568.c data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions               */
#include <stdio.h>
#include "DAC8568.h"
#include "Cubic.h"
#include "Peripherals_LPC2194.h"

void DAC8568_CS_INIT(void)
{
	PINSEL0 = PINSEL0 & (~(0x03 << 20));			/*/SYNC->P0[10]*/
	IODIR0 = IODIR0 | 0x1<<10;								
	IOSET0 = IOSET0 | 0x1<<10;
	
	PINSEL0 = PINSEL0 & (~(0x03 << 24));			/*LDAC->P0[12]*/
	IODIR0 = IODIR0 | 0x1<<12;								
	IOSET0 = IOSET0 | 0x1<<12;
}



void DAC8568_SET(unsigned char PB,unsigned char CB,unsigned char AB,unsigned short DB,unsigned char FB)
{	
	IOCLR0 = IOCLR0 | 0x1<<10;	/*/SYNC  SET LOW */
	
	SPI0_SendDate(PB<<4|CB);
	SPI0_SendDate(AB<<4|DB>>12);
	SPI0_SendDate(0xFF & (DB>>4));
	SPI0_SendDate((0xF & DB)<<4 | FB);
	
	IOSET0 = IOSET0 | 0x1<<10;	/*/SYNC  SET HIGHT */
	DelayNS(100);
	IOCLR0 = IOCLR0 | 0x1<<12;  /*/LDAC set LOW*/
	DelayNS(100);
	IOSET0 = IOSET0 | 0x1<<12;  /*/LDAC set HIGHT*/
}


void DAC_SET_Chanel_Din(float temperature,int *DAC_DIN)
{
//	*DAC_DIN = 13107*Current_of_Temperature_resistance*(temperature*Temp_resistance_slope + (369.8-Temp_resistance_slope*65));

	float DAC_Din_Temp_slope = 0, x = 0, y = 0;

	Linear_slope(&DAC_Din_Temp_slope, &x, &y, 2);
//	printf("*DAC_Din_Temp_slope=%.3f,x=%.3f,y=%.3f\n",DAC_Din_Temp_slope,x,y);
	
	*DAC_DIN = (temperature - (y-(DAC_Din_Temp_slope*x)))/DAC_Din_Temp_slope;
	printf("*DAC_DIN=%d\n",*DAC_DIN);

}

void DAC8568_INIT_SET(float temperature,float current)
{
	int DAC_Din = 0;
	DAC_SET_Chanel_Din(temperature,&DAC_Din); /*set want temp value*/
	
	DAC8568_SET(0x0,0x9,0x0,0xA000,0);		/*Power up internal reference all the time regardless DAC states*/
	
	DAC8568_SET(0x0,0x3,0x2,current,0);		/*DAC-C*/
	DAC8568_SET(0x0,0x3,0x6,DAC_Din,0);		/*DAC-G*/

//	DAC8568_SET(0x0,0x3,0x5,0x1000,0);		/*DAC-F*/
//	DAC8568_SET(0x0,0x3,0x7,11336,0);		/*DAC-H*/
}

