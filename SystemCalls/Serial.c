/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

/***********************************************
@		Description: This file is serial communication for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: megzheng.
@		Date: 2017/10/13.
***********************************************/
#include "Serial.h"
#include <LPC21xx.H>
#include "stdarg.h"
#include <stdio.h> 
#include "config.h"


unsigned char rcv_buf[4096];
volatile unsigned char rcv_new;
unsigned int rcv_cnt;

__irq void IRQ_UART0(void)
{

  volatile unsigned char i;
	
	do{
     switch(U1IIR&0x0e)
		 {
		   case 0x04:
				 for(i=0;i<7;i++)
				{
			    rcv_buf[rcv_cnt++]=U1RBR;				
				}
				   break;
       case 0x0c:
				  while((U1LSR&0x01)!=0)
					{
					  rcv_buf[rcv_cnt++]=U1RBR;
					}
					rcv_new=1;
				   break;
       case 0x02:
				   break;
       case 0x06:
				  i=U1LSR;
				   break;
			 default:				 
           break;	
		 }			 			 
	}while((U1IIR&0x01)==0);
	VICVectAddr=0;
}

void init_serial (void)  {               /* Initialize Serial Interface       */
	unsigned short Fdiv;
  PINSEL0 = 0x00050000;                  /* Enable RxD1 and TxD1              */
  U1LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
  U1DLL = 156;                            /* 19200 Baud Rate @ 12MHz VPB Clock  */
  U1LCR = 0x03;                          /* DLAB = 0                          */

  U1LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
  U1DLL = 156;                            /* 9600 Baud Rate @ 12MHz VPB Clock  */
  U1LCR = 0x03;                          /* DLAB = 0 */
  U1IER = 0x01;  //
		 
	U0FCR = 0x01;                //使能FIFO设置1个字节触发点
  VICIntSelect=0x00;        //中断选择为 IQR 模式
  VICVectCntl1=0x26 | 6;
	VICVectAddr1 = (unsigned long)IRQ_UART0;										// ??UART0????       
  VICIntEnClr|=1<<6;  //UART0中断使能
  U0IER=0x00000001; //使能接收中断
  VICIntEnable |= 0x00000040;
		 

  PINSEL0 |= 0x00000005;                  /* Enable RxD0 and TxD0              */
  U0LCR=0x83;
  Fdiv=(Fpclk/16)/19200;									
  U0DLM=Fdiv/256;
  U0DLL=Fdiv%256;
  U0LCR=0x03;	
}


/* implementation of putchar (also used by printf function to output data)    */
int sendchar (int ch)  {                 /* Write character to Serial Port    */

  if (ch == '\n')  {
    while (!(U0LSR & 0x20));
    U0THR = CR;                          /* output CR */
  }
  while (!(U0LSR & 0x20));
  return (U0THR = ch);
}


int getkey (void)  {                    /* Read character from Serial Port   */

  while (!(U0LSR & 0x01));              /* wait until character ready        */
  return (U0RBR);
}

void sendstring(char *string)//??*string?????
{
	while(*string!='\0')//??????????
	{
		sendchar(*string);
		string++;
	}
}

void Uart1_SentByte(unsigned char data)
{
	U1THR=data;
	while((U1LSR & 0x40)==0);//?????
}

void UARTprintf(const char *fmt,...)//reentrant
{	
	va_list ap;
//  char *string;	
	char string[256];//??????RAM,?????RAM,????????RAM??(???1024)	
	///EA=0;
	va_start(ap,fmt);
	vsprintf(string,fmt,ap);//???????sprintf??,?????,??????,????
	//EA=0;	
	sendstring(string);
	//EA=1;	
	va_end(ap);
	///EA=1;	
}




