/***********************************************
@		Description: This file is about M25P16 data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/
#include <LPC21xx.H>                     /* LPC21xx definitions */
#include "Peripherals_LPC2194.h"
#include "M25P16_FLASH.h"
#include <string.h>
#include "parameter.h"
#include "e25LC512.h"
#include "DS1390.h"

unsigned char spi_flash_buffer[256] = {0};//flash 数据缓存

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
Function:	M25P16_erase_map.
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
	unsigned char buffer[256] = {0};
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

//void M25P16_TEST(void)
//{
//	UARTprintf("flash test start\n");
//	
//	M25P16_reset();
//	M25P16_read_ID(ID_buffer);
//	
//	static unsigned char sector = 0, flag = 0; 
//	static int page = 0;
//	
//	spi_flash_buffer[254] = sector;
//	spi_flash_buffer[255] = page;
//	
//	/*SE test*/
//  if (page == 0)
//	M25P16_erase_map(sector*0x10000,SE);

//	M25P16_Write_Data(spi_flash_buffer,256,sector*0x10000+page*256);

//	page++;

//	if (page == 256){
//		sector++;
//		page = 0;
//	}
//	if (sector == 32){
//		sector = 0;
//	}
//  /*read test*/
//	if (page == 0){
//    M25P16_Read_Data(buffer,256,(sector-1)*0x10000+255*256);
//	}else{
//		M25P16_Read_Data(buffer,256,sector*0x10000+(page-1)*256);
//  }
//	UARTprintf("sector = %d\n",buffer[254]);
//	UARTprintf("page = %d\n",buffer[255]);
//		
//	/*write and read all 16Mbit*/
////	M25P16_erase_map(0,BE);
////	for (page=0;page<8192;page++){
////			flag = 0;
////			for (i=0;i<256;i++)
////			{
////				spi_flash_buffer[i] = i;
////			}
////		
////			M25P16_Write_Data(spi_flash_buffer,256,page*256);
////			M25P16_Read_Data(buffer,256,page*256);

////			for (i=0;i<256;i++)
////			{
////				if (spi_flash_buffer[i] != buffer[i]){
////					UARTprintf("spi_flash_buffer[%d]=%d buffer[%d]=%d\n",i,spi_flash_buffer[i],i,buffer[i]);
////					flag++;
////				}
////			}
////		}
//	UARTprintf("flash test over\n");
//}

void Init_M25P16(void)
{
	M25P16_CS_INIT();
	M25P16_reset();
}

