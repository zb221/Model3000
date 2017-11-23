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
	LED_RED_SET
	LED_BLUE_SET
	
	DelayNS(1);
	
	LED_RED_CLR
	LED_BLUE_CLR
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
	Relay1_Pin_SET
	Relay1_Pin_CLR
	
	Relay2_Pin_SET
	Relay2_Pin_CLR
	
	Relay3_Pin_SET
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
	  float Heat_V = 0;
    Heat_V = Heat_R_checkself();
	  if (Heat_V<200 && output_data.temperature>30){
		    UARTprintf("\r\n U23-AIN7=%.3f Heating resistance self-check error.\r\n",Heat_V);	
		}
}
