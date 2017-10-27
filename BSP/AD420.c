/***********************************************
@		Description: This file is about data of AD420.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions               */
#include "Peripherals_LPC2194.h"

/***********************************************************
Function:	init SPI0 for AD420.
Input:	none
Output: none
Author: zhuobin
Date: 2017/9/22
Description: 
***********************************************************/
void AD420_SPI_SET(void)
{
/*---------------------------------SPI0-----------------------------------------------*/
	PINSEL0 = (PINSEL0 & 0xFFFF00FF) | 0x15<<8; /*SPI0 PIN SEL*/
	S0SPCR = BitEnable_X | CPHA_first | CPOL_high | MSTR_master | LSBF_MSB_first | SPI_interrupt_disable | BITS_16;
	S0SPCCR = SPI0_CLK;
}

/***********************************************************
Function:	AD420 latch set.
Input:	value to select high or low for LATCH
Output: none
Author: zhuobin
Date: 2017/9/22
Description: AD420 latch data into device
***********************************************************/
void AD420_LATCH_SET(unsigned char value)
{
	PINSEL0 = PINSEL0 & (~(0x3 << 22));			/*/CTSI->P0[11]*/
	IODIR0 = IODIR0 | 0x1<<11;	
	
	if (value == 1)
		IOSET0 = IOSET0 | 0x1<<11;
	else if (value == 0)
		IOCLR0 = IOCLR0 | 0x1<<11;

}

/***********************************************************
Function:	SPI0 use 16Bit to send data .
Input:	data
Output: SPI0 recevie data from buffer
Author: zhuobin
Date: 2017/9/22
Description: 
***********************************************************/
unsigned short SPI0_SendDate_16Bit(unsigned short data)
{
	S0SPDR = data;			//Send date
	
	while ((S0SPSR & 0x80) == 0);

	return (S0SPDR);
}

/***********************************************************
Function:	set AD420 current and voltage output.
Input:	value-> (65535/20)*value
Output: none
Author: zhuobin
Date: 2017/9/22
Description: 16bit value -> 0-20mA output
***********************************************************/
void AD420_OUTPUT_SET(unsigned short value)
{
	AD420_LATCH_SET(0);
	SPI0_SendDate_16Bit(value);
//	DelayNS(1);
	AD420_LATCH_SET(1);
}
