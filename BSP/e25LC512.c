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

void EEPROM_CS_INIT(void)
{
	PINSEL0 = PINSEL0 & (~(0x3 << 14));			/*/CS0->P0[7]*/
	IODIR0 = IODIR0 | 0x1<<7;
	IOSET0 = IOSET0 | 0x1<<7;
}

void Eeprom25lc512_write_enable(void)
{
	IOCLR0 |= 0x1<<7;		/* set CS low */
	SPI1_SendDate(WREN);	/* Write Enable */
	IOSET0 |= 0x1<<7;		/* Set CS Hight*/
}

void Eeprom25lc512_write_disable(void)
{
	IOCLR0 |= 0x1<<7;		/* set CS low */
	SPI1_SendDate(WRDI);	/* Write Enable */
	IOSET0 |= 0x1<<7;		/* Set CS Hight*/
}

void EEPROM_READ_STATUS(void)
{
	unsigned char ic_status = 0;
	IOCLR0 |= 0x1<<7;		/* set CS low*/
	do
	{
		SPI1_SendDate(RDSR);	/* Read Status Register */
		ic_status = SPI1_SendDate(0xff);
	}while(ic_status & 0x01);	/* wait chip free */
	IOSET0 |= 0x1<<7;		/* Set CS Hight*/
}
	
/***********************************************************
Function:	EEPROM erase map.
Input:	address and erase_type
Output: none
Author: zhuobin
Date: 2017/10/30
Description: 512 Kbit SPI Bus Serial EEPROM.
***********************************************************/
void EEPROM_erase_map(unsigned int address, unsigned char erase_type)
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
	
	add.temp_address = address;
		
	if (erase_type == CE){
		IOCLR0 |= 0x1<<7;		/* set CS low*/
		SPI1_SendDate(CE);	//Chip Erase
		IOSET0 |= 0x1<<7;		/* Set CS Hight*/
	}else if (erase_type == PE){
		IOCLR0 |= 0x1<<7;		/* set CS low*/
		SPI1_SendDate(PE);	//Page Erase
		SPI1_SendDate(add.t.addhigh);
		SPI1_SendDate(add.t.addlow);
		IOSET0 |= 0x1<<7;		/* Set CS Hight*/
	}else if (erase_type == SE){
		IOCLR0 |= 0x1<<7;		/* set CS low*/
		SPI1_SendDate(SE);	//Sector Erase
		SPI1_SendDate(add.t.addhigh);
		SPI1_SendDate(add.t.addlow);
		IOSET0 |= 0x1<<7;		/* Set CS Hight*/
	}
}

/***********************************************************
Function:	EEPROM write one Byte.
Input:	value and address
Output: none
Author: zhuobin
Date: 2017/10/30
Description: 512 Kbit SPI Bus Serial EEPROM.
***********************************************************/
void Eeprom25lc512_write_data(unsigned char value,unsigned int address)
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
	
	add.temp_address = address;
	
	Eeprom25lc512_write_enable();
	
	IO0CLR |= 0x1<<7;		/* set CS low*/
	SPI1_SendDate(WRITE);	/* WRITE data */
	SPI1_SendDate(add.t.addhigh);
	SPI1_SendDate(add.t.addlow);
	SPI1_SendDate(value);
	IO0SET |= 0x1<<7;		/* set CS hight*/
	
	EEPROM_READ_STATUS();
	Eeprom25lc512_write_disable();
}

/***********************************************************
Function:	EEPROM read one Byte.
Input:	start_address
Output: none
Author: zhuobin
Date: 2017/10/30
Description: 512 Kbit SPI Bus Serial EEPROM.
***********************************************************/
unsigned char Eeprom25lc512_read_data(unsigned int start_address)
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
	
	add.temp_address = start_address;
		
	IO0CLR |= 0x1<<7;		/* set CS low*/
	SPI1_SendDate(READ);	/* send read commond*/
	SPI1_SendDate(add.t.addhigh);
	SPI1_SendDate(add.t.addlow);
	temp_value = SPI1_SendDate(0x00);	
	IO0SET |= 0x1<<7;		/* set CS hight*/
		
	return temp_value;
}

void LC512_TEST(void)
{
	int i = 0;
	EEPROM_CS_INIT();
	EEPROM_erase_map(0,CE);
	UARTprintf("LC512_TEST start \n");
	for (i=0;i<65536;i++)
	{
		Eeprom25lc512_write_data(1,i);
		if (1 != Eeprom25lc512_read_data(i))
		UARTprintf("error \n");
	}
	UARTprintf("LC512_TEST Ok \n");
}

