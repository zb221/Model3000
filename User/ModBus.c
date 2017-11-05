/******************************************************************************/
/* MEASURE.C: Remote Measurement Recorder                                     */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/
#include <stdio.h>                      /* standard I/O .h-file              */
#include <stdlib.h>
#include <ctype.h>                      /* character functions               */
#include <LPC21xx.H>                    /* LPC21xx definitions               */
#include <string.h>
#include "main.h"                    /* global project definition file    */
#include "config.h"
#include "Peripherals_LPC2194.h"
#include "Command.h"
#include "parameter.h"
#include "Modbus.h"
#include "DS1390.h"
#include "M25P16_FLASH.h"
#include "e25LC512.h"
REALTIMEINFO	RealTime_Modbus;     //??????

//ºê¶¨ÒåStop   hugo add 
unsigned char SpiFlash_Addr[5];
//±äÁ¿¶¨ÒåStart  hugo add
unsigned short time0_count_1ms;
unsigned short time0_count_1ms_run;

/* CRC ¸ßÎ»×Ö½ÚÖµ±í */
const unsigned char auchCRCHi[] = {
  
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
 
} ; 
/* CRCµÍÎ»×Ö½ÚÖµ±í*/
const unsigned char auchCRCLo[] = {
  
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
 
} ;

 __irq void Uart_IRQ(void)
{
	unsigned char recept_value_buffer;

	if((U1IIR&0x0e)==0x04)  //ÅÐ¶Ï½øÈëµÄÊÇ·ñÎªUART1½ÓÊÕÖÐ¶Ï
	{

		while (!(U1LSR & 0x01));//µÈ´ýÊý¾Ý½ÓÊÕÍê±Ï

		recept_value_buffer=U1RBR;   	  
		if(user_parameter.flag.ubit.recept_ok==0)
		{
			Data_Analysis(recept_value_buffer);
		}
	}
	VICVectAddr = 0;                              /* Acknowledge Interrupt       */
}

void Init_io(void)//hugo add
{
	IO1DIR |=EN_485_DE;
	//IO1SET = EN_485_DE;
	AT25_CS_H;// ²»Ê¹ÄÜAT25
	}

//void Delay_ms(unsigned short value)//hugo add
//{
//	unsigned short k=50000;
//	unsigned short i,j;
//	for(i=0;i<value;i++)
//	{
//		for(j=0;j<k;j++);
//		}
//	}
//void Delay_us(unsigned short value)//hugo add
//{
//	unsigned short k=1;
//	unsigned short i,j;
//	for(i=0;i<value;i++)
//	{
//		for(j=0;j<k;j++);
//		}
//	}	
	
//void Init_Pll(void)//hugo add 60MHz      VPB 60/4  MHz
//	{
//		PLLCON=1;					
//		PLLCFG=0X24;
//		PLLFEED=0XAA;
//		PLLFEED=0X55;
//		while((PLLSTAT&(1<<10))==0);
//		PLLCON=3;
//		PLLFEED=0XAA;
//		PLLFEED=0X55;	
//		}

//void init_spi(void)//hugo add mclk 60MHz Fpclk 15MHz
//	{
//		PINSEL1 |= 0x000000A8;                  /* Enable SPI1   SCK MISO MOSI            */
//		S1SPCCR = 16;														//·ÖÆµÏµÊý
//		S1SPCR= (0<<3)|													//CPHA=0;Êý¾ÝÔÚsckµÄµÚÒ»¸öÊ±ÖÓÑØ²ÉÑù
//						(1<<4)|													//CPLO=1£»SCKÎªµÍÓÐÐ§//Ä¬ÈÏÊÇ1
//						(1<<5)|                         //MSTR=1£¬SPI´¦ÓÚÖ÷Ä£Ê½
//						(0<<6)|													//LSBF=0;Êý¾Ý´«Êä¸ßÎ»ÔÚÇ°
//						(0<<7);													//SPIE=0£¬SPIÖÐ¶Ï½ûÖ¹				
//		
//		}
		
//void Spi_write_data(unsigned char data)//spi Ð´Êý¾Ý
//{
//unsigned char i;
//DS1390_CS_H;
//LC512_CS_H;
//	
//	for(i=0;i<8;i++)
//	{
//		LC512_SCK_L;
//		delay_ms(1);		
//		if((data&0x80)==0x80)
//		{
//			LC512_SI_H;
//		}
//    else
//    {
//      LC512_SI_L;
//    }		
//		data = data<<1; 
//		LC512_SCK_H; 
//		delay_ms(1); 
//	}		
//}	

//void AT25df16_reset(void)//RESET
//{
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(100);
//  AT25_CS_H;// ²»Ê¹ÄÜAT25
//}
	
//unsigned char Spi_read_data(void)//spi ¶ÁÊý¾Ý
//{
//unsigned char tmp = 0, i;
//	
//	for(i=0;i<8;i++)
//	{
//		LC512_SCK_L;
//		delay_ms(1);		
//		tmp <<= 1; 
//    if(LC512_SO_IN!=0)
//      {
//       tmp |= 0x01;
//      }	
//    else
//      {
//       tmp &= 0xFE;
//      }			
//		LC512_SCK_H;
//		delay_ms(1);
//	}	
//	return tmp;
//}	

//void AT25df16_write_data(unsigned char *pbuffer,unsigned short data_count,unsigned int start_address)//spi Ò³ÈÎÒâµØÖ·Ð´Êý¾Ý
//{
//	unsigned char ic_status;
//	union
//	{
//		struct
//		{
//			unsigned char addlow;
//			unsigned char addmid;
//			unsigned char addhigh;
//			unsigned char addhigher;
//			}t;
//			unsigned int temp_address;
//		}add;
//	
//	add.temp_address=start_address;
//		
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x06);//·¢Ð´Ê¹ÄÜÖ¸Áî 
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//		
//	Delay_us(1);
//		
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x02);//·¢Ð´Êý¾ÝÃüÁ
//	
//	Spi_write_data(add.t.addhigh);
//	Spi_write_data(add.t.addmid);
//	Spi_write_data(add.t.addlow);
//		
//	while(data_count--)
//	{
//		Spi_write_data(*pbuffer);
//		pbuffer++;
//		}	
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//	do
//	{
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x05);//·¢Ð´¶Á×´Ì¬ÃüÁî
//	Spi_write_data(0xff);
//	ic_status=Spi_read_data();
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//		}while(ic_status & 0x01);//´ýÐ¾Æ¬¿ÕÏÐ
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x02);//·¢Ð´½ûÖ¹Ö¸Áî
//	AT25_CS_H;// ²»Ê¹ÄÜAT25	
//	}


//void AT25df16_erase_4Kmap(unsigned int block_address)//4K²Á³ý  0~511
//{
//	unsigned char ic_status;
//	union
//	{
//		struct
//		{
//			unsigned char addlow;
//			unsigned char addmid;
//			unsigned char addhigh;
//			unsigned char addhigher;
//			}t;
//			unsigned int temp_address;
//		}add;
//	block_address*=4096;
//	add.temp_address=block_address;
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x06);//·¢Ð´Ê¹ÄÜÖ¸Áî 
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//	
//	do
//	{
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x05);//·¢Ð´¶Á×´Ì¬ÃüÁî
//	Spi_write_data(0xff);
//	ic_status=Spi_read_data();
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//		}while(ic_status & 0x01);//´ýÐ¾Æ¬¿ÕÏÐ
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x02);//·¢Ð´½ûÖ¹Ö¸Áî
//	AT25_CS_H;// ²»Ê¹ÄÜAT25	
//	Delay_us(1);
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Spi_write_data(0x20);//·¢Ð´¿é²Á³ýÃüÁî
//	Spi_write_data(add.t.addhigh);
//	Spi_write_data(add.t.addmid);
//	Spi_write_data(add.t.addlow);
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//	
//	do
//	{
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x05);//·¢Ð´¶Á×´Ì¬ÃüÁî
//	Spi_write_data(0xff);
//	ic_status=Spi_read_data();
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//		}while(ic_status & 0x01);//´ýÐ¾Æ¬¿ÕÏÐ
//	
//	}

