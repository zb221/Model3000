/***********************************************
@		Description: This file is main input for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/10.
***********************************************/

#include <stdio.h>                      /* standard I/O .h-file              */
#include <ctype.h>                      /* character functions               */
#include <LPC21xx.H>                    /* LPC21xx definitions               */

#include "main.h"                    /* global project definition file    */
#include "Peripherals_LPC2194.h"
#include "AD7738.h"
#include "DAC8568.h"

extern int flag1, flag2;
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

	init_peripherals();
	
	while (1)  
	{

		switch (flag1)  {
			case 1:
				//capture oil temp;
				Read_channel(1);
				break;

			case 2:
				//set 50 temp;
				DAC8568_INIT_SET(50,0xF000);
				break;
			
			case 3:
				//capture Temperature_of_resistance and Hydrogen_Resistance;
				Read_channel(1);
				Read_channel(2);
				break;
			
			case 4:
				//Stop heating;
				DAC8568_INIT_SET(0,0);
				break;
			
			default:                                 /* Error Handling              */
				break;
		}
		
		switch (flag2)  {
			case 1:
				//LED;
				break;

			case 2:
				//DS1390;
				break;
			
			case 3:
				//relay;
				break;
			
			case 4:
				//ADC;
				break;
						
			case 5:
				//DAC;
				break;
									
			case 6:
				//FLASH;
				break;
			
			case 7:
				//checkself;
		  break;
			
			default:                                 /* Error Handling              */
				break;
		}
	}
}
