/***********************************************
@		Description: This file is about AD7738 data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/
#include <LPC21xx.H>                     /* LPC21xx definitions */
#include "Peripherals_LPC2194.h"
#include "M25P16_FLASH.h"
#include <string.h>
#include "parameter.h"

unsigned char spi_flash_buffer[256] = {0};//flash 数据缓存
unsigned char buffer[256] = {0};
unsigned char ID_buffer[3] = {0};

/***********************************************************
Function:	 M25P16 CS pin init.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_CS_INIT(void)
{
	PINSEL1 = PINSEL1 & (~(0x03 << 18));        /*RD1->P0.25*/	
	IODIR0 = IODIR0 | 0x1<<25;								
	IOSET0 = IOSET0 | 0x1<<25;
}

/***********************************************************
Function:	 M25P16 reset.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_reset(void)//RESET
{
	IOCLR0 |= 0x1<<25;		/* set CS low*/
	SPI1_SendDate(0xf0);
	IOSET0 |= 0x1<<25;		/* Set CS Hight*/
}

/***********************************************************
Function:	M25P16 read ID.
Input:	pbuffer to recive flash ID
Output: none
Author: zhuobin
Date: 2017/10/10
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_read_ID(unsigned char *pbuffer) 
{	
	IOCLR0 |= 0x1<<25;		/* set CS low*/
	SPI1_SendDate(0x9f);

	*pbuffer = SPI1_SendDate(0x00);
	*(pbuffer+1) = SPI1_SendDate(0x00);
	*(pbuffer+2) = SPI1_SendDate(0x00);
	
	IOSET0 |= 0x1<<25;		/* Set CS Hight*/
}

/***********************************************************
Function:	M25P16_erase_4Kmap.
Input:	block_address
Output: none
Author: zhuobin
Date: 2017/10/10
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_erase_map(unsigned int block_address, unsigned char erase_type)
{
	unsigned char ic_status;
	union
	{
		struct
		{
			unsigned char addlow;
			unsigned char addmid;
			unsigned char addhigh;
			unsigned char addhigher;
		}t;
		unsigned int temp_address;
	}add;
	
//	block_address *= 4096;
	add.temp_address = block_address;
	
	IOCLR0 |= 0x1<<25;		/* set CS low */
	SPI1_SendDate(0x06);	/* Write Enable */
	IOSET0 |= 0x1<<25;		/* Set CS Hight*/
	
	do
	{
		IOCLR0 |= 0x1<<25;		/* set CS low*/
		SPI1_SendDate(0x05);	/* Read Status Register */
		ic_status = SPI1_SendDate(0xff);
		IOSET0 |= 0x1<<25;		/* Set CS Hight*/
	}while(ic_status & 0x01);	/* wait chip free */
	
	IOCLR0 |= 0x1<<25;		/* set CS low */
	SPI1_SendDate(0x02);	/* Page Program */
	IOSET0 |= 0x1<<25;		/* Set CS Hight */	
	
	if (erase_type == SE){
		IOCLR0 |= 0x1<<25;		/* set CS low*/
		SPI1_SendDate(0xD8);	//Sector Erase
		SPI1_SendDate(add.t.addhigh);
		SPI1_SendDate(add.t.addmid);
		SPI1_SendDate(add.t.addlow);
		IOSET0 |= 0x1<<25;		/* Set CS Hight*/
	}else if (erase_type == BE){
		IOCLR0 |= 0x1<<25;		/* set CS low*/
		SPI1_SendDate(0xC7);	//Bulk Erase
		IOSET0 |= 0x1<<25;		/* Set CS Hight*/
	}
	
	do
	{
		IOCLR0 |= 0x1<<25;		/* set CS low*/
		SPI1_SendDate(0x05);	/* Read Status Register */
		ic_status = SPI1_SendDate(0xff);
		IOSET0 |= 0x1<<25;		/* Set CS Hight*/
	}while(ic_status & 0x01);	/* wait chip free */
	
}

