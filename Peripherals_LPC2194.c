/***********************************************
@		Description: This file is about data of AD420.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions */
#include <stdio.h>
#include "Peripherals_LPC2194.h"

/*---------------------------------------------------General define -------------------------------------------*/
#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
#define CR         0x0D
#define LF         0x0A

/*---------------------------------------------------General function -------------------------------------------*/
void DelayNS (unsigned int uiDly)
{
    unsigned int  i;
    
    for(; uiDly > 0; uiDly--){
        for(i = 0; i < 50000; i++);
    }
} 

void CLOCK_SET(void)
{
	/*--Phase Locked Loop(PLL)--*/
	PLLCON |= 1<<1|1<<0;		/*PLL enable and Connect*/
	PLLCFG |= 1<<5| 3<<0;			/*P|M: set cclk->48MHz*/
	PLLFEED |= 0xAA;
	PLLFEED |= 0x55;
	/*--APB divider--*/
	VPBDIV |= 0;			/*--00:VPB=1/4CCLK----01: APB =processor clock. Pclk->60MHz--10:VPB=1/2CCLK--------*/
}

/*------------------------------------------------------Serial-------------------------------------------------*/
void init_serial (void)  
{               
/* Initialize Serial Interface(uart0/1) */
  PINSEL0 = 0x00000000 | 0x01<<18 | 0x01<<16 | 0x01<<2 | 0x01<<0;                  /* Enable RxD1 and TxD1              */
  U1LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit   0x1<<7|0x11<<0  */
  U1DLL = 8;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
  U1LCR = 0x03;                          /* DLAB = 0                          */
}

/* implementation of putchar (also used by printf function to output data)    */
int sendchar (int ch)  {                 /* Write character to Serial Port    */

  if (ch == '\n')  {
    while (!(U1LSR & 0x20));
    U1THR = 0x0D;                          /* output CR */
  }
  while (!(U1LSR & 0x20));
  return (U1THR = ch);
}


int getkey (void)  {                    /* Read character from Serial Port   */

  while (!(U1LSR & 0x01));              /* wait until character ready        */
  return (U1RBR);
}

/***************/
/* Line Editor */
/***************/
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

/*------------------------------------------------------SPI-------------------------------------------------*/
unsigned char SPI0_SendDate(unsigned char date)
{
	S0SPDR = date;			//Send date
	
	while ((S0SPSR & 0x80) == 0);

	return (S0SPDR);
}

unsigned char SPI1_SendDate(unsigned char date)
{
	S1SPDR = date;			//Send date

	while ((S1SPSR & 0x80) == 0);

	return (S1SPDR);
}

void SPI0_INIT(void)
{
	PINSEL0 = (PINSEL0 & 0xFFFF00FF) | 0x15<<8; /*SPI0 PIN SEL*/
	S0SPCR = 0x00|(0 << 3)|(1 << 4)|(1 << 5)|(0 << 6)|(0 << 7);  /*Master mode*/
	S0SPCCR = SPI_CLK; /*SPI Clock Counter: PCLK / SnSPCCR=15MHz/12*/
}

void SPI1_INIT(void)
{
	PINSEL1 = (PINSEL1 & 0xFFFFFF03)| 0x10<<2|0x10<<4|0x10<<6; /*SPI1 PIN SEL*/
	S1SPCR = 0x00|(0 << 3)|(1 << 4)|(1 << 5)|(0 << 6)|(0 << 7);  /*Master mode*/
	S1SPCCR = 0x00 | 0x1E; /*SPI Clock Counter: PCLK / SnSPCCR*/
}

/*------------------------------------------------------PWM-------------------------------------------------*/
void init_PWM (void) 
{
  PINSEL1 |= 0x01<<10;											//Enable pin 0.21   as PWM5
  PWMPR    = 0x00000000;                    /* Load prescaler  */
  
  PWMPCR = 0x1<<13;                      /* PWM channel 5 single edge control, output enabled */
  PWMMCR = 1<<1;                      /* PWMMR0/PWMMR5 On match with timer reset the counter */
  PWMMR0 = 200;                           /* PWMMR0       */
  PWMMR5 = 100;                               /* PWMMR5    */
  PWMLER = 0xF;                             /* enable shadow latch for match 1 - 3   */ 
  PWMTCR = 0x00000002;                      /* Reset counter and prescaler           */ 
  PWMTCR = 0x00000009;                      /* enable counter and PWM, release counter from reset */
}
/*------------------------------------------------------timer --------------------------------------------------*/

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
	
	if (counter%1 == 0)
		flag2 = 1;//LED
	else if (counter%2 == 0)
		flag2 = 2;//DS1390 
	else if (counter%3 == 0)
		flag2 = 3;//relay
	else if (counter%4 == 0)
		flag2 = 4;//ADC
	else if (counter%5 == 0)
		flag2 = 5;//DAC
	else if (counter%6 == 0)
		flag2 = 6;//FLASH
	else if (counter%30 == 0)
		flag2 = 7;//checkself

  T0IR = 1;                                    /* Clear interrupt flag        */
  VICVectAddr = 0;                             /* Acknowledge Interrupt       */
}

void init_timer(void)
{
	T0MR0 = 14999*1000;                          /* 1mSec = 15.000-1 counts     */
	T0MCR = 3;                                   /* Interrupt and Reset on MR0  */
	T0TCR = 1;                                   /* Timer0 Enable               */
	VICVectAddr0 = (unsigned long)TC0_IR;        /* set interrupt vector in 0   */
	VICVectCntl0 = 0x20 | 4;                     /* use it for Timer 0 Interrupt*/
	VICIntEnable = 0x00000010;                   /* Enable Timer0 Interrupt     */
	VICDefVectAddr = (unsigned long) DefISR;     /* un-assigned VIC interrupts  */
}
