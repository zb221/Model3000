/**********************************************
@		Description: This file is about EEPROM 25LC512 defines.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/25.
***********************************************/

#ifndef _e25LC512_H
#define _e25LC512_H

#define  EE_Sector0_addr   0//用于产品配置信息存储
#define  EE_Sector1_addr   16384//用于原始数据存储
#define  EE_Sector2_addr   32768//用于原始数据存储
#define  EE_Sector3_addr   49152//用于报警记录存储
#define  EE_Max_addr       65535
//#define  addr							 0x0000

#define READ		0x03
#define WRITE		0x02
#define WREN		0x06
#define WRDI		0x04
#define RDSR		0x05
#define WRSR		0x01
#define PE			0x42
#define SE			0xd8
#define CE			0xc7
#define RDID		0xab
#define DPD			0xb9
#define LC512_CS_H			{	IO0DIR|=(1<<7);IO0SET|=(1<<7); }		//P0.7
#define LC512_CS_L			{	IO0DIR|=(1<<7);IO0CLR|=(1<<7); }		//0.7
#define LC512_SI_H			{	IO0DIR|=(1<<19);IO0SET|=(1<<19); }	//P0.19
#define LC512_SI_L			{	IO0DIR|=(1<<19);IO0CLR|=(1<<19); }	//    
#define LC512_SCK_H			{	IO0DIR|=(1<<17);IO0SET|=(1<<17); }	//P0.17
#define LC512_SCK_L			{	IO0DIR|=(1<<17);IO0CLR|=(1<<17); }	//
#define LC512_SO_IN			( IO0PIN&(1<<18) )										//P0.18

//VARIABLE_EXT void delay_ms(unsigned int n);
//VARIABLE_EXT void SPI_byte_write(const unsigned int dat);
//VARIABLE_EXT char  SPI_byte_read(unsigned char dat);
//VARIABLE_EXT void SPI_nbyte_write(const unsigned int address, const unsigned char *dat, unsigned int num);
//VARIABLE_EXT void SPI_nbyte_read(unsigned int address, unsigned char *dat, unsigned int num);
//VARIABLE_EXT void EE25LC_Erase_Sector(char sector_addr);
void Delay_us(unsigned short value);
void Delay_ms(unsigned short value);
void e2prom512_write_enable(void);
void e2prom512_write_disable(void);
void e2prom512_byte_write(unsigned char value,unsigned short address);
unsigned char e2prom512_byte_read(unsigned short start_address);
void e2prom512_chip_erase(void);
void e2prom512_page_erase(void);
void e2prom512_sector_erase(void);
void SPI_nbyte_write(const unsigned int address, const unsigned char *dat, unsigned int len);
unsigned int e2prom512_write (const void *buffer,unsigned int len,unsigned int address);
void e2prom512_read (unsigned char *buffer,unsigned int len,unsigned int address);   /* FLASH address to read from */
void RW_Status(void);
void Initial_e2prom(void);
void e2promtest(void);
#endif