void M25P16_Data_Records(void)
{
	memset(spi_flash_buffer,0,sizeof(spi_flash_buffer));

	spi_flash_buffer[0] = 0x5A;
	spi_flash_buffer[1] = run_parameter.reserved_parameter35 >> 8;
	spi_flash_buffer[2] = run_parameter.reserved_parameter35 & 0xFF;
	spi_flash_buffer[3] = run_parameter.reserved_parameter341 >> 8;
	spi_flash_buffer[4] = run_parameter.reserved_parameter341 & 0xFF;
	spi_flash_buffer[5] = run_parameter.reserved_parameter36 & 0xFF;
	spi_flash_buffer[6] = run_parameter.reserved_parameter37 >> 8;
	spi_flash_buffer[7] = run_parameter.reserved_parameter37 & 0xFF;

	spi_flash_buffer[8] = (unsigned int)output_data.H2DG >> 24;
	spi_flash_buffer[9] = ((unsigned int)output_data.H2DG >> 16) & 0xFF;
	spi_flash_buffer[10] = ((unsigned int)output_data.H2DG >> 8) & 0xFF;
	spi_flash_buffer[11] = (unsigned int)output_data.H2DG & 0xFF;

	spi_flash_buffer[12] = run_parameter.oil_temperature_celsius.ubit.hi;
	spi_flash_buffer[13] = run_parameter.oil_temperature_celsius.ubit.lo;
	
	spi_flash_buffer[14] = (unsigned int)output_data.DayROC >> 24;
	spi_flash_buffer[15] = ((unsigned int)output_data.DayROC >> 16) & 0xFF;
	spi_flash_buffer[16] = ((unsigned int)output_data.DayROC >> 8) & 0xFF;
	spi_flash_buffer[17] = (unsigned int)output_data.DayROC & 0xFF;
	
	spi_flash_buffer[18] = 0xA5;
	
//	e2prom512_read(&a,sizeof(unsigned char),232*2);
//	e2prom512_read(&b,sizeof(unsigned char),232*2+1);
	e2prom512_read((unsigned char *)&Intermediate_Data.M25P16_Data_Addr,sizeof(unsigned int),116*2);
//  UARTprintf("M25P16_Data_Addr=%d\n",Intermediate_Data.M25P16_Data_Addr);
	
	if (Intermediate_Data.M25P16_Data_Addr > 1966050)
		Intermediate_Data.M25P16_Data_Addr = 0;
//	if (a > 29)
//		Intermediate_Data.sector = 0;
//	else
//	  Intermediate_Data.sector = a;
//	
//	Intermediate_Data.page = b;

//  if (Intermediate_Data.page == 0)
//	M25P16_erase_map(Intermediate_Data.sector*0x10000,SE);
	
	M25P16_Write_Data(spi_flash_buffer,19,Intermediate_Data.M25P16_Data_Addr);
	
	Intermediate_Data.M25P16_Data_Addr += 19;

//	Intermediate_Data.page++;

//	if (Intermediate_Data.page == 256){
//		Intermediate_Data.sector++;
//		Intermediate_Data.page = 0;
//	}
//	if (Intermediate_Data.sector == 30){
//		Intermediate_Data.sector = 0;
//	}
	
//	e2prom512_write(&Intermediate_Data.sector,sizeof(unsigned char),232*2);
//	e2prom512_write(&Intermediate_Data.page,sizeof(unsigned char),232*2+1);
	e2prom512_write((unsigned char *)&Intermediate_Data.M25P16_Data_Addr,sizeof(unsigned int),116*2);
	
  /*read test*/
//	if (page == 0){
//    M25P16_Read_Data(buffer,256,(sector-1)*0x10000+255*256);
//	}else{
//		M25P16_Read_Data(buffer,256,sector*0x10000+(page-1)*256);
//  }
//	UARTprintf("sector = %d\n",buffer[254]);
//	UARTprintf("page = %d\n",buffer[255]);
	
//	e2prom512_read(&a,sizeof(unsigned char),0);
//	e2prom512_read(&b,sizeof(unsigned char),1);
//	UARTprintf("a=%d,b=%d\n",a,b);
}

//void M25P16_Write_Sensor_Data(void)
//{
//	M25P16_erase_map(30*0x10000,SE);
//	memset(spi_flash_buffer,0,sizeof(spi_flash_buffer));

//	spi_flash_buffer[0] = 0x6A;
//	memcmp(&spi_flash_buffer[1],Sensor_Data.original_data,sizeof(Sensor_Data.original_data));
//	spi_flash_buffer[81] = 0xA6;
//	
//	spi_flash_buffer[82] = 0x7A;
//	memcmp(&spi_flash_buffer[84],Sensor_Data.Sensor_Fit_Para,sizeof(Sensor_Data.Sensor_Fit_Para));
//	spi_flash_buffer[155] = 0xA7;

//	M25P16_Write_Data(spi_flash_buffer,256,30*0x10000);
//	memset(spi_flash_buffer,0,sizeof(spi_flash_buffer));

//}

//void M25P16_Read_Sensor_Data(void)
//{
//    unsigned char i = 0;
//    double *a = NULL;

//    spi_flash_buffer[83] = 0;
//    spi_flash_buffer[84] = 0;
//    spi_flash_buffer[85] = 0;
//    spi_flash_buffer[86] = 0;
//    spi_flash_buffer[87] = 0;
//    spi_flash_buffer[88] = 0;
//    spi_flash_buffer[89] = 0;
//    spi_flash_buffer[90] = 0;

//    a = (double*)(&spi_flash_buffer[83]);
//	  *a = 409.052293;
//    Sensor_Data.p1 = *a;

//    for (i=0;i<8;i++)
//        UARTprintf("spi_flash_buffer[%d]=%d\n",i+83,*(&spi_flash_buffer[83]+i));

