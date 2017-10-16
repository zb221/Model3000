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
extern int flag1, flag2;
extern unsigned char rcv_buf[4096];
extern volatile unsigned char rcv_new;
extern unsigned int rcv_cnt;

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
	UARTprintf("model3000 test\n");
	
	while (1)  
	{
		
		if(rcv_new==1)//���Դ����շ�
		{
		  rcv_new=0;
			UART0_SendData(rcv_buf,rcv_cnt);
			memset(rcv_buf,0,rcv_cnt);
			rcv_cnt=0;
		}		
		switch (flag1)  {
			case 1:
				//capture oil temp;
				ADC7738_acquisition(1);
				ADC7738_acquisition_output(1);
				break;

			case 2:
				//set 50 temp;
				DAC8568_INIT_SET(50,0xF000);
				break;
			
			case 3:
				//capture Temperature_of_resistance and Hydrogen_Resistance;
				ADC7738_acquisition(1);
				ADC7738_acquisition_output(1);
				ADC7738_acquisition(2);
				ADC7738_acquisition_output(2);
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
				//200ms LED
			   UARTprintf("model3000 test led\n");
				break;

			case 2:
				//300ms ADC
				ADC7738_acquisition(1);
				ADC7738_acquisition(2);
				break;
			
			case 3:
				//600 ms checkself
				break;
			
			case 4:
				//800ms DS1390 
				break;
						
			case 5:
				//30min FLASH
				break;
									
			default:                                 /* Error Handling              */
				break;
		}
	}
}