/***********************************************************
Function:	 M25P16 read data.
Input: Instruction, Receive buffer, data nubmer, start to read address.
Output: none
Author: zhuobin
Date: 2017/10/19
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_Read_Data(unsigned char *Rec_buffer,unsigned int data_nub,unsigned int start_address)
{
	union
	{
		struct
		{
			unsigned char addlow;
			unsigned char addmid;
			unsigned char addhigh;
			unsigned char addhigher;
			}t;
			unsigned int temp_address;
	}add;
	
	add.temp_address=start_address;
	IOCLR0 |= 0x1<<25;		/* set CS low*/

	SPI1_SendDate(0x03);	/* Read Data Bytes */
	SPI1_SendDate(add.t.addhigh);
	SPI1_SendDate(add.t.addmid);
	SPI1_SendDate(add.t.addlow);
	while(data_nub--)
	{
		*Rec_buffer = SPI1_SendDate(0xff);
		Rec_buffer++;
	}	
	IOSET0 |= 0x1<<25;		/* Set CS Hight*/
}

/***********************************************************
Function:	 M25P16 write data.
Input: write buffer, data nubmer, start to write address.
Output: none
Author: zhuobin
Date: 2017/10/19
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_Write_Data(unsigned char *write_buffer,unsigned int data_nub,unsigned int start_address)
{
	unsigned char ic_status;
	union
	{
		struct
		{
			unsigned char addlow;
			unsigned char addmid;
			unsigned char addhigh;
			unsigned char addhigher;
			}t;
			unsigned int temp_address;
	}add;
	
	add.temp_address = start_address;
	
	IOCLR0 |= 0x1<<25;		/* set CS low */
	SPI1_SendDate(0x06);	/* Write Enable */
	IOSET0 |= 0x1<<25;		/* Set CS Hight */

	IOCLR0 |= 0x1<<25;		/* set CS low */
	SPI1_SendDate(0x02);	/* Page Program */
	SPI1_SendDate(add.t.addhigh);
	SPI1_SendDate(add.t.addmid);
	SPI1_SendDate(add.t.addlow);
	
	while(data_nub--)
	{
		SPI1_SendDate(*write_buffer);
		write_buffer++;
	}	
	IOSET0 |= 0x1<<25;		/* Set CS Hight*/

	do
	{
		IOCLR0 |= 0x1<<25;		/* set CS low*/
		SPI1_SendDate(0x05); /* Read Status Register */
		ic_status = SPI1_SendDate(0xff);
		IOSET0 |= 0x1<<25;		/* Set CS Hight*/
	}while(ic_status & 0x01); /* wait chip free */
	
	IOCLR0 |= 0x1<<25;		/* set CS low*/
	SPI1_SendDate(0x02);	/* Page Program */
	IOSET0 |= 0x1<<25;		/* Set CS Hight*/	
}

/***********************************************************
Function:	M25P16_read_data_anywhere.
Input:	data_count start_address
Output: none
Author: zhuobin
Date: 2017/10/10
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_read_data_anywhere(unsigned short data_count,unsigned int start_address)//spi 芯片任意地址，读任意个数据，数据量小于等于256
{
	unsigned int temp_address,integer_before,integer_after;
	temp_address=start_address+data_count;
	integer_before=start_address>>8;
	integer_after=temp_address>>8;
	if(integer_after>integer_before)
	{
			M25P16_Read_Data(buffer,(integer_after<<8)-start_address,start_address);
			M25P16_Read_Data(&buffer[(integer_after<<8)-start_address],data_count-((integer_after<<8)-start_address),integer_after<<8);
	}
	else
	{
			M25P16_Read_Data(buffer,data_count,start_address);
	}
	
}	

/***********************************************************
Function:	M25P16_write_data_anywhere.
Input:	data_count start_address
Output: none
Author: zhuobin
Date: 2017/10/10
Description: 16 Mbit, low voltage, Serial Flash memory with 50 MHz SPI bus interface.
***********************************************************/
void M25P16_write_data_anywhere(unsigned short data_count,unsigned int start_address)//spi 芯片任意地址，写任意个数据，数据量小于等于256
{
	unsigned int temp_address,integer_before,integer_after;
	temp_address=start_address+data_count;
	integer_before=start_address>>12;
	integer_after=temp_address>>12;	
	if(integer_after>integer_before)
		{
			if(integer_before==511)//最后2k预留
				{
					return;
					}
				M25P16_erase_map(integer_after,SE);//擦除下一个4K区间
			}
			
	temp_address=start_address+data_count;
	integer_before=start_address>>8;
	integer_after=temp_address>>8;
	if(integer_after>integer_before)
		{
			M25P16_Write_Data(spi_flash_buffer,(integer_after<<8)-start_address,start_address);
			M25P16_Write_Data(&spi_flash_buffer[(integer_after<<8)-start_address],data_count-((integer_after<<8)-start_address),integer_after<<8);
	}
	else
	{
			M25P16_Write_Data(spi_flash_buffer,data_count,start_address);
	}		
}