//void AT25df16_write_data_anywhere(unsigned short data_count,unsigned int start_address)//spi Ð¾Æ¬ÈÎÒâµØÖ·£¬Ð´ÈÎÒâ¸öÊý¾Ý£¬Êý¾ÝÁ¿Ð¡ÓÚµÈÓÚ256
//{
//	unsigned int temp_address,integer_before,integer_after;
//	temp_address=start_address+data_count;//ÆðÊ¼µØÖ·+Ð´Èë×Ö½ÚÊý
//	integer_before=start_address>>12;//4K  ³ýÒÔ4096byte  Ò»ÉÈÇø
//	integer_after=temp_address>>12;	
//	if(integer_after>integer_before)//¿çÉÈÇø²Ù×÷
//		{
//			if(integer_before==511)//×îºó2kÔ¤Áô
//				{
//					return;
//					}
//			AT25df16_erase_4Kmap(integer_after);//²Á³ýÏÂÒ»¸ö4KÇø¼ä
//			}
//			
//	temp_address=start_address+data_count;
//	integer_before=start_address>>8;//256×Ö½Ú  Ò»Ò³
//	integer_after=temp_address>>8;
//	if(integer_after>integer_before)//¿çpage²Ù×÷
//		{
//			AT25df16_write_data(user_parameter.spi_flash_buffer,(integer_after<<8)-start_address,start_address);//µ±Ç°Ò³Ð´Íê
//			AT25df16_write_data(&user_parameter.spi_flash_buffer[(integer_after<<8)-start_address],data_count-((integer_after<<8)-start_address),integer_after<<8);//Ð´ÈëÐÂÒ³
//			//½Ó×ÅÊ£ÏÂµÄµØÖ·Ð´È&user_parameter.spi_flash_buffer[(integer_after<<8)-start_address]
//		}
//	else
//		{
//			AT25df16_write_data(user_parameter.spi_flash_buffer,data_count,start_address);//Ö±½ÓÁ¬ÐøÐ´
//			}		
//			
//	}
//	
//void AT25df16_erase_block(unsigned int block_address)//¿é²Á³ý  0~31
//{
//	unsigned char ic_status;
//	union
//	{
//		struct
//		{
//			unsigned char addlow;
//			unsigned char addmid;
//			unsigned char addhigh;
//			unsigned char addhigher;
//			}t;
//			unsigned int temp_address;
//		}add;
//	block_address*=65536;
//	add.temp_address=block_address;
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x06);//·¢Ð´Ê¹ÄÜÖ¸Áî 
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//	
//	do
//	{
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x05);//·¢Ð´¶Á×´Ì¬ÃüÁî
//	Spi_write_data(0xff);
//	ic_status=Spi_read_data();
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//		}while(ic_status & 0x01);//´ýÐ¾Æ¬¿ÕÏÐ
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x02);//·¢Ð´½ûÖ¹Ö¸Áî
//	AT25_CS_H;// ²»Ê¹ÄÜAT25	
//	Delay_us(1);
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Spi_write_data(0xD8);//·¢Ð´¿é²Á³ýÃüÁî
//	Spi_write_data(add.t.addhigh);
//	Spi_write_data(add.t.addmid);
//	Spi_write_data(add.t.addlow);
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//	
//	do
//	{
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(1);
//	Spi_write_data(0x05);//·¢Ð´¶Á×´Ì¬ÃüÁî
//	Spi_write_data(0xff);
//	ic_status=Spi_read_data();
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	Delay_us(1);
//		}while(ic_status & 0x01);//´ýÐ¾Æ¬¿ÕÏÐ
//	
//	}

//void AT25df16_read_data(unsigned char *pbuffer,unsigned short data_count,unsigned int start_address)//spi ????????
//{
//	union
//	{
//		struct
//		{
//			unsigned char addlow;
//			unsigned char addmid;
//			unsigned char addhigh;
//			unsigned char addhigher;
//			}t;
//			unsigned int temp_address;
//		}add;
//	
//	add.temp_address=start_address;
//	AT25_CS_L;
//	Delay_us(10);
//	Spi_write_data(0x03);
//		
//	Spi_write_data(add.t.addhigh);
//	Spi_write_data(add.t.addmid);
//	Spi_write_data(add.t.addlow);		
//		
//	while(data_count--)
//	{
////		Spi_write_data(0xff);		
//		*pbuffer=Spi_read_data();
//		pbuffer++;
//		}	
//	AT25_CS_H;// ???AT25
//	}
//	
//void AT25df16_read_data_anywhere(unsigned short data_count,unsigned int start_address)//spi ??????,??????,???????256
//{
//	unsigned int temp_address,integer_before,integer_after;
//	temp_address=start_address+data_count;
//	integer_before=start_address>>8;
//	integer_after=temp_address>>8;
//	if(integer_after>integer_before)
//		{
//			AT25df16_read_data(user_parameter.spi_flash_buffer,(integer_after<<8)-start_address,start_address);
//			AT25df16_read_data(&user_parameter.spi_flash_buffer[(integer_after<<8)-start_address],data_count-((integer_after<<8)-start_address),integer_after<<8);
//			}
//	else
//		{
//			AT25df16_read_data(user_parameter.spi_flash_buffer,data_count,start_address);
//			}
//	
//	}
//	
//void AT25df16_read_ID(unsigned char *pbuffer)//debug
//{	
//	unsigned char data_count=3;
//	AT25_CS_L;// Ê¹ÄÜAT25
//	Delay_us(10);
//	Spi_write_data(0x9f);//·¢¶ÁID
//	while(data_count--)
//	{
//		Spi_write_data(0xff);
//		*pbuffer=Spi_read_data();
//		pbuffer++;
//		}	
//	AT25_CS_H;// ²»Ê¹ÄÜAT25
//	}

void Spi_Flash_Data_read(void)//?SPI??
{
	unsigned short temp_read_start_address,temp_read_end_address,k;
	unsigned int tt;
	
	if(run_parameter.reserved_parameter15==1001)
		{
			temp_read_start_address=run_parameter.reserved_parameter13;
			temp_read_end_address=run_parameter.reserved_parameter14;
			if((temp_read_start_address>8190)||(temp_read_end_address>8190)||(temp_read_start_address>=temp_read_end_address))
				{
					run_parameter.reserved_parameter13=0;
					run_parameter.reserved_parameter14=0;
					run_parameter.reserved_parameter15=0;
					return;
					}
			for(tt=temp_read_start_address;tt<temp_read_end_address;tt++)
  			{
  				
  				M25P16_read_data_anywhere(256,tt<<8);
  				for(k=0;k<256;k++)
  				{
  					IO1SET	=EN_485_DE;
						Delay_us(1);
						Uart1_SentByte(user_parameter.spi_flash_buffer[k]);
						IO1CLR	=EN_485_DE;
  					  					
  				}
				}
			IO1SET	=EN_485_DE;
			Delay_us(1);
			Uart1_SentByte(0xaa);
			Uart1_SentByte(0xbb);
			Uart1_SentByte(0xcc);
			Uart1_SentByte(0xdd);
			IO1CLR	=EN_485_DE;	
			run_parameter.reserved_parameter13=0;
			run_parameter.reserved_parameter14=0;
			run_parameter.reserved_parameter15=0;
			}
	
	}
	
void Init_interrupt(void)//hugo add 
	{
		
	VICIntSelect = 0x00000000;												// ????????IRQ??
	VICVectCntl2 = 0x20 | 7;													// UART1???IRQ slot0,??????
  VICVectAddr2=(unsigned int)Uart_IRQ;//Ñ¡ÔñÖÐ¶ÏÈë¿ÚµØÖ·µÄ³ÌÐ
	VICIntEnable |= (1 << 0x07);							// ??UART1??
		}	

