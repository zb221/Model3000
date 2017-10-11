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

int counter = 0, flag1 = 0, flag2 = 0;
/***********************************************************
Function:	timer default interrupt.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: timer default interrupt.
***********************************************************/
void DefISR (void) __irq  {
	printf("This is Test 1\n");
}

/***********************************************************
Function:	timer 0 interrupt.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: timer 0 interrupt.
***********************************************************/
__irq void TC0_IR (void) 
{
//printf("This is Test 2\n");
	
	counter++;
	switch (counter){
		case 40://capture oil temp
			flag1 = 1;
		break;
		
		case 50://set 50 temp
			flag1 = 2;
		break;
		
		case 3600://capture Temperature_of_resistance and Hydrogen_Resistance;Stop heating
			flag1 = 3;
		break;
		
		case 5400://stop heating
			flag1 = 4;
		break;
		
		case 7200://capture oil temp
			flag1 = 5;
		break;
		
		case 8000://capture oil temp
			counter = 0;
		break;
		
		default:
			break;
	}
	
	if (counter%5 == 0)
		flag2 = 1;//LED
	if (counter%10 == 0)
		flag2 = 2;//DS1390 
	else if (counter%15 == 0)
		flag2 = 3;//relay
	else if (counter%30 == 0)
		flag2 = 4;//checkself

  T0IR = 1;                                    /* Clear interrupt flag        */
  VICVectAddr = 0;                             /* Acknowledge Interrupt       */
}

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
	T0MR0 = 14999*1000;                          /* 1mSec = 15.000-1 counts     */
	T0MCR = 3;                                   /* Interrupt and Reset on MR0  */
	T0TCR = 1;                                   /* Timer0 Enable               */
	VICVectAddr0 = (unsigned long)TC0_IR;        /* set interrupt vector in 0   */
	VICVectCntl0 = 0x20 | 4;                     /* use it for Timer 0 Interrupt*/
	VICIntEnable = 0x00000010;                   /* Enable Timer0 Interrupt     */
	VICDefVectAddr = (unsigned long) DefISR;     /* un-assigned VIC interrupts  */
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
				break;

			case 2:
				//set 50 temp;
				break;
			
			case 3:
				//capture Temperature_of_resistance and Hydrogen_Resistance;
				break;
			
			case 4:
				//Stop heating;
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
				//checkself;
		  break;
			
			default:                                 /* Error Handling              */
				break;
		}
	}
}
