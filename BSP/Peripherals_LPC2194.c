/***********************************************
@		Description: This file is about data of AD420.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions */
#include <stdio.h>
#include "Peripherals_LPC2194.h"
#include "config.h"

/***********************************************************
Description: General define.
Author: zhuobin
Date: 2017/10/10
***********************************************************/
#define CNTLQ           0x11
#define CNTLS           0x13
#define DEL               0x7F
#define BACKSPACE   0x08
#define CR                0x0D
#define LF                 0x0A

/***********************************************************
Description: Global variable region.
Author: zhuobin
Date: 2017/10/10
***********************************************************/
int counter = 0, flag1 = 0, flag2 = 0;

/***********************************************************
Function: Delay .
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description:  .
***********************************************************/
void DelayNS (unsigned int uiDly)
{
    unsigned int  i;
    
    for(; uiDly > 0; uiDly--){
        for(i = 0; i < 50000; i++);
    }
} 


/***********************************************************
Function:	setup system clock.
Input:	none
Output: none
Author: megzheng
Date: 2017/10/13
Description: setup system clock.
***********************************************************/
void FrecInit(void)
{
	PLLCON = 1;
		
	#if ((Fcclk / 4) / Fpclk) == 1
	VPBDIV = 0;
	#endif
	#if ((Fcclk / 4) / Fpclk) == 2
	VPBDIV = 2;
	#endif
	#if ((Fcclk / 4) / Fpclk) == 4
	VPBDIV = 1;
	 #endif
		
	#if (Fcco / Fcclk) == 2
	PLLCFG = ((Fcclk / Fosc) - 1) | (0 << 5);
	#endif
	#if (Fcco / Fcclk) == 4
	PLLCFG = ((Fcclk / Fosc) - 1) | (1 << 5);
	#endif
	 #if (Fcco / Fcclk) == 8
	PLLCFG = ((Fcclk / Fosc) - 1) | (2 << 5);
	#endif
	#if (Fcco / Fcclk) == 16
	PLLCFG = ((Fcclk / Fosc) - 1) | (3 << 5);
	 #endif
	PLLFEED = 0xaa;//发送PLL馈送序列
	PLLFEED = 0x55;
	while((PLLSTAT & (1 << 10)) == 0);//等待PLL锁定
	PLLCON = 3;
	PLLFEED = 0xaa;
	PLLFEED = 0x55; 
}

/***********************************************************
Function: Initialize Serial Interface(uart0/1).
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description:  .
***********************************************************/
void init_serial (void)  
{
  PINSEL0 = 0x00000000 | 0x01<<18 | 0x01<<16 | 0x01<<2 | 0x01<<0;                 /* Enable RxD1 and TxD1                                   */
  U1LCR = 0x83;                          											/* 8 bits, no Parity, 1 Stop bit   0x1<<7|0x11<<0  */
  U1DLL = 8;                      											      /* 9600 Baud Rate @ 15MHz VPB Clock                */
  U1LCR = 0x03;                        										      /* DLAB = 0                                                    */
}

/***********************************************************
Function: implementation of putchar (also used by printf function to output data).
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description:  .
***********************************************************/
int sendchar (int ch)  {                 /* Write character to Serial Port    */

  if (ch == '\n')  {
    while (!(U1LSR & 0x20));
    U1THR = 0x0D;                             /* output CR */
  }
  while (!(U1LSR & 0x20));
  return (U1THR = ch);
}

/***********************************************************
Function: .
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description:  .
***********************************************************/
int getkey (void)  {                    /* Read character from Serial Port   */

  while (!(U1LSR & 0x01));              /* wait until character ready        */
  return (U1RBR);
}

/***********************************************************
Function: .
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description:  .
***********************************************************/
void getline (char *line, int n)  
{
  int  cnt = 0;
  char c;

  do  {
    if ((c = getkey ()) == CR)  c = LF;     /* read character                 */
    if (c == BACKSPACE  ||  c == DEL)  {    /* process backspace              */
      if (cnt != 0)  {
        cnt--;                              /* decrement count                */
        line--;                             /* and line pointer               */
        putchar (BACKSPACE);                /* echo backspace                 */
        putchar (' ');
        putchar (BACKSPACE);
      }
    }
    else if (c != CNTLQ && c != CNTLS)  {   /* ignore Control S/Q             */
      putchar (*line = c);                  /* echo and store character       */
      line++;                               /* increment line pointer         */
      cnt++;                                /* and count                      */
    }
  }  while (cnt < n - 1  &&  c != LF);      /* check limit and line feed      */
  *(line - 1) = 0;                          /* mark end of string             */
}