void Data_Analysis(unsigned char data)//hugo add 
{
	switch(user_parameter.recept_point)
	{
		case 0:
						if(run_parameter.unit_id.ubit.lo==data)
							{
								user_parameter.recept_data_buffer[user_parameter.recept_point]=run_parameter.unit_id.ubit.lo;
								user_parameter.recept_point=1;
								}
						else
							{
								user_parameter.recept_point=0;
								}					
						break;
		case 1:
						 switch(data)
						 {
						 	case 0x03://¶Á
						 						user_parameter.recept_function_code=data;
						 						user_parameter.recept_data_buffer[user_parameter.recept_point]=data;
						 						user_parameter.recept_point=12;
						 						break;
						 		
						 	case 0x06://Ð´
						 						user_parameter.recept_function_code=data;
						 						user_parameter.recept_data_buffer[user_parameter.recept_point]=data;
						 						user_parameter.recept_point=22;
						 						break;
						 						
						 	default:
						 						//Òì³£»Ø¸´
												/*user_parameter.send_buffer[0]=run_parameter.uint_id.ubit.lo;
												user_parameter.send_buffer[1]=0x83;
												user_parameter.send_buffer[2]=0x01;
												user_parameter.calculate_crc=crc16(user_parameter.send_buffer, 3);
												user_parameter.send_buffer[3]=(unsigned char)(user_parameter.calculate_crc>>8);
												user_parameter.send_buffer[4]=(unsigned char)(user_parameter.calculate_crc);	
												for(i=0;i<5;i++)
												{
													IO1SET	=EN_485_DE;
													Delay_us(1);
													Uart0_SentByte(user_parameter.send_buffer[i]);
													IO1CLR	=EN_485_DE;
													}
												//Òì³£»Ø¸´
						 						user_parameter.flag.ubit.recept_data_error=1;
						 						user_parameter.flag.ubit.recept_crc_error=0;
						 						user_parameter.recept_point=0;
						 					  break;*/
						 						user_parameter.recept_function_code=data;
						 						user_parameter.recept_data_buffer[user_parameter.recept_point]=data;
						 						user_parameter.recept_point=12;
						 						break;
						 	}	
						
						break;
		case 12://¶Á
						user_parameter.start_address.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-10]=data;
						user_parameter.recept_point=13;	
						break;
		case 13://¶Á
						user_parameter.start_address.ubit.lo=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-10]=data;
						user_parameter.recept_point=14;	
						break;
		case 14://¶Á
						user_parameter.register_count.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-10]=data;
						user_parameter.recept_point=15;	
						break;
		case 15://¶Á
						user_parameter.register_count.ubit.lo=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-10]=data;
						user_parameter.recept_point=36;	
						break;											
		case 22://Ð´
						user_parameter.start_address.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-20]=data;
						user_parameter.recept_point=23;	
						break;
		case 23://Ð´
						user_parameter.start_address.ubit.lo=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-20]=data;
						user_parameter.recept_point=24;	
						break;
		case 24://Ð´
						user_parameter.register_count.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-20]=data;
						user_parameter.recept_point=25;	
						break;
		case 25://Ð´
						user_parameter.register_count.ubit.lo=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-20]=data;
						user_parameter.recept_point=36;	
						break;
		case 36://CRC
						user_parameter.recept_crc.ubit.lo=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-30]=data;
						user_parameter.recept_point=37;
						break;
		case 37://CRC
						user_parameter.recept_crc.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-30]=data;
						user_parameter.calculate_crc=crc16(user_parameter.recept_data_buffer, user_parameter.recept_point-31);
						if(user_parameter.recept_crc.hi_lo==user_parameter.calculate_crc)
							{
								user_parameter.flag.ubit.recept_ok=1;
								user_parameter.flag.ubit.recept_crc_error=0;
						 		user_parameter.flag.ubit.recept_data_error=0;
								user_parameter.recept_point=0;
								}
						else
							{
						 		user_parameter.flag.ubit.recept_crc_error=1;
						 		user_parameter.flag.ubit.recept_data_error=0;
								user_parameter.recept_point=0;
								}	
						break;									
		default:
						
						user_parameter.recept_point=0;
						break;								
		}
	 
	}

void Data_Ack_Processor(void)//hugo add 
{
	unsigned short i;
	unsigned short temp_point_count,temp_point_halfcount;
	unsigned short temp_crc;
	unsigned char *p;
	unsigned short temp_vp,temp_vmax;
	p=&(run_parameter.h2_ppm_h16.ubit.lo);
	switch(user_parameter.recept_function_code)
	{
		case 0x03://?
							user_parameter.send_buffer[0]=run_parameter.unit_id.ubit.lo;
							user_parameter.send_buffer[1]=user_parameter.recept_function_code;
							user_parameter.send_buffer[2]=(user_parameter.register_count.hi_lo<<1)+0;
							temp_point_count=user_parameter.register_count.hi_lo;
							user_parameter.register_count.hi_lo=user_parameter.register_count.hi_lo<<1;						
							p=p+(user_parameter.start_address.hi_lo<<1);
							temp_vp=user_parameter.register_count.hi_lo+(user_parameter.start_address.hi_lo<<1);
							temp_vmax=512;
							if((temp_vp>temp_vmax)||(temp_point_count>125))
								{
									//????
												user_parameter.send_buffer[0]=run_parameter.unit_id.ubit.lo;
												user_parameter.send_buffer[1]=0x83;
												user_parameter.send_buffer[2]=0x02;	
												user_parameter.calculate_crc=crc16(user_parameter.send_buffer, 3);
												user_parameter.send_buffer[4]=(unsigned char)(user_parameter.calculate_crc>>8);
												user_parameter.send_buffer[3]=(unsigned char)(user_parameter.calculate_crc);
												for(i=0;i<5;i++)
												{
													IO1SET	=EN_485_DE;
													Delay_us(1);
													Uart1_SentByte(user_parameter.send_buffer[i]);
													IO1CLR	=EN_485_DE;
													}
												//????
									user_parameter.flag.ubit.recept_data_error=1;
									break;
									}
							for(i=0;i<temp_point_count;i++)
							{
										user_parameter.send_buffer[3+i*2]=*(p+1);
										user_parameter.send_buffer[3+i*2+1]=*p;
										p+=2;	
								}
							temp_crc=crc16(user_parameter.send_buffer,user_parameter.send_buffer[2]+3);
							user_parameter.send_buffer[user_parameter.register_count.hi_lo+4]=temp_crc;
							user_parameter.send_buffer[user_parameter.register_count.hi_lo+3]=temp_crc>>8;
							for(i=0;i<(user_parameter.register_count.hi_lo+5);i++)
							{
								IO1SET	=EN_485_DE;// ???? 485 ??
								Delay_us(1);
								Uart1_SentByte(user_parameter.send_buffer[i]);
								IO1CLR	=EN_485_DE;// ???? 485 ??
								}
							
							break;
							
		case 0x06://?
							user_parameter.send_buffer[0]=run_parameter.unit_id.ubit.lo;
							user_parameter.send_buffer[1]=user_parameter.recept_function_code;
							user_parameter.send_buffer[2]=user_parameter.start_address.ubit.hi;
							user_parameter.send_buffer[3]=user_parameter.start_address.ubit.lo;
							user_parameter.send_buffer[4]=user_parameter.register_count.ubit.hi;
							user_parameter.send_buffer[5]=user_parameter.register_count.ubit.lo;
							temp_crc=crc16(user_parameter.send_buffer,6);
							user_parameter.send_buffer[7]=temp_crc;
							user_parameter.send_buffer[6]=temp_crc>>8;
							temp_point_count=((user_parameter.start_address.ubit.hi<<8)+user_parameter.start_address.ubit.lo)<<1;
							temp_point_halfcount=temp_point_count>>1;
							if(((temp_point_halfcount>0)&&(temp_point_halfcount<=3))||
								 ((temp_point_halfcount>=6)&&(temp_point_halfcount<=88))||
								 ((temp_point_halfcount>=102)&&(temp_point_halfcount<=120))||//								 ((temp_point_halfcount>=135)&&(temp_point_halfcount<=140))||
								 ((temp_point_halfcount>=157)&&(temp_point_halfcount<=200))||
								 ((temp_point_halfcount>=231)&&(temp_point_halfcount<=255)))
								{
									temp_point_count=600;//????,??????
									}
								else
								{user_parameter.function_point=temp_point_halfcount; user_parameter.flag.ubit.recept_write=1;}
							if(temp_point_count<512)
								{
									*(p+temp_point_count+1)=user_parameter.register_count.ubit.hi;
									*(p+temp_point_count)=user_parameter.register_count.ubit.lo;
									}
							else
								{
									//??????
												user_parameter.send_buffer[0]=run_parameter.unit_id.ubit.lo;
												user_parameter.send_buffer[1]=0x86;
												user_parameter.send_buffer[2]=0x02;
												user_parameter.calculate_crc=crc16(user_parameter.send_buffer, 3);
												user_parameter.send_buffer[4]=(unsigned char)(user_parameter.calculate_crc>>8);
												user_parameter.send_buffer[3]=(unsigned char)(user_parameter.calculate_crc);
												for(i=0;i<5;i++)
												{
													IO1SET	=EN_485_DE;
													Delay_us(1);
													Uart1_SentByte(user_parameter.send_buffer[i]);
													IO1CLR	=EN_485_DE;
													}
												//??????
									user_parameter.flag.ubit.recept_data_error=1;
									break;
									}									
							for(i=0;i<8;i++)
							{
								IO1SET	=EN_485_DE;// ???? 485 ??
								Delay_us(1);
								Uart1_SentByte(user_parameter.send_buffer[i]);
								IO1CLR	=EN_485_DE;// ???? 485 ??
								}
							break;
		default:
						//????
						user_parameter.send_buffer[0]=run_parameter.unit_id.ubit.lo;
						user_parameter.send_buffer[1]=0x83;
						user_parameter.send_buffer[2]=0x01;
						user_parameter.calculate_crc=crc16(user_parameter.send_buffer, 3);
						user_parameter.send_buffer[4]=(unsigned char)(user_parameter.calculate_crc>>8);
						user_parameter.send_buffer[3]=(unsigned char)(user_parameter.calculate_crc);	
						for(i=0;i<5;i++)
						{
							IO1SET	=EN_485_DE;
							Delay_us(1);
							Uart1_SentByte(user_parameter.send_buffer[i]);
							IO1CLR	=EN_485_DE;
							}
						//????
						user_parameter.flag.ubit.recept_crc_error=1;
						user_parameter.flag.ubit.recept_data_error=0;
						break;										
		}
	user_parameter.flag.ubit.recept_ok=0;	
	}

