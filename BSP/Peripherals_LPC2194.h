/**********************************************
@		Description: This file is about LPC2194 Peripherals defines.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/


#ifndef __Peripherals_LPC2194_h
#define __Peripherals_LPC2194_h

/*------------------------------------------------General function declaration-----------------------------------*/
void DelayNS (unsigned int uiDly);
void FrecInit(void);

/*-------------------------------------------------LED-----------------------------------------------------------*/
#define		LED_RED_SET		IO1SET|=(1<<17);
#define		LED_RED_CLR		IO1CLR|=(1<<17);
#define		LED_BLUE_SET		IO1SET|=(1<<18);
#define		LED_BLUE_CLR		IO1CLR|=(1<<18);

void LED_init(void);
	
/*------------------------------------------------------------------Serial--------------------------------------------------------------*/
void init_serial (void);          /* initialize serial interface       */
int  sendchar (int ch);           /* write character to serial port    */
int  getkey (void);               /* read character from serial port   */
void getline (char *line, int n);
void sendstring(char *string);
void Uart1_SentByte(unsigned char data);
void UARTprintf(const char *fmt,...);
void UART0_SendData(unsigned char *sendbuf,unsigned int len);
unsigned char Spi1_read_data(void);

/*------------------------------------------------------------------PWM--------------------------------------------------------------*/
void init_PWM (void);


/*------------------------------------------------------------------SPI--------------------------------------------------------------*/
/*---------SPI Control Register(S0SPCR)--------*/
#define BitEnable_8		(0<<2)
#define BitEnable_X		(1<<2)
#define CPHA_first		(0<<3)
#define CPHA_second		(1<<3)
#define CPOL_high			(0<<4)
#define CPOL_low			(1<<4)
#define MSTR_master		(1<<5)
#define MSTR_slave		(0<<5)
#define LSBF_MSB_first	(0<<6)
#define LSBF_LSB_first	(1<<6)
#define SPI_interrupt_disable	(0<<7)
#define SPI_interrupt_enable	(1<<7)
#define BITS_8				(8<<8)
#define BITS_16				(0<<8)

/*----------SPI Clock Counter Register (S0SPCCR)----------------------------------
SPI CLK = PCLK / SnSPCCR value; PCLK = 12MHZ
In Master mode, this register must be an even number greater than or equal to 8.
----------------------------------------------------------------------------*/
#define SPI0_CLK		0x1E
#define SPI1_CLK		0x8

void SPI0_INIT(void);
void SPI1_INIT(void);

unsigned char SPI0_SendDate(unsigned char date);
unsigned char SPI1_SendDate(unsigned char date);

/*----------------------------------timer----------------------------------------*/
void init_timer(void);

#endif

