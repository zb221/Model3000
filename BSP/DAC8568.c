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
#include "fitting.h"

extern float Din_temp_DAC_Din_K;
extern float Din_temp_DAC_Din_B;
extern float DAC_Din[5];
extern float Din_temp[5];

extern float PCB_TEMP_Din_K;
extern float PCB_TEMP_Din_B;
extern float PCB_TEMP_Din[3];
extern float PCB_TEMP_SET[3];
/***********************************************************
Function:	init DAC8568 CS PIN.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void DAC8568_CS_INIT(void)
{
	PINSEL0 = PINSEL0 & (~(0x03 << 20));			/*/SYNC->P0[10]*/
	IODIR0 = IODIR0 | 0x1<<10;								
	IOSET0 = IOSET0 | 0x1<<10;
	
	PINSEL0 = PINSEL0 & (~(0x03 << 24));			/*LDAC->P0[12]*/
	IODIR0 = IODIR0 | 0x1<<12;								
	IOSET0 = IOSET0 | 0x1<<12;
}

/***********************************************************
Function:	 DAC8568 SET.
Input: data to set.
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void DAC8568_SET(unsigned char PB,unsigned char CB,unsigned char AB,unsigned short DB,unsigned char FB)
{	
	IOCLR0 = IOCLR0 | 0x1<<10;              /*/SYNC  SET LOW */
	
	SPI0_SendDate(PB<<4|CB);
	SPI0_SendDate(AB<<4|DB>>12);
	SPI0_SendDate(0xFF & (DB>>4));
	SPI0_SendDate((0xF & DB)<<4 | FB);
	
	IOSET0 = IOSET0 | 0x1<<10;            /*/SYNC  SET HIGHT */
	DelayNS(10);
	IOCLR0 = IOCLR0 | 0x1<<12;            /*/LDAC set LOW*/
	DelayNS(10);
	IOSET0 = IOSET0 | 0x1<<12;           /*/LDAC set HIGHT*/
}

/***********************************************************
Function:	Calculate the output voltage of DAC8568 corresponding to the resolution.
Input: variables to saves the calculation results
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void DAC_SET_Chanel_Din(float temperature,int *DAC_DIN, unsigned char type)
{
	switch (type){
		case DAC_temp:
		Line_Fit(Din_temp,DAC_Din);
		*DAC_DIN = Din_temp_DAC_Din_K*temperature + Din_temp_DAC_Din_B;
//		UARTprintf("DAC_DIN2=%d\n",*DAC_DIN);
		break;
		case PCB_TEMP:
		Line_Fit(PCB_TEMP_SET, PCB_TEMP_Din);
		*DAC_DIN = PCB_TEMP_Din_K*temperature + PCB_TEMP_Din_B;
//		UARTprintf("DAC_DIN4=%d\n",*DAC_DIN);
		break;
		
		default:
			break;
	}
}

/***********************************************************
Function:	DAC8568 INIT SET.
Input: temperature and current want to be set
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void DAC8568_INIT_SET(float temperature,float current)
{
	int DAC_G_Din = 0;
	DAC_SET_Chanel_Din(temperature,&DAC_G_Din,DAC_temp);     /* set sense want temp value */

	DAC8568_SET(0x0,0x9,0x0,0xA000,0);		        /* Power up internal reference all the time regardless DAC states */
	
	DAC8568_SET(0x0,0x3,0x2,current,0);		       /* DAC-C */
	DAC8568_SET(0x0,0x3,0x6,DAC_G_Din,0);		       /* DAC-G */
	UARTprintf("DAC_G_Din=%d\n",DAC_G_Din);
}

/***********************************************************
Function:	DAC8568 PCB temp control.
Input: PCB control temperature and current want to be set
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void DAC8568_PCB_TEMP_SET(float PCB_temp,float current)
{
	int DAC_H_Din = 0;
	DAC_SET_Chanel_Din(PCB_temp,&DAC_H_Din,PCB_TEMP);     /* set PCB want temp value */

	DAC8568_SET(0x0,0x9,0x0,0xA000,0);		        /* Power up internal reference all the time regardless DAC states */

	DAC8568_SET(0x0,0x3,0x5,current,0);		/*DAC-F*/
	DAC8568_SET(0x0,0x3,0x7,DAC_H_Din,0);		/*DAC-H*/
}