unsigned short crc16(unsigned char *puchMsg, unsigned short usDataLen) 
{
  
unsigned char uchCRCHi = 0xFF ; /* ¸ßCRC×Ö½Ú³õÊ¼»¯ */
unsigned char uchCRCLo = 0xFF ; /* µÍCRC ×Ö½Ú³õÊ¼»¯ */
unsigned int uIndex ; /* CRCÑ­»·ÖÐµÄË÷Òý */
while (usDataLen--) /* ´«ÊäÏûÏ¢»º³åÇø */
{
  
uIndex = uchCRCHi ^ *puchMsg++ ; /* ¼ÆËãCRC */
uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
uchCRCLo = auchCRCLo[uIndex] ; 
 
} 
return (uchCRCHi << 8 | uchCRCLo) ; 
 
}

void UpData_ModbBus(REALTIMEINFO *Time)
{
	unsigned char Temp;
	unsigned char Tens, units;
	REALTIMEINFO TimeBCD;   //BCDÂëÊ±¼ä		
/////////////////////////////////////////////////////ModBusÐ­Òé±äÁ¿////////////////////////////////////////////////////////////////////

////0¡¢1
//run_parameter.h2_ppm_h16.hilo=(unsigned int)(SenseData.H2G*10000)>>16;//ÓÍÖÐÇâ(´øÐ¡Êýµã)
//run_parameter.h2_ppm_l16.hilo=(unsigned int)(SenseData.H2G*10000)&0xFFFF;	

////2¡¢3	
//run_parameter.h2_ppm_dga_h16.hilo=(unsigned int)(SenseData.H2G*10000)>>16;//ÓÍÖÐÇâ
//run_parameter.h2_ppm_dga_l16.hilo=(unsigned int)(SenseData.H2G*10000)&0xFFFF;
//	
////4¡¢5
//run_parameter.h2_ppm_max_h16.hilo=(unsigned int)(SenseData.h2)>>16;//N2 Air Çâ
//run_parameter.h2_ppm_max_l16.hilo=(unsigned int)(SenseData.h2)&0xFFFF;
//	
////6
//run_parameter.die_temperature_celsius.hilo=(unsigned int)(SenseData.St*100.F);//SenseÎÂ¶È

////7
//run_parameter.pcb_temperature_celsius.hilo=(unsigned int)(SenseData.Pt*100.F);//PCBÎÂ¶È	

////8
//run_parameter.oil_temperature_celsius.hilo=(unsigned int)(SenseData.Ot*100.F);//ÓÍÎÂ
////9¡¢10

////11¡¢12
//run_parameter.h2_ppm_24hour_average_h16.hilo=(unsigned int)(SenseData.H2SldAv*10000)>>16;//24h±ä»¯ÂÊ
//run_parameter.h2_ppm_24hour_average_l16.hilo=(unsigned int)(SenseData.H2SldAv*10000)&0xFFFF;

////13¡¢14
//run_parameter.h2_ppm_DRC_h16.hilo=(unsigned int)(SenseData.DayROC*10000)>>16;//Ìì±ä»¯ÂÊ
//run_parameter.h2_ppm_DRC_l16.hilo=(unsigned int)(SenseData.DayROC*10000)&0xFFFF;

////15¡¢16
//run_parameter.h2_ppm_WRC_h16.hilo=(unsigned int)(SenseData.WeekROC*10000)>>16;//ÖÜ±ä»¯ÂÊ
//run_parameter.h2_ppm_WRC_l16.hilo=(unsigned int)(SenseData.WeekROC*10000)&0xFFFF;

////17¡¢18
//run_parameter.h2_ppm_MRC_h16.hilo=(unsigned int)(SenseData.MonthROC*10000)>>16;//ÔÂ±ä»¯ÂÊ
//run_parameter.h2_ppm_MRC_l16.hilo=(unsigned int)(SenseData.MonthROC*10000)&0xFFFF;
//19-30±£Áô

        //*
	Temp = Time->SpecificTime.sec;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.sec = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.min;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.min = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.hour;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.hour = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.week;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.week = ((Tens << 4) & 0xF0) | (units & 0x0F);	
	
	Temp = Time->SpecificTime.day;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.day = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.month;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.month = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.year;		//
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.year = ((Tens << 4) & 0xF0) | (units & 0x0F);
	
  			run_parameter.reserved_parameter35=(0x20<<8 | TimeBCD.SpecificTime.year);//?
  			run_parameter.reserved_parameter34=(TimeBCD.SpecificTime.month<<8 | TimeBCD.SpecificTime.day);
  			run_parameter.reserved_parameter36=TimeBCD.SpecificTime.hour;
  			run_parameter.reserved_parameter37=(TimeBCD.SpecificTime.min<<8 | TimeBCD.SpecificTime.sec);

//if(Runtimes>=0xFFFFFFFFFFFFFFFF)Runtimes=0;
//run_parameter.run_time_in_secends_hh32.hilo=((Runtimes>>48)&0xFFFF);
//run_parameter.run_time_in_secends_h32.hilo=((Runtimes>>32)&0xFFFF);
//run_parameter.run_time_in_secends_ll32.hilo=((Runtimes>>16)&0xFFFF);
//run_parameter.run_time_in_secends_l32.hilo=Runtimes&0xFFFF;
}

