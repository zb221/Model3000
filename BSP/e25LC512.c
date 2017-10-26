/***********************************************
@		Description: This file is about AD7738 data.
			25LC256 pin  Name     Protoboard	
							1        CS(NOT)     A3
							2        SO          C4
							3        WP(NOT)     5v
							4        Vss         Gnd
							5        SI          C5
							6        SCK         C3
							7        HOLD(NOT)   5v
							8        Vcc         5v
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/25.

***********************************************/
#include <LPC21xx.H>                     /* LPC21xx definitions */
#include "e25LC512.h"
#include "stdio.h"

char dat[255] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
				0x77,0x7c,0x39,0x5e,0x79,0x71};//0~F????

void delay_ms(unsigned int n)
{
	unsigned int i, j;
	for(i=0;i<n;++i)
	for(j=0;j<10;++j);
}

//SPI???byte
void SPI_byte_write(const unsigned int dat)
{
	char tmp = dat, i;

	for(i=0;i<8;i++)
	{
		LC512_SCK_L;
		if((tmp&0x80)==0x80)
		{
			LC512_SI_H;
		}
		else
		{
			LC512_SI_L;
		}		
		tmp <<= 1; //??????
		LC512_SCK_H; //SCK????bit
		delay_ms(1); //?????????
	}
}

//SPI???byte
char SPI_byte_read(unsigned char dat)
{
	char tmp = 0, i;

	for(i=0;i<8;i++)
	{
		LC512_SCK_L;		
		tmp <<= 1; //????
		if(LC512_SO_IN!=0)
		{
			tmp |= 0x01;
		}else{
			tmp &= 0xFE;
		}			
		LC512_SCK_H;//SCK????bit
		delay_ms(10);
	}	
	return tmp;
}

//SPI???byte	
void SPI_nbyte_write(const unsigned int address, const unsigned char *dat, unsigned int num)
{
	char inst_wren = WREN, inst_write = WRITE;

	unsigned int i;

	LC512_CS_L;
	delay_ms(1); 
	SPI_byte_write(inst_wren); //???
	LC512_CS_H;
	delay_ms(1);

	delay_ms(1);

	LC512_CS_L;
	delay_ms(1);
	SPI_byte_write(inst_write); //???
	delay_ms(1);	
	SPI_byte_write(address/256);//???
	SPI_byte_write(address%256);//???	
	for(i=0;i<num;i++)
	{		
		SPI_byte_write(dat[i]);//???
	}	
	LC512_CS_H;
	delay_ms(1);
	RW_Status();	
}

//SPI???byte
void SPI_nbyte_read(unsigned int address, unsigned char *dat, unsigned int num)
{
	char inst = READ;
	unsigned int i;
	LC512_CS_L;
	delay_ms(1);
	SPI_byte_write(inst); //???
	delay_ms(1);	
	SPI_byte_write(address/256);//???
	SPI_byte_write(address%256);//???

	for(i=0;i<num;i++){
		dat[i]=SPI_byte_read(0);//???
	}
	LC512_CS_H;
	delay_ms(1000);	
}

void EE25LC_Erase_Sector(char sector_addr)
{
	char inst_wren = WREN, inst_write = SE;

	unsigned int address;

	address = (sector_addr*16*1024);

	LC512_CS_L;
	delay_ms(1); 
	SPI_byte_write(inst_wren); //???
	LC512_CS_H;
	delay_ms(1);

	delay_ms(1);

	LC512_CS_L;
	delay_ms(1);
	SPI_byte_write(inst_write); //???

	SPI_byte_write(address/256);//???
	SPI_byte_write(address%256);//???

	LC512_CS_H;
	delay_ms(1);	
	RW_Status();		
}

void RW_Status(void)
{
	char inst_wren = WREN, inst_write = RDSR;//¶Á×´Ì¬¼Ä´æÆ÷

	char R_status;	

	LC512_CS_L;
	delay_ms(1); 
	SPI_byte_write(inst_wren); //
	LC512_CS_H;
	delay_ms(1);

	LC512_CS_L;
	delay_ms(1);
	SPI_byte_write(inst_write); //	
	delay_ms(1); 	
	do
	{		
		R_status=SPI_byte_read(0);		
		delay_ms(1);		
	}while(R_status & 0x01);//´ýÐ¾Æ¬¿ÕÏ
	LC512_CS_H;
}



int LC512_Init(void)
{
	char inst_wren = WREN, inst_write = WRDI;//Ð´¸´Î»
	delay_ms(300);	
	LC512_CS_H;//Ò»¶¨ÒªÏÈÖÃ1ÉÏµç¼¤»î
	delay_ms(300);

	LC512_CS_L;
	delay_ms(1); 
	SPI_byte_write(inst_wren);	
	LC512_CS_H;
	delay_ms(1);

	delay_ms(1);

	LC512_CS_L;
	delay_ms(1);
	SPI_byte_write(inst_write);	
	delay_ms(10);
	LC512_CS_H;

	return 0;
}