//    UARTprintf("%d\n",sizeof(double));
//    UARTprintf("Sensor_Data.p1 = %f\n",Sensor_Data.p1);
//		
//		
//	
//	memset(spi_flash_buffer,0,sizeof(spi_flash_buffer));
//	
//	M25P16_Read_Data(spi_flash_buffer,256,30*0x10000);
//	
//	if ((spi_flash_buffer[0] == 0x6A) && (spi_flash_buffer[81] == 0xA6)){

//	}
//	
//	if ((spi_flash_buffer[82] == 0x7A) && (spi_flash_buffer[155] == 0xA7)){
//    Sensor_Data.p1 = *(double*)(&spi_flash_buffer[83]);
//		Sensor_Data.p2 = *(double*)(&spi_flash_buffer[91]);
//    Sensor_Data.p3 = *(double*)(&spi_flash_buffer[99]);
//		Sensor_Data.p4 = *(double*)(&spi_flash_buffer[107]);
//    Sensor_Data.p5 = *(double*)(&spi_flash_buffer[115]);
//		Sensor_Data.p6 = *(double*)(&spi_flash_buffer[123]);
//		Sensor_Data.p7 = *(double*)(&spi_flash_buffer[131]);
//    Sensor_Data.p8 = *(double*)(&spi_flash_buffer[139]);
//		Sensor_Data.p9 = *(double*)(&spi_flash_buffer[147]);
//	}

//	memset(spi_flash_buffer,0,sizeof(spi_flash_buffer));
//}

void M25P16_Alarm_Log_Records(void)
{
	unsigned char b = 0;
	
	memset(spi_flash_buffer,0,sizeof(spi_flash_buffer));

	spi_flash_buffer[0] = run_parameter.reserved_parameter35 >> 8;
	spi_flash_buffer[1] = run_parameter.reserved_parameter35 & 0xFF;
	spi_flash_buffer[2] = run_parameter.reserved_parameter341 >> 8;
	spi_flash_buffer[3] = run_parameter.reserved_parameter341 & 0xFF;
	spi_flash_buffer[4] = run_parameter.reserved_parameter36 & 0xFF;
	spi_flash_buffer[5] = run_parameter.reserved_parameter37 >> 8;
	spi_flash_buffer[6] = run_parameter.reserved_parameter37 & 0xFF;

	spi_flash_buffer[7] = (unsigned int)output_data.H2DG >> 24;
	spi_flash_buffer[8] = ((unsigned int)output_data.H2DG >> 16) & 0xFF;
	spi_flash_buffer[9] = ((unsigned int)output_data.H2DG >> 8) & 0xFF;
	spi_flash_buffer[10] = (unsigned int)output_data.H2DG & 0xFF;

	spi_flash_buffer[11] = run_parameter.oil_temperature_celsius.ubit.hi;
	spi_flash_buffer[12] = run_parameter.oil_temperature_celsius.ubit.lo;
	
	spi_flash_buffer[13] = (unsigned int)output_data.DayROC >> 24;
	spi_flash_buffer[14] = ((unsigned int)output_data.DayROC >> 16) & 0xFF;
	spi_flash_buffer[15] = ((unsigned int)output_data.DayROC >> 8) & 0xFF;
	spi_flash_buffer[16] = (unsigned int)output_data.DayROC & 0xFF;

	e2prom512_read(&b,sizeof(unsigned char),115*2);
	
	Intermediate_Data.Alarm_page = b;
	
	M25P16_Write_Data(spi_flash_buffer,256,31*0x10000+Intermediate_Data.Alarm_page*256);

	Intermediate_Data.Alarm_page++;

	if (Intermediate_Data.Alarm_page == 256){
		Intermediate_Data.Alarm_page = 0;
	}

	e2prom512_write(&Intermediate_Data.Alarm_page,sizeof(unsigned char),115*2);
	
//  /*read test*/
//	if (Intermediate_Data.page == 0){
//    M25P16_Read_Data(buffer,256,Intermediate_Data.sector*0x10000+255*256);
//	}else{
//		M25P16_Read_Data(buffer,256,Intermediate_Data.sector*0x10000+(Intermediate_Data.page-1)*256);
//  }

//	UARTprintf("sector = %d\n",buffer[254]);
//	UARTprintf("page = %d\n",buffer[255]);
	
//	e2prom512_read(&a,sizeof(unsigned char),2);
//	e2prom512_read(&b,sizeof(unsigned char),3);
//	UARTprintf("a=%d,b=%d\n",a,b);
}