///******************************************************************************/
///***************************      MAIN PROGRAM      ***************************/
///******************************************************************************/
int Init_ModBus (void)  
{
  int i;
	unsigned char ch;	
	float date;
	char month[2],day[2];
	char year[5];
	
//	Init_Pll();
	Init_io();	
//  init_serial();/*Gu*/
//  init_spi();
//	Init_timer();/*Gu*/
	Init_interrupt();
	//debug parameter init
	run_parameter.reserved_parameter100=0x0240;//255	


	
///////////////////////////////////////////////////////ModBusÐ­Òé³£Á¿////////////////////////////////////////////////////////////////////
user_parameter.flag.ubit.recept_write=0;
//31-40
//sprintf(TempBuffer,"x",ProtectType);
//strcpy(run_parameter.model_number.model_number_str,productInf.ProtectType);	
for(i=0;i<20;i++)
{
ch=run_parameter.model_number.model_number_str[i];	
run_parameter.model_number.model_number_str[i]=run_parameter.model_number.model_number_str[i+1];
run_parameter.model_number.model_number_str[i+1]=ch;i++;
}
//41-50
//sprintf(TempBuffer,"x",SerialNum);
//strcpy(run_parameter.product_serial_number.product_serial_number_str,productInf.SerialNum);
for(i=0;i<20;i++)
{
ch=run_parameter.product_serial_number.product_serial_number_str[i];	
run_parameter.product_serial_number.product_serial_number_str[i]=run_parameter.product_serial_number.product_serial_number_str[i+1];
run_parameter.product_serial_number.product_serial_number_str[i+1]=ch;i++;
}

//51-60
//sprintf(TempBuffer,"x",SenseNum);
//strcpy(run_parameter.sensor_serial_number.sensor_serial_number_str,productInf.SenseNum);
for(i=0;i<20;i++)
{
ch=run_parameter.sensor_serial_number.sensor_serial_number_str[i];	
run_parameter.sensor_serial_number.sensor_serial_number_str[i]=run_parameter.sensor_serial_number.sensor_serial_number_str[i+1];
run_parameter.sensor_serial_number.sensor_serial_number_str[i+1]=ch;i++;
}
//61-70
//sprintf(TempBuffer,"x","10L114-30");
//strcpy(run_parameter.sensor_board_serial_number.sensor_board_serial_number_str,productInf.SensorBoardNum);//productInf.SensorBoardNum
for(i=0;i<20;i++)
{
ch=run_parameter.sensor_board_serial_number.sensor_board_serial_number_str[i];	
run_parameter.sensor_board_serial_number.sensor_board_serial_number_str[i]=run_parameter.sensor_board_serial_number.sensor_board_serial_number_str[i+1];
run_parameter.sensor_board_serial_number.sensor_board_serial_number_str[i+1]=ch;i++;
}

//71-80
//sprintf(TempBuffer,"x","10L137-31");InterfaceBoardNum
//strcpy(run_parameter.interface_board_serial_number.interface_board_serial_number_str,productInf.InterfaceBoardNum);//productInf.InterfaceBoardNum
for(i=0;i<20;i++)
{
ch=run_parameter.interface_board_serial_number.interface_board_serial_number_str[i];	
run_parameter.interface_board_serial_number.interface_board_serial_number_str[i]=run_parameter.interface_board_serial_number.interface_board_serial_number_str[i+1];
run_parameter.interface_board_serial_number.interface_board_serial_number_str[i+1]=ch;i++;
}

//81¡¢82
run_parameter.manufacturing_date.month=01;//
run_parameter.manufacturing_date.day=01;
run_parameter.manufacturing_date.year=2000;

//83¡¢84
//year[0]=CalDate[0];year[1]=CalDate[1];year[2]=CalDate[2];year[3]=CalDate[3];
//month[0]=CalDate[4];month[1]=CalDate[5];
//day[0]=CalDate[6];day[1]=CalDate[7];

date=atof(month);
run_parameter.factory_calibration_date.month=(unsigned char)date;//
date=atof(day);
run_parameter.factory_calibration_date.day=(unsigned char)date;
date=atof(year);
run_parameter.factory_calibration_date.year=(unsigned short)date;

//85¡¢86
//run_parameter.field_calibration_date.month=((cmd_ConfigData.Caldate&0xFFFF)>>8);
//run_parameter.field_calibration_date.day=(cmd_ConfigData.Caldate & 0xFF);
//run_parameter.field_calibration_date.year=cmd_ConfigData.Caldate>>16;

//87¡¢88
run_parameter.dissolved_gas_calibration_date.month=01;//
run_parameter.dissolved_gas_calibration_date.day=01;
run_parameter.dissolved_gas_calibration_date.year=2000;

//89-98
//sprintf(TempBuffer,"x",FirmwareREV);
//strcpy(run_parameter.firmware_revesion.firmware_revesion_str,FirmwareREV);
for(i=0;i<20;i++)
{
ch=run_parameter.firmware_revesion.firmware_revesion_str[i];	
run_parameter.firmware_revesion.firmware_revesion_str[i]=run_parameter.firmware_revesion.firmware_revesion_str[i+1];
run_parameter.firmware_revesion.firmware_revesion_str[i+1]=ch;i++;
}
//99-110±£Áô

//111
//run_parameter.status_flag.all=0x00;//16Î»
run_parameter.status_flag.ubit.senser_state0=0;
run_parameter.status_flag.ubit.senser_state1=0;
run_parameter.status_flag.ubit.senser_state2=0;	//000- Î´×¼±¸ºÃ 001-Õý³£ÔËÐÐ 010-²âÁ¿ÓÍÎÂ 011-»ù×¼Ñ­»· 100-¹ýÎÂ		
run_parameter.status_flag.ubit.reserved7=0;				 
run_parameter.status_flag.ubit.relay1=0;//¼ÌµçÆ÷1			  
run_parameter.status_flag.ubit.relay2=0;//¼ÌµçÆ÷2				 
run_parameter.status_flag.ubit.relay3=0;//¼ÌµçÆ÷3				  		
run_parameter.status_flag.ubit.reserved6=0;				 
run_parameter.status_flag.ubit.reserved5=0;				  
run_parameter.status_flag.ubit.reserved4=0;				  
run_parameter.status_flag.ubit.reserved3=0;				  
run_parameter.status_flag.ubit.reserved2=0;				  
run_parameter.status_flag.ubit.error=0;						  
run_parameter.status_flag.ubit.reserved1=0;				  
run_parameter.status_flag.ubit.new_data_available=0; 
run_parameter.status_flag.ubit.unit_ready=1;
//112-120±£Áô


//150
//run_parameter.unit_id.ubit.lo=systemInf.ModbusID;//¶ÁID	
run_parameter.unit_id.ubit.lo=1;	
//201-210
//sprintf(TempBuffer,"x","user 01");
//strcpy(run_parameter.own_id.own_id_str,productInf.Own_id);
for(i=0;i<20;i++)
{
ch=run_parameter.own_id.own_id_str[i];	
run_parameter.own_id.own_id_str[i]=run_parameter.own_id.own_id_str[i+1];
run_parameter.own_id.own_id_str[i+1]=ch;i++;
}
//211-220
//sprintf(TempBuffer,"x","Sub-Station 01");
//strcpy(run_parameter.sub_station_id.sub_station_id_str,productInf.Sub_station_id);
for(i=0;i<20;i++)
{
ch=run_parameter.sub_station_id.sub_station_id_str[i];	
run_parameter.sub_station_id.sub_station_id_str[i]=run_parameter.sub_station_id.sub_station_id_str[i+1];
run_parameter.sub_station_id.sub_station_id_str[i+1]=ch;i++;
}
//221-230
//sprintf(TempBuffer,"x","Transformer 01");
//strcpy(run_parameter.transformer_id.transformer_id_str,productInf.Transformer_id);
for(i=0;i<20;i++)
{
ch=run_parameter.transformer_id.transformer_id_str[i];	
run_parameter.transformer_id.transformer_id_str[i]=run_parameter.transformer_id.transformer_id_str[i+1];
run_parameter.transformer_id.transformer_id_str[i+1]=ch;i++;
}
//231-255±£Áô


DS1390_CS_H;
LC512_CS_H;

  		//AT25df16_reset();
M25P16_reset();
  		//AT25df16_read_ID(user_parameter.spi_flash_buffer);
M25P16_read_ID(user_parameter.spi_flash_buffer);
//			flash_read(&SpiFlash_Addr[0], 4, 120);
e2prom512_read(&SpiFlash_Addr[0], 4, 120);
			
return 0;
}


