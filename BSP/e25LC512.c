/***********************************************
@		Description: This file is about 512 Kbit(64KB/65536 Byte) SPI Bus Serial EEPROM.
			25LC256 pin  Name     Protoboard	
							1        CS(NOT)     A3
							2        SO          C4
							3        WP(NOT)     5v
							4        Vss         Gnd
							5        SI          C5
							6        SCK         C3
							7        HOLD(NOT)   5v
							8        Vcc         5v
1. Byte and Page-level Write Operations:	128-byte page
2. 16KB/Sector

@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/25.

***********************************************/
#include <LPC21xx.H>                     /* LPC21xx definitions */
#include "Peripherals_LPC2194.h"
#include "e25LC512.h"
#include "stdio.h"
#include "Peripherals_LPC2194.h"

char dat[255] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
				0x77,0x7c,0x39,0x5e,0x79,0x71};//0~F????

void Delay_us(unsigned short value)//hugo add
{
	unsigned short k=1;
	unsigned short i,j;
	for(i=0;i<value;i++)
	{
		k++;j++;k++;j++;
		k++;j++;k++;j++;
		k++;j++;k++;j++;
		k++;j++;k++;j++;
		}
	}

void Delay_ms(unsigned short value)//hugo add
{
	unsigned short i;
	for(i=0;i<value;i++)
	{
		Delay_us(1090);
		}
	}

void e2prom512_write_enable(void)
{
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(WREN);
	LC512_CS_H;
	Delay_ms(5);
}

void e2prom512_write_disable(void)
{
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(WRDI);
	LC512_CS_H;
	Delay_ms(5);
}

void RW_Status(void)
{
  char R_status;	

	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(WREN); 
	LC512_CS_H;
	Delay_us(1);
		
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(RDSR); 	
	Delay_us(1); 	
	do
	{		
	 R_status=SPI1_SendDate(0);		
	 Delay_us(1);		
	}while(R_status & 0x01);//?????
   LC512_CS_H;
}
	
void e2prom512_byte_write(unsigned char value,unsigned short address)//?????????0~65535
{
	union
	{
		struct
		{
			unsigned char addlow;
			unsigned char addhigh;
		}t;
			unsigned short temp_address;
	}add;	
	add.temp_address=address;
	e2prom512_write_enable();
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(0x02);
	SPI1_SendDate(add.t.addhigh);
	SPI1_SendDate(add.t.addlow);
	SPI1_SendDate(value);
	LC512_CS_H;
	Delay_ms(5);
	e2prom512_write_disable();	
}
 	 
unsigned char e2prom512_byte_read(unsigned short start_address)//?????????0~65535
{
	unsigned char temp_value;
	union
	{
		struct
		{
			unsigned char addlow;
			unsigned char addhigh;
		}t;
			unsigned short temp_address;
		}add;

	add.temp_address=start_address;
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(READ);
	SPI1_SendDate(add.t.addhigh);
	SPI1_SendDate(add.t.addlow);
	temp_value=SPI1_SendDate(0x00);	
	LC512_CS_H;
	return temp_value;
	}
void e2prom512_chip_erase(void)
{
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(CE);
	LC512_CS_H;
	Delay_ms(5);	
}

void e2prom512_page_erase(void)
{
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(PE);
	LC512_CS_H;
	Delay_ms(5);	
}

void e2prom512_sector_erase(void)
{
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(SE);
	LC512_CS_H;
	Delay_ms(5);	
}

void SPI_nbyte_write(const unsigned int address, const unsigned char *dat, unsigned int len)
{
  unsigned int i;
	
	e2prom512_write_enable();	
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(WRITE);	
	SPI1_SendDate(address/256);//???
	SPI1_SendDate(address%256);//???	
	for(i=0;i<len;i++)
	{		
		SPI1_SendDate(dat[i]);//???
	}	
	LC512_CS_H;
  Delay_ms(5);
  RW_Status();	
}

unsigned int e2prom512_write (
                           const void *buffer,      /* Buffer to save */
                           unsigned int len,       /* Buffer length */
                           unsigned int address)   /* FLASH address to write */
{
  const unsigned char *s = buffer;
	unsigned int temp_address,integer_before,integer_after;
  	temp_address=address+len;
	integer_before=address>>7;//cannot bigger than 128byte
	integer_after=temp_address>>7;
	if(integer_after>integer_before)//
		{
			SPI_nbyte_write(address,s,(integer_after<<7)-address);//
			SPI_nbyte_write(integer_after<<7,&s[(integer_after<<7)-address],len-((integer_after<<7)-address));//
		}
	else
		{
			SPI_nbyte_write(address,s,len);//
		}	 
		
		return 0;
	
}

void e2prom512_read (
                          unsigned char *buffer,            /* Buffer to fill */
                          unsigned int len,       /* Bytes to read */
                          unsigned int address)   /* FLASH address to read from */
{
  union
	{
		struct
		{
			unsigned char addlow;
			unsigned char addhigh;
		}t;
			unsigned short temp_address;
	}add;
	add.temp_address=address;
	
	unsigned int i;
	LC512_CS_L;
	Delay_us(1);
	SPI1_SendDate(READ); 
	SPI1_SendDate(add.t.addhigh);
	SPI1_SendDate(add.t.addlow);
	
	for(i=0;i<len;i++)
	{
		*buffer=SPI1_SendDate(0);
		 buffer++;
	}
	LC512_CS_H;
	Delay_us(1000);	

}

void Initial_e2prom(void)
{
	LC512_CS_H;
	LC512_SI_H;
	LC512_SCK_H;
}

void e2promtest(void)
{
 unsigned short i;
 unsigned char temp[512];
 unsigned char buffer[512];
 
 for(i=0;i<256;i++)
{
  temp[i]=i;
	//e2prom512_byte_write(temp[i],i);
}
 for(i=0;i<256;i++)
{
  temp[i+256]=i;
	//e2prom512_byte_write(temp[i],i);
}
e2prom512_write(temp,128,0);
e2prom512_write(temp+128,128,128);
e2prom512_write(temp+256,128,256);
e2prom512_write(temp+384,128,384);
e2prom512_read(buffer,512,0); 
for(i=0;i<512;i++)
{
	//buffer[i]=e2prom512_byte_read(i);
	UARTprintf("test %d\n",buffer[i]);
}
}