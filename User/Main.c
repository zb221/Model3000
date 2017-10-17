/***********************************************
@		Description: This file is main input for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/10.
***********************************************/

#include <stdio.h>                      /* standard I/O .h-file              */
#include <ctype.h>                      /* character functions               */
#include <LPC21xx.H>                    /* LPC21xx definitions               */
#include <string.h>

#include "main.h"                    /* global project definition file    */
#include "config.h"
#include "Peripherals_LPC2194.h"
#include "AD7738.h"
#include "DAC8568.h"

/*-------------------------Global variable region----------------------*/
extern int flag1, flag2, flag3;

/***********************************************************
Function:	init peripherals.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: setup the timer counter 0 interrupt.
***********************************************************/
void init_peripherals(void)
{
	init_serial();
	init_timer();
	init_PWM();
	SPI0_INIT();
	AD7738_CS_INIT();
	DAC8568_CS_INIT();
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
	FrecInit();
	init_peripherals();

	while (1)  
	{
		switch (flag1)  {
			case 1:
			//capture oil temp;
			ADC7738_acquisition(1);
			ADC7738_acquisition_output(1);
			flag1 = 0;
			UARTprintf("capture oil temp\n");
			break;

			case 2:
			//set 50 temp;
			DAC8568_INIT_SET(50,0xF000);
			flag1 = 0;
			UARTprintf("set 50 temp\n");
			break;

			case 3:
			//capture Temperature_of_resistance and Hydrogen_Resistance;
			ADC7738_acquisition(1);
			ADC7738_acquisition_output(1);
			ADC7738_acquisition(2);
			ADC7738_acquisition_output(2);
			flag1 = 0;
			UARTprintf("capture Temperature_of_resistance and Hydrogen_Resistance\n");
			break;

			case 4:
			//Stop heating;
			DAC8568_INIT_SET(0,0);
			flag1 = 0;
			UARTprintf("Stop heating\n");
			break;

			default:                                 /* Error Handling              */
			break;
		}

		switch (flag2)  {
			case 1:
			//200ms LED
			UARTprintf("200ms LED\n");
			flag2 = 0;
			break;

			case 2:
			//300ms ADC
			UARTprintf("300ms ADC\n");
			ADC7738_acquisition(1);
			ADC7738_acquisition(2);
			flag2 = 0;
			break;

			case 3:
			//600 ms checkself
			UARTprintf("600 ms checkself\n");
			flag2 = 0;
			break;

			case 4:
			//800ms DS1390 
			UARTprintf("800ms DS1390\n");
			flag2 = 0;
			break;

			default:                                 /* Error Handling              */
			break;
		}

		if (flag3 == 1)
		{
			//30min FLASH
			UARTprintf("30min FLASH\n");
			flag3 = 0;
		}
	}
}