/******************************************************************************/
//if(user_parameter.flag.ubit.recept_write==1)
//{RW_ModBus_Data();}
/******************************************************************************/
int RW_ModBus_Data (void)  
{

	unsigned int wr;
	unsigned int db_H2ppm;	
/////////////////////////////////////////////////////ModBusÐ­Òé³£Á¿write////////////////////////////////////////////////////////////////////
////	UARTprintf("point=%d\r\n",user_parameter.function_point);
	//99-110±£Áô
switch (user_parameter.function_point)
{	
//************************************************
//¶ÁFlash
  case 101:
	{
  Spi_Flash_Data_read();
	break;		
	}		
//************************************************	
  case 121:
	{		
//    UARTprintf("Current H2 value is %5u ppm H2\n",(unsigned int)((SenseData.h2*10000)/20));
//    cmd_ConfigData.h2cdata_ka=SenseData.h2;		
//    float_char(cmd_ConfigData.h2cdata_ka,systemInf.Cal_Ka);
 	break;		
	}
  case 122:
	{	
//Ê×ÏÈÐ´126 127   128 129 ÔÚÐ´122 Ð´0È·ÈÏ
    db_H2ppm = (run_parameter.h2_ppm_calibration_gas_h16.hilo<<16 |run_parameter.h2_ppm_calibration_gas_l16.hilo);
	
//		UARTprintf("Set hydrogen to %d ppm\r\n",db_H2ppm);//126 127	
//    cmd_ConfigData.h2cdata_kb = (((float)(db_H2ppm*20)/10000.F) - cmd_ConfigData.h2cdata_ka);
//    float_char(cmd_ConfigData.h2cdata_kb,systemInf.Cal_Kb);
////×Ô¶¯Ð´ÈëÐ£×¼Ê±¼ä
//    cmd_ConfigData.Caldate=(CurrentTime.SpecificTime.year<<16 | CurrentTime.SpecificTime.month<<8 | CurrentTime.SpecificTime.day);		 		
//    UARTprintf("Calibration Gas finished\n");
	break;		
	}
  case 123:
	{	
//Ê×ÏÈÐ´126 127   128 129 ÔÚÐ´122 Ð´0È·ÈÏ
    db_H2ppm = (run_parameter.h2_ppm_calibration_gas_h16.hilo<<16 |run_parameter.h2_ppm_calibration_gas_l16.hilo);
	
//		UARTprintf("Set hydrogen to %d ppm\r\n",db_H2ppm);//126 127	
//    cmd_ConfigData.h2cdata_kb = (((float)(db_H2ppm*20)/10000.F) - cmd_ConfigData.h2cdata_ka);
//    float_char(cmd_ConfigData.h2cdata_kb,systemInf.Cal_Kb);
////×Ô¶¯Ð´ÈëÐ£×¼Ê±¼ä
//    cmd_ConfigData.Caldate=(CurrentTime.SpecificTime.year<<16 | CurrentTime.SpecificTime.month<<8 | CurrentTime.SpecificTime.day);		 		
//    UARTprintf("Calibration Gas finished\n");	
	break;		
	}	
	
	case 150:
	{
//150
//		systemInf.ModbusID=hex2dec(run_parameter.uint_id.ubit.lo);//systemInf.CallID=(systemInf.CallID-50);
//		systemInf.ModbusID=(unsigned char)run_parameter.uint_id.ubit.lo;//systemInf.CallID=(systemInf.CallID-50);		
//    UARTprintf("Set ModbusID to:%d\r\n", (unsigned int)systemInf.ModbusID);		
//run_parameter.uint_id.ubit.lo=0x01;//Ä¬ÈÏID
	break;
	}
	
	case 133:
	{	
//130 131 132 133
		
RealTime_Modbus.Real_Time[1]=run_parameter.reserved_parameter341>>8;//ÔÂ
RealTime_Modbus.Real_Time[2]=run_parameter.reserved_parameter341&0xFF;//ÈÕ	
RealTime_Modbus.Real_Time[0]=run_parameter.reserved_parameter35&0xFF;//Äê
		
RealTime_Modbus.Real_Time[4]=run_parameter.reserved_parameter36&0xFF;//Ê±	
RealTime_Modbus.Real_Time[5]=run_parameter.reserved_parameter37>>8;//·Ö
RealTime_Modbus.Real_Time[6]=run_parameter.reserved_parameter37&0xFF;//Ãë
    UARTprintf("Change time to 20%x-%x-%x %x:%x:%x",\
	  (long int)RealTime_Modbus.Real_Time[0],\
		(long int)RealTime_Modbus.Real_Time[1],\
    (long int)RealTime_Modbus.Real_Time[2],(long int)RealTime_Modbus.Real_Time[4],\
    (long int)RealTime_Modbus.Real_Time[5],(long int)RealTime_Modbus.Real_Time[6]);
		
//*/
	Write1390(REG_STATUS,0x80); //Ð´ÈëÔÊÐí
	Write1390(REG_YEAR,RealTime_Modbus.Real_Time[0]); 
	Write1390(REG_MONTH,RealTime_Modbus.Real_Time[1]);
	Write1390(REG_DAY,RealTime_Modbus.Real_Time[2]);
////	Write1390(REG_WEEK,TimeBCD.SpecificTime.week);	
	Write1390(REG_HOUR,RealTime_Modbus.Real_Time[4]);
	Write1390(REG_MIN,RealTime_Modbus.Real_Time[5]);
	Write1390(REG_SEC,RealTime_Modbus.Real_Time[6]);
	Write1390(REG_STATUS,0x00); //½ûÖ¹Ð´Èë		
		
//    DS1390_AdjustTime(&RealTime_Modbus);
break;
	}
case 142:
{
//141-142	
//cmd_ConfigData.H2low = (double)(((run_parameter.h2_ppm_report_low_h16.hilo<<16 |run_parameter.h2_ppm_report_low_l16.hilo)/10000.F)*20.F);
//float_char(cmd_ConfigData.H2low,systemInf.LowH2);		
break;
}	
case 144:
{
//143-144	
//cmd_ConfigData.H2high = (double)(((run_parameter.h2_ppm_report_high_h16.hilo<<16 |run_parameter.h2_ppm_report_high_l16.hilo)/10000.F)*20.F);		
//float_char(cmd_ConfigData.H2high,systemInf.HighH2);		
	break;
}
	case 148:
	{
//145 146 147 148
//cmd_ConfigData.LowmA=(double)((run_parameter.h2_ppm_out_current_low.hilo)/100.F);
//cmd_ConfigData.HighmA=(double)((run_parameter.h2_ppm_out_current_high.hilo)/100.F);
//cmd_ConfigData.ErrmA=(double)((run_parameter.h2_ppm_error_out_current.hilo)/100.F);
//cmd_ConfigData.NotRmA=(double)((run_parameter.h2_ppm_no_ready_out_current.hilo)/100.F);

//    float_char(cmd_ConfigData.LowmA,systemInf.Out_mA_Low);//float?char  ??????
//    float_char(cmd_ConfigData.HighmA,systemInf.Out_mA_High);
//    float_char(cmd_ConfigData.ErrmA,systemInf.Out_mA_Err);
//    float_char(cmd_ConfigData.NotRmA,systemInf.Out_mA_NotR);
break;
	}
//150
	
//152 153
case 153:
{	
//cmd_ConfigData.MaxAlertH2 = (double)((run_parameter.h2_ppm_alert_low_h16.hilo<<16 |run_parameter.h2_ppm_alert_low_l16.hilo)/10000.F);
//float_char(cmd_ConfigData.MaxAlertH2,systemInf.AlertH2);		
break;
}

//154 155
case 155:
{	
//cmd_ConfigData.MaxAlarmH2 = (double)((run_parameter.h2_ppm_alarm_low_h16.hilo<<16 |run_parameter.h2_ppm_alarm_low_l16.hilo)/10000.F);
//float_char(cmd_ConfigData.MaxAlarmH2,systemInf.AlarmH2);		
break;
}

//156
case 156:
{	
//cmd_ConfigData.MaxAlarmOil = (double)(run_parameter.OilTemp_Alarm_celsius.hilo/100.F);
//float_char(cmd_ConfigData.MaxAlarmOil,systemInf.AlarmOil);		
break;
}

	case 210:
	{
//				 wr=Msg8StartAdr;//????0x0100?  ???257????						 
// 				 strcpy(productInf.Own_id, run_parameter.own_id.own_id_str);//??        write_opera);//    cmd_analyze.processed_buf
//	       flash_write (&productInf.Own_id[0],EElength, wr);//wr????EE??
break;
}
		case 220:
	{
//				 wr=Msg9StartAdr;//????0x0100?  ???257????						 
// 				 strcpy(productInf.Sub_station_id, run_parameter.sub_station_id.sub_station_id_str);//??        write_opera);//    cmd_analyze.processed_buf
//	       flash_write (&productInf.Sub_station_id[0],EElength, wr);//wr????EE??
break;
}
		case 230:
	{
//				 wr=Msg10StartAdr;//????0x0100?  ???257????						 
// 				 strcpy(productInf.Transformer_id,run_parameter.transformer_id.transformer_id_str);//??        write_opera);//    cmd_analyze.processed_buf
//	       flash_write (&productInf.Transformer_id[0],EElength, wr);//wr????EE??
break;
}
//231-255±£Áô
}

user_parameter.flag.ubit.recept_write=0;			
//return 0;
//wFLASHconfig(sys_addr,(unsigned char *)&systemInf);
return 0;
}


