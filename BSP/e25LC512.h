/**********************************************
@		Description: This file is about EEPROM 25LC512 defines.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/25.
***********************************************/

#ifdef VARIABLE_GLOBALS
#define VARIABLE_EXT
#else
#define VARIABLE_EXT extern
#endif

#ifndef _e25LC512_H
#define _e25LC512_H

#define  EE_Sector0_addr   0//用于产品配置信息存储
#define  EE_Sector1_addr   16384//用于原始数据存储
#define  EE_Sector2_addr   32768//用于原始数据存储
#define  EE_Sector3_addr   49152//用于报警记录存储
#define  EE_Max_addr       65535
#define  addr							 0x0000//?????

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

void delay_ms(unsigned int n);
void LC512_TEST(void);
#endif

