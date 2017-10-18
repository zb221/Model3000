/***********************************************
@		Description: This file is about AD7738 data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/

#include "Peripherals_LPC2194.h"
#include "M25P16_FLASH.h"
/***********************************************************
Function:	 M25P16 CS pin init.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_CS_INIT(void)
{
	PINSEL1 = PINSEL1 & (~(0x03 << 18));        /*RD1->P0.25*/	
	IODIR0 = IODIR0 | 0x1<<25;								
	IOSET0 = IOSET0 | 0x1<<25;
}