//void Spi_Flash_Addr_RW(unsigned long ddr)
//{
//SpiFlash_Addr[0]=((ddr & 0xFFFFFFFF) >> 24);
//SpiFlash_Addr[1]=((ddr & 0xFFFFFF) >> 16);
//SpiFlash_Addr[2]=((ddr & 0xFFFF) >> 8);
//SpiFlash_Addr[3]=ddr&0xFF;			
//flash_write(&SpiFlash_Addr[0],4, 120);	
//}

//unsigned long Spi_Flash_Addr_R(void)
//{
//unsigned int i=0;
//unsigned int wr;	
//unsigned long ddr;	
//flash_read(&SpiFlash_Addr[0], 4, 120);
//ddr=((unsigned long)SpiFlash_Addr[0]<<24 | SpiFlash_Addr[1]<<16 | SpiFlash_Addr[2]<<8 | SpiFlash_Addr[3]);

//if(ddr==0xFFFFFFFF)
//{
//ddr=0;
//UARTprintf("Erase Sector0\r\n");	
//UARTprintf("...wait...\r\n");
//EE25LC_Erase_Sector(0);//²Á³ýÊ×ÉÈÇø	
//AT25df16_erase_4Kmap(0);//²Á³ýÊ×ÉÈÇø	

////Ö´ÐÐÄ¬ÈÏÅäÖÃ.		
//		cmd_ConfigData.h2cdata_kb=0;//????kb 
//    float_char(cmd_ConfigData.h2cdata_kb,systemInf.Cal_Kb);
//		cmd_ConfigData.Caldate=run_parameter.factory_calibration_date.year<<16 | run_parameter.factory_calibration_date.month<<8 | run_parameter.factory_calibration_date.day;//??????
//    systemInf.CaliDate[0]=((cmd_ConfigData.Caldate>>16)&0xFFFF)>>8; //| (Caldate&0xFFFF)>>8
//    systemInf.CaliDate[1]=(cmd_ConfigData.Caldate>>16)&0xFF;    
//    systemInf.CaliDate[2]=((cmd_ConfigData.Caldate&0xFFFF)>>8);
//    systemInf.CaliDate[3]=(cmd_ConfigData.Caldate&0xFF);
//		//??????		
//    cmd_ConfigData.inter_time = 30;//??30?     
//    systemInf.tt[0]=(unsigned char)((cmd_ConfigData.inter_time&0xFFFF)>>8);//??????
//    systemInf.tt[1]=(unsigned char)(cmd_ConfigData.inter_time&0xFF);//??????
//		//??????		
//		cmd_ConfigData.H2low  = RangeLow;//??200-40000ppm
//    cmd_ConfigData.H2high  = RangeHigh;
//    float_char(cmd_ConfigData.H2low,systemInf.LowH2);
//    float_char(cmd_ConfigData.H2high,systemInf.HighH2);	    		
//		//??????		
//    cmd_ConfigData.MaxAlertH2 = 0.4;//??Alert:10000ppm  Alarm:20000ppm					   
//		cmd_ConfigData.MaxAlarmH2 = 0.5;
//    float_char(cmd_ConfigData.MaxAlertH2,systemInf.AlertH2);
//    float_char(cmd_ConfigData.MaxAlarmH2,systemInf.AlarmH2);	 			
//		//??????    
//    cmd_ConfigData.LowmA=4.0;//??4-20mA   3.5  2.0
//    cmd_ConfigData.HighmA=20.0;
//    cmd_ConfigData.ErrmA=3.5;
//    cmd_ConfigData.NotRmA=2.0;
//    float_char(cmd_ConfigData.LowmA,systemInf.Out_mA_Low);//float?char  ??????
//    float_char(cmd_ConfigData.HighmA,systemInf.Out_mA_High);
//    float_char(cmd_ConfigData.ErrmA,systemInf.Out_mA_Err);
//    float_char(cmd_ConfigData.NotRmA,systemInf.Out_mA_NotR);
//		//??????    
//    cmd_ConfigData.LowVout=0.5;//??0.5-4.5V   5.0  0.0
//    cmd_ConfigData.HighVout=4.5;
//    cmd_ConfigData.ErrVout=5.0;
//    cmd_ConfigData.NotRVout=0.0;
//    float_char(cmd_ConfigData.LowVout,systemInf.Out_Vout_Low);
//    float_char(cmd_ConfigData.HighVout,systemInf.Out_Vout_High);
//    float_char(cmd_ConfigData.ErrVout,systemInf.Out_Vout_Err);
//    float_char(cmd_ConfigData.NotRVout,systemInf.Out_Vout_NotR);
//		//????????
//		cmd_ConfigData.OutCi_ka=1.0;//????ka
//		cmd_ConfigData.OutCi_kb=0;//????kb 
//    float_char(cmd_ConfigData.OutCi_ka,systemInf.OutCi_Ka);
//    float_char(cmd_ConfigData.OutCi_kb,systemInf.OutCi_Kb);

//    cmd_ConfigData.OutCv_ka=1.0;//????ka
//		cmd_ConfigData.OutCv_kb=0;//????kb 
//    float_char(cmd_ConfigData.OutCv_ka,systemInf.OutCv_Ka);
//    float_char(cmd_ConfigData.OutCv_kb,systemInf.OutCv_Kb);		
//		
//		wFLASHconfig(sys_addr,(unsigned char *)&systemInf);//??????! 
//    UARTprintf("Returns to last factory calibration data\n");
//    UARTprintf("...Wait......\n"); 	