void M25P16_TEST(void)
{
	UARTprintf("flash test start\n");
	
	M25P16_reset();
	M25P16_read_ID(ID_buffer);
	
	int i = 0, page = 0, flag = 0;
	
	/*SE test*/
	M25P16_erase_map(0x000000,SE);
	spi_flash_buffer[0] = 18;
	M25P16_write_data_anywhere(1,0x123);
	M25P16_read_data_anywhere(1,0x123);
	UARTprintf("buffer[0] = %d\n",buffer[0]);
		
	/*write and read all 16Mbit*/
	M25P16_erase_map(0,BE);
	for (page=0;page<8192;page++){
			flag = 0;
			for (i=0;i<256;i++)
			{
				spi_flash_buffer[i] = i;
			}
		
			M25P16_Write_Data(spi_flash_buffer,256,page*256);
			M25P16_Read_Data(buffer,256,page*256);

			for (i=0;i<256;i++)
			{
				if (spi_flash_buffer[i] != buffer[i]){
					UARTprintf("spi_flash_buffer[%d]=%d buffer[%d]=%d\n",i,spi_flash_buffer[i],i,buffer[i]);
					flag++;
				}
			}
		}
	UARTprintf("flash test over\n");
}

void Init_M25P16(void)
{
	M25P16_CS_INIT();
	M25P16_reset();
}

void M25P16_Data_Records(void)
{
	unsigned int i = 0;
	float tmp_buffer[12] = {0};
	static unsigned char sector = 0; 
	static int page = 0;
	
	spi_flash_buffer[0] = CurrentTime.SpecificTime.year+2000;
	spi_flash_buffer[1] = '/';
	spi_flash_buffer[2] = CurrentTime.SpecificTime.month;
	spi_flash_buffer[3] = '/';
	spi_flash_buffer[4] = CurrentTime.SpecificTime.day;
	spi_flash_buffer[5] = ' ';
	spi_flash_buffer[6] = CurrentTime.SpecificTime.hour;
	spi_flash_buffer[7] = ':';
	spi_flash_buffer[8] = CurrentTime.SpecificTime.min;
	spi_flash_buffer[9] = ':';
	spi_flash_buffer[10] = CurrentTime.SpecificTime.sec;
	spi_flash_buffer[11] = ' ';
	
	tmp_buffer[0] = output_data.PcbTemp;
	tmp_buffer[1] = output_data.H2AG;
	tmp_buffer[2] = output_data.OilTemp;
	tmp_buffer[3] = output_data.H2DG;
	tmp_buffer[4] = output_data.H2G;
	tmp_buffer[5] = output_data.H2SldAv;
	tmp_buffer[6] = output_data.DayROC;
	tmp_buffer[7] = output_data.WeekROC;
	tmp_buffer[8] = output_data.MonthROC;
	
	tmp_buffer[9] = output_data.SensorTemp;
	tmp_buffer[10] = output_data.H2Resistor;
	tmp_buffer[11] = output_data.TempResistor;
	
	memcpy(spi_flash_buffer+12,tmp_buffer,sizeof(tmp_buffer));
	
	spi_flash_buffer[254] = sector;
	spi_flash_buffer[255] = page;

  if (page == 0)
	M25P16_erase_map(sector*0x10000,SE);
	
	M25P16_Write_Data(spi_flash_buffer,256,sector*0x10000+page*256);

	page++;

	if (page == 256){
		sector++;
		page = 0;
	}
	if (sector == 32){
		sector = 0;
	}
  /*read test*/
//	if (page == 0){
//    M25P16_Read_Data(buffer,256,(sector-1)*0x10000+255*256);
//	}else{
//		M25P16_Read_Data(buffer,256,sector*0x10000+(page-1)*256);
//  }
//	UARTprintf("sector = %d\n",buffer[254]);
//	UARTprintf("page = %d\n",buffer[255]);
}