/***********************************************************
Function: Initialize SPI 0 to send one char data.
Input: one char data
Output: slave return data
Author: zhuobin
Date: 2017/10/10
Description: SPI 0 can send max 16BIT data one time.
***********************************************************/
unsigned char SPI0_SendDate(unsigned char date)
{
	S0SPDR = date;			//Send date
	
	while ((S0SPSR & 0x80) == 0);

	return (S0SPDR);
}

/***********************************************************
Function: Initialize SPI 1 to send one char data.
Input: one char data
Output: slave return data
Author: zhuobin
Date: 2017/10/10
Description: SPI 1 can send max 16BIT data one time.
***********************************************************/
unsigned char SPI1_SendDate(unsigned char date)
{
	S1SPDR = date;			//Send date

	while ((S1SPSR & 0x80) == 0);

	return (S1SPDR);
}

/***********************************************************
Function: Initialize SPI 0 interface.
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void SPI0_INIT(void)
{
	PINSEL0 = (PINSEL0 & 0xFFFF00FF) | 0x15<<8;                        /* SPI0 PIN SEL*/
	S0SPCR = 0x00|(0 << 3)|(1 << 4)|(1 << 5)|(0 << 6)|(0 << 7);  /* Master mode*/
	S0SPCCR = SPI_CLK;                                                            /* SPI Clock Counter: PCLK / SnSPCCR=15MHz/12*/
}

/***********************************************************
Function: Initialize SPI 1 interface.
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void SPI1_INIT(void)
{
	PINSEL1 = (PINSEL1 & 0xFFFFFF03)| 0x10<<2|0x10<<4|0x10<<6;  /* SPI1 PIN SEL*/
	S1SPCR = 0x00|(0 << 3)|(1 << 4)|(1 << 5)|(0 << 6)|(0 << 7);      /* Master mode*/
	S1SPCCR = 0x00 | 0x1E;                                                          /* SPI Clock Counter: PCLK / SnSPCCR*/
}

/***********************************************************
Function: Initialize PWM interface.
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void init_PWM (void) 
{
  PINSEL1 |= 0x01<<10;                       /* Enable pin 0.21   as PWM5*/
  PWMPR    = 0x00000000;                    /* Load prescaler  */
  
  PWMPCR = 0x1<<13;                         /* PWM channel 5 single edge control, output enabled */
  PWMMCR = 1<<1;                             /* PWMMR0/PWMMR5 On match with timer reset the counter */
  PWMMR0 = 200;                               /* PWMMR0       */
  PWMMR5 = 100;                               /* PWMMR5    */
  PWMLER = 0xF;                                /* enable shadow latch for match 1 - 3   */ 
  PWMTCR = 0x00000002;                    /* Reset counter and prescaler           */ 
  PWMTCR = 0x00000009;                    /* enable counter and PWM, release counter from reset */
}

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
	counter++;
	switch (counter){
		case 10000://capture oil temp
			flag1 = 1;
		break;

		case 20000://set 50 temp
			flag1 = 2;
		break;

		case 30000://capture Temperature_of_resistance and Hydrogen_Resistance;Stop heating
			flag1 = 3;
		break;

		case 40000://stop heating
			flag1 = 4;
		break;

		case 50000://capture oil temp
			flag1 = 5;
		break;

		case 60000://capture oil temp
			counter = 0;
		break;

		default:
			break;
	}

	if (counter%200 == 0)
		flag2 = 1;//200ms LED
	else if (counter%300 == 0)
		flag2 = 2;//300ms ADC
	else if (counter%600 == 0)
		flag2 = 3;//600 ms checkself
	else if (counter%800 == 0)
		flag2 = 4;//800ms DS1390 
	else if (counter%1800 == 0)
		flag2 = 5;//30min FLASH
	
	T0IR = 1;                                    /* Clear interrupt flag        */
	VICVectAddr = 0;                             /* Acknowledge Interrupt       */
}

/***********************************************************
Function: Initialize timer0 interface.
Input: none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void init_timer(void)
{
	T0MR0 = 14999;                               /* 1mSec = 15.000-1 counts     */
	T0MCR = 3;                                   /* Interrupt and Reset on MR0  */
	T0TCR = 1;                                   /* Timer0 Enable               */
	VICVectAddr0 = (unsigned long)TC0_IR;        /* set interrupt vector in 0   */
	VICVectCntl0 = 0x20 | 4;                     /* use it for Timer 0 Interrupt*/
	VICIntEnable = 0x00000010;                   /* Enable Timer0 Interrupt     */
	VICDefVectAddr = (unsigned long) DefISR;     /* un-assigned VIC interrupts  */
}

