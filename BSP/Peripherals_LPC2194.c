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
#include "stdarg.h"
#include "Command.h"

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
unsigned char rcv_buf[100];
volatile unsigned char rcv_new;
unsigned int rcv_cnt;


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
Function:	serial default interrupt.
Input:	none
Output: none
Author: megzheng
Date: 2017/10/13
Description: serial default interrupt.
***********************************************************/

__irq void IRQ_UART0(void)
{
		volatile unsigned char i;

		do{
		switch(U0IIR&0x0e)
		{
		case 0x04://接收数据可用
		for(i=0;i<7;i++)
		{
		rcv_buf[rcv_cnt++]=U0RBR;				
		}
		break;
		case 0x0c://接收超时
		while((U0LSR&0x01)!=0)//U0RBR包含有效数据
		{
		rcv_buf[rcv_cnt++]=U0RBR;
		}
		break;
		case 0x02://THRE中断
		break;
		case 0x06://接收线状态
		i=U0LSR;
		break;
		default:				 
		break;	
		}
		}while((U0IIR&0x01)==0);//没有挂起的中断
	if(0x0A==rcv_buf[rcv_cnt-1] && 0x0D==rcv_buf[rcv_cnt-2])
	{
		rcv_new=1;
	}
	else
	{
		if(rcv_cnt>=CMD_LEN)
		{
		rcv_cnt=0;
		}
	}
		VICVectAddr=0;
}


/***********************************************************
Function:	init peripherals.
Input:	none
Output: none
Author: megzheng
Date: 2017/10/16
Description: setup the timer counter 0 interrupt.
***********************************************************/
void init_serial (void)  
{
	unsigned short Fdiv;
  PINSEL0 |= 0x00050005;                /* Enable UART0 UART1             */
  U1LCR = 0x83;                         /* 8 bits, no Parity, 1 Stop bit     */
	Fdiv=(Fpclk/16)/19200;								/* 19200 Baud Rate @ 12MHz VPB Clock  */	
  U1DLM=Fdiv/256;
  U1DLL=Fdiv%256;                     
  U1LCR = 0x03;                         /* DLAB = 0                          */

	U0FCR = 0x81;                          /* FIFO enable 8 character trigger   */
  VICIntSelect=0x00;        
  VICVectCntl1=0x26 | 6;
	VICVectAddr1 = (unsigned long)IRQ_UART0;										   
  VICIntEnClr|=1<<6;  
  U0IER=0x00000001;                      /* enable uart0 irq   */
  VICIntEnable |= 0x00000040;            
		 
  U0LCR=0x83;								
  U0DLM=Fdiv/256;
  U0DLL=Fdiv%256;
  U0LCR=0x03;	                           /* DLAB = 0          */
}
/***********************************************************
Function:	serial send char.
Input:	32bit string
Output: none
Author: megzheng
Date: 2017/10/13
Description: serial init.
***********************************************************/
/* implementation of putchar (also used by printf function to output data)    */
int sendchar (int ch)  {                 /* Write character to Serial Port    */

  if (ch == '\n')  {
    while (!(U0LSR & 0x20));
    U0THR = CR;                          /* output CR */
  }
  while (!(U0LSR & 0x20));
  return (U0THR = ch);
}

/***********************************************************
Function:get serial key
Input: none
Output: none
Author: megzheng
Date: 2017/10/16
Description:  .
***********************************************************/
int getkey (void)  {                    /* Read character from Serial Port   */

  while (!(U0LSR & 0x01));              /* wait until character ready        */
  return (U0RBR);
}

void sendstring(char *string)
{
	while(*string!='\0')
	{
		sendchar(*string);
		string++;
	}
}

void UART0_SendData(unsigned char *sendbuf,unsigned int len)
{
  while(len!=0)
	{
	  U0THR = *sendbuf++;
		while((U0LSR&0x20)==0);
		len--;
	}
}

void Uart1_SentByte(unsigned char data)
{
	U1THR=data;
	while((U1LSR & 0x40)==0);//?????
}

void UARTprintf(const char *fmt,...)
{	
	va_list ap;
//  char *string;	
	char string[256];	
	///EA=0;
	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	//EA=0;	
	sendstring(string);
	//EA=1;	
	va_end(ap);
	///EA=1;	
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
	T0MR0 = 12000-1;                               /* 1mSec = 15.000-1 counts     */
	T0MCR = 3;                                   /* Interrupt and Reset on MR0  */
	T0TCR = 1;                                   /* Timer0 Enable               */
	VICVectAddr0 = (unsigned long)TC0_IR;        /* set interrupt vector in 0   */
	VICVectCntl0 = 0x20 | 4;                     /* use it for Timer 0 Interrupt*/
	VICIntEnable = 0x00000010;                   /* Enable Timer0 Interrupt     */
	VICDefVectAddr = (unsigned long) DefISR;     /* un-assigned VIC interrupts  */
}



