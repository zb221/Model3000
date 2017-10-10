#ifdef VARIABLE_GLOBALS
#define VARIABLE_EXT
#else
#define VARIABLE_EXT extern
#endif

#ifndef __SERIAL_H
#define __SERIAL_H


#define CR     0x0D

typedef struct UartMode
{
  unsigned char datab;  
  unsigned char stopb;  
  unsigned char parity;  
}UARTMODE;

VARIABLE_EXT unsigned char rcv_buf[4096];
VARIABLE_EXT volatile unsigned char rcv_new;
VARIABLE_EXT unsigned int rcv_cnt;
VARIABLE_EXT   void init_serial (void);
VARIABLE_EXT   int sendchar (int ch);
VARIABLE_EXT   int getkey (void);

VARIABLE_EXT  void sendstring(char *string);
VARIABLE_EXT void Uart1_SentByte(unsigned char data);
VARIABLE_EXT  void UARTprintf(const char *fmt,...);

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

#endif