//for(i=0;i<10;i++)
//{
//switch(i)
//{
//   case 0:
//	 				wr=Msg1StartAdr;			 		  	
//					memset(buffer,0,CMD_BUF_LEN);				 
// 				  strcpy(buffer, "3000");				
//	        flash_write (&buffer[0],EElength, wr);			 
//			   break;
//   case 1: 					 
//				 wr=Msg2StartAdr;		 				  	
//					memset(buffer,0,CMD_BUF_LEN);				 
// 				  strcpy(buffer, "D0000001");
//	        flash_write (&buffer[0],EElength, wr);				 
//			   break;
//   case 2: 					 
//				 wr=Msg3StartAdr;				 				  	
//					memset(buffer,0,CMD_BUF_LEN);			 
// 				  strcpy(buffer, "1.0");	
//	        flash_write (&buffer[0],EElength, wr);			 
//			   break;
//   case 3: 					 
//				 wr=Msg4StartAdr;			 				  	
//					memset(buffer,0,CMD_BUF_LEN);				 
// 				  strcpy(buffer, "S2.3.01999");		
//	        flash_write (&buffer[0],EElength, wr);			 
//			   break;
//   case 4: 				 
//				 wr=Msg5StartAdr;				 			  	
//					memset(buffer,0,CMD_BUF_LEN);					 
// 				  strcpy(buffer, "4WRes-200V3");			
//	        flash_write (&buffer[0],EElength, wr);			 
//			   break;
//   case 5: 					 
//				 wr=Msg6StartAdr;			 				  	
//					memset(buffer,0,CMD_BUF_LEN);			 
// 				  strcpy(buffer, "P/N20000001");		
//	        flash_write (&buffer[0],EElength, wr);				 
//			   break;
//   case 6: 				 
//				 wr=Msg7StartAdr;			 				  	
//					memset(buffer,0,CMD_BUF_LEN);				 
// 				  strcpy(buffer, "17A-200V3-1001");		
//	        flash_write (&buffer[0],EElength, wr);			 
//			   break;
//					
//   case 7: 				 
//				 wr=Msg8StartAdr;			 				  	
//					memset(buffer,0,CMD_BUF_LEN);				 
// 				  strcpy(buffer, "1");			
//	        flash_write (&buffer[0],EElength, wr);			 
//			   break;
//   case 8: 				 
//				 wr=Msg9StartAdr;		 			  	
//					memset(buffer,0,CMD_BUF_LEN);				 
// 				  strcpy(buffer, "2");			
//	        flash_write (&buffer[0],EElength, wr);				 
//			   break;
//   case 9: 				 
//				 wr=Msg10StartAdr;			 			  	
//					memset(buffer,0,CMD_BUF_LEN);				 
// 				  strcpy(buffer, "3");			
//	        flash_write (&buffer[0],EElength, wr);				 
//			   break;										
//   default :
//			   break;				 
//}
//    UARTprintf("...Wait...SAVED - Done\n");
//    ReadEEProductInf();//

//}

//EEpromData_Addr = EE_Sector1_addr;//??	 
//EEPROM_Addr[0]=((EEpromData_Addr & 0xFFFF) >> 8);
//EEPROM_Addr[1]=EEpromData_Addr&0xFF; 
//flash_write(&EEPROM_Addr[0],2, 124);

//EEpromAlarmData_Addr = EE_Sector3_addr;//??
//EEPROM_ALARM_Addr[0]=((EEpromAlarmData_Addr & 0xFFFF) >> 8);
//EEPROM_ALARM_Addr[1]=EEpromAlarmData_Addr&0xFF; 
//flash_write(&EEPROM_ALARM_Addr[0],2, 126);

//Spi_Flash_Addr_RW(0);
//////UARTprintf("SPI_Flash_Addr=%lu\r\n",ddr);
//return ddr;	
//}
//////UARTprintf("SPI_Flash_Addr=%lu\r\n",ddr);
//return ddr;
//}

//unsigned int save_parameter(unsigned int address)
//  		{
//	unsigned char Temp;
//	unsigned char Tens, units;				
//	REALTIMEINFO TimeBCD;   //BCDÂëÊ±¼ä
//        //*
//	Temp = CurrentTime.SpecificTime.sec;		//
//	Tens = Temp / 10;
//	units = Temp % 10;
//	TimeBCD.SpecificTime.sec = ((Tens << 4) & 0xF0) | (units & 0x0F);

//	Temp = CurrentTime.SpecificTime.min;		//
//	Tens = Temp / 10;
//	units = Temp % 10;
//	TimeBCD.SpecificTime.min = ((Tens << 4) & 0xF0) | (units & 0x0F);

//	Temp = CurrentTime.SpecificTime.hour;		//
//	Tens = Temp / 10;
//	units = Temp % 10;
//	TimeBCD.SpecificTime.hour = ((Tens << 4) & 0xF0) | (units & 0x0F);

//	Temp = CurrentTime.SpecificTime.week;		//
//	Tens = Temp / 10;
//	units = Temp % 10;
//	TimeBCD.SpecificTime.week = ((Tens << 4) & 0xF0) | (units & 0x0F);	
//	
//	Temp = CurrentTime.SpecificTime.day;		//
//	Tens = Temp / 10;
//	units = Temp % 10;
//	TimeBCD.SpecificTime.day = ((Tens << 4) & 0xF0) | (units & 0x0F);

//	Temp = CurrentTime.SpecificTime.month;		//
//	Tens = Temp / 10;
//	units = Temp % 10;
//	TimeBCD.SpecificTime.month = ((Tens << 4) & 0xF0) | (units & 0x0F);

//	Temp = CurrentTime.SpecificTime.year;		//
//	Tens = Temp / 10;
//	units = Temp % 10;
//	TimeBCD.SpecificTime.year = ((Tens << 4) & 0xF0) | (units & 0x0F);				
//				
//  			user_parameter.spi_flash_buffer[0]=0x5A;				
//				
//  			user_parameter.spi_flash_buffer[1]=0x20;//130~133 Time->SpecificTime.year
//  			user_parameter.spi_flash_buffer[2]=TimeBCD.SpecificTime.year;//?
//  			user_parameter.spi_flash_buffer[3]=TimeBCD.SpecificTime.month;
//  			user_parameter.spi_flash_buffer[4]=TimeBCD.SpecificTime.day;
//  			user_parameter.spi_flash_buffer[5]=TimeBCD.SpecificTime.hour;
//  			user_parameter.spi_flash_buffer[6]=TimeBCD.SpecificTime.min;
//  			user_parameter.spi_flash_buffer[7]=TimeBCD.SpecificTime.sec;

//run_parameter.h2_ppm_h16.hilo=(unsigned int)(SenseData.H2G*10000)>>16;//ÓÍÖÐÇâ
//run_parameter.h2_ppm_l16.hilo=(unsigned int)(SenseData.H2G*10000)&0xFFFF;
//		
//run_parameter.oil_temperature_celsius.hilo=(unsigned int)(SenseData.Ot*100.F);//ÓÍÎÂ
//	
//run_parameter.h2_ppm_DRC_h16.hilo=(unsigned int)(SenseData.DayROC*10000)>>16;//Ìì±ä»¯ÂÊ
//run_parameter.h2_ppm_DRC_l16.hilo=(unsigned int)(SenseData.DayROC*10000)&0xFFFF;

//				
//  			user_parameter.spi_flash_buffer[8]=run_parameter.h2_ppm_h16.ubit.hi;//??
//  			user_parameter.spi_flash_buffer[9]=run_parameter.h2_ppm_h16.ubit.lo;
//  			user_parameter.spi_flash_buffer[10]=run_parameter.h2_ppm_l16.ubit.hi;
//  			user_parameter.spi_flash_buffer[11]=run_parameter.h2_ppm_l16.ubit.lo;

//				
//  			user_parameter.spi_flash_buffer[12]=run_parameter.oil_temperature_celsius.ubit.hi;//??
//  			user_parameter.spi_flash_buffer[13]=run_parameter.oil_temperature_celsius.ubit.lo;
//				
//  			user_parameter.spi_flash_buffer[14]=run_parameter.h2_ppm_DRC_h16.ubit.hi;//????
//  			user_parameter.spi_flash_buffer[15]=run_parameter.h2_ppm_DRC_h16.ubit.lo;
//  			user_parameter.spi_flash_buffer[16]=run_parameter.h2_ppm_DRC_l16.ubit.hi;
//  			user_parameter.spi_flash_buffer[17]=run_parameter.h2_ppm_DRC_l16.ubit.lo;
//  			user_parameter.spi_flash_buffer[18]=0xA5;
//  			AT25df16_write_data_anywhere(19,address);
//  			address+=19;
//  			return address;				
//  			}

//void Read_Flash(void)
//{
//unsigned int i,n;

//unsigned int m;

//m=((FlashData_Addr/256)+1);
//	
//for(i=0;i<m;i++)	
//{	
//AT25df16_read_data_anywhere(256,256*i);	

//for(n=0;n<256;n++)	
//{
//								IO1SET	=EN_485_DE;// Ê¹ÄÜÊäÈë 485 ·¢ËÍ
//								Delay_us(1);
//								Uart0_SentByte(user_parameter.spi_flash_buffer[n]);
//								IO1CLR	=EN_485_DE;// Ê¹ÄÜÊäÈë 485 ·¢ËÍ	
//}
//}
//}

//				
