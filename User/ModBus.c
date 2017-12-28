/***********************************************
@		Description: This file is about ModBus data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/12/13.
***********************************************/

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

//宏定义Stop   hugo add 
unsigned char SpiFlash_Addr[5];
//变量定义Start  hugo add
unsigned short time0_count_1ms;
unsigned short time0_count_1ms_run;

/* CRC 高位字节值表 */
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
/* CRC低位字节值表*/
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

 __irq void IRQ_UART1(void)
{
	unsigned char recept_value_buffer;

	if((U1IIR&0x0e)==0x04)  //判断进入的是否为UART1接收中断
	{

		while (!(U1LSR & 0x01));//等待数据接收完毕

		recept_value_buffer=U1RBR;   	  
		//UARTprintf(" %x ",recept_value_buffer);
		if(user_parameter.flag.ubit.recept_ok==0)
		{
			Data_Analysis(recept_value_buffer);
		}
	}
	VICVectAddr = 0;                              /* Acknowledge Interrupt       */
}

void Init_io(void)//hugo add
{
	IO1DIR |= EN_485_DE;
	//IO1SET = EN_485_DE;
	AT25_CS_H; // 不使能AT25
}

void Spi_Flash_Data_read(void)
{
	unsigned short temp_read_start_address = 0, temp_read_end_address = 0, page = 0;
	unsigned short number = 0;
	
//	UARTprintf("%d %d %d\n",run_parameter.reserved_parameter13,run_parameter.reserved_parameter14,run_parameter.reserved_parameter15);
	if(run_parameter.reserved_parameter15 == 1001){
		temp_read_start_address = run_parameter.reserved_parameter13;
		temp_read_end_address = run_parameter.reserved_parameter14;
		
		if((temp_read_start_address>(8192-512))||(temp_read_end_address>(8192-512))||(temp_read_start_address>=temp_read_end_address))
		{
			run_parameter.reserved_parameter13=0;
			run_parameter.reserved_parameter14=0;
			run_parameter.reserved_parameter15=0;
			return;
		}
		
		for(page=temp_read_start_address;page<temp_read_end_address;page++)
		{
			M25P16_Read_Data(user_parameter.spi_flash_buffer,256,page*256);
			for(number=0;number<256;number++)
			{
//				UARTprintf("%d\n",user_parameter.spi_flash_buffer[number]);
				IO1SET = EN_485_DE;
				Delay_us(1);
				Uart1_SentByte(user_parameter.spi_flash_buffer[number]);
				IO1CLR = EN_485_DE;
			}
		}
		
		IO1SET = EN_485_DE;
		Delay_us(1);
		Uart1_SentByte(0xaa);
		Uart1_SentByte(0xbb);
		Uart1_SentByte(0xcc);
		Uart1_SentByte(0xdd);
		IO1CLR = EN_485_DE;	
		
		run_parameter.reserved_parameter13=0;
		run_parameter.reserved_parameter14=0;
		run_parameter.reserved_parameter15=0;
	}
}
	
void Init_interrupt(void)//hugo add 
{
		
	VICIntSelect = 0x00000000;												// ????????IRQ??
	VICVectCntl5 = 0x20 | 7;													// UART1???IRQ slot0,??????
	VICVectAddr5=(unsigned long)IRQ_UART1;//选择中断入口地址的程?
	VICIntEnable |= 1 << 0x07;							// ??UART1??
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
						 	case 0x03://读
						 						user_parameter.recept_function_code=data;
						 						user_parameter.recept_data_buffer[user_parameter.recept_point]=data;
						 						user_parameter.recept_point=12;
						 						break;
						 		
						 	case 0x06://写
						 						user_parameter.recept_function_code=data;
						 						user_parameter.recept_data_buffer[user_parameter.recept_point]=data;
						 						user_parameter.recept_point=22;
						 						break;
						 						
						 	default:
						 						//异常回复
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
												//异常回复
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
		case 12://读
						user_parameter.start_address.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-10]=data;
						user_parameter.recept_point=13;	
						break;
		case 13://读
						user_parameter.start_address.ubit.lo=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-10]=data;
						user_parameter.recept_point=14;	
						break;
		case 14://读
						user_parameter.register_count.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-10]=data;
						user_parameter.recept_point=15;	
						break;
		case 15://读
						user_parameter.register_count.ubit.lo=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-10]=data;
						user_parameter.recept_point=36;	
						break;											
		case 22://写
						user_parameter.start_address.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-20]=data;
						user_parameter.recept_point=23;	
						break;
		case 23://写
						user_parameter.start_address.ubit.lo=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-20]=data;
						user_parameter.recept_point=24;	
						break;
		case 24://写
						user_parameter.register_count.ubit.hi=data;
						user_parameter.recept_data_buffer[user_parameter.recept_point-20]=data;
						user_parameter.recept_point=25;	
						break;
		case 25://写
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
		          //UARTprintf("P %d\n",p);
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
  
unsigned char uchCRCHi = 0xFF ; /* 高CRC字节初始化 */
unsigned char uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */
unsigned int uIndex ; /* CRC循环中的索引 */
while (usDataLen--) /* 传输消息缓冲区 */
{
  
uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */
uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
uchCRCLo = auchCRCLo[uIndex] ; 
 
} 
return (uchCRCHi << 8 | uchCRCLo) ; 
 
}

///******************************************************************************/
///***************************      MAIN PROGRAM      ***************************/
///******************************************************************************/
int Init_ModBus (void)  
{

	Init_io();	
	Init_interrupt();

	run_parameter.reserved_parameter100=0x0240;//255	

/*---------------------------------------------------ModBus协议常量------------------------------------------------------------------*/
	user_parameter.flag.ubit.recept_write = 0;
	//31-40
	strcpy(run_parameter.model_number.model_number_str,"3000");
	//41-50
	strcpy(run_parameter.product_serial_number.product_serial_number_str,"180000001");
	//51-60
	strcpy(run_parameter.sensor_serial_number.sensor_serial_number_str,"S2.3.01893");
	//61-70
	strcpy(run_parameter.sensor_board_serial_number.sensor_board_serial_number_str,"12345678");//productInf.SensorBoardNum
	//71-80
	strcpy(run_parameter.interface_board_serial_number.interface_board_serial_number_str,"12345678");//productInf.InterfaceBoardNum
	//81-82
	run_parameter.manufacturing_date.month = 01;
	run_parameter.manufacturing_date.day = 01;
	run_parameter.manufacturing_date.year = 2018;

	//83、84
	//date=atof(month);
	run_parameter.factory_calibration_date.month = 01;
	//date=atof(day);
	run_parameter.factory_calibration_date.day = 01;
	//date=atof(year);
	run_parameter.factory_calibration_date.year = 2018;
	//85、86
	run_parameter.field_calibration_date.month = 01;
	run_parameter.field_calibration_date.day = 01;
	run_parameter.field_calibration_date.year = 2018;
	//87、88
	run_parameter.dissolved_gas_calibration_date.month = 01;
	run_parameter.dissolved_gas_calibration_date.day = 01;
	run_parameter.dissolved_gas_calibration_date.year = 2018;
	//89-98
	strcpy(run_parameter.firmware_revesion.firmware_revesion_str,"V2.0");
	strcpy(run_parameter.hardware_version.hardware_version_str,"V3.0");
	strcpy(run_parameter.factory.factory_str,"V1.0");
	//99-110保留

	//111
	//run_parameter.status_flag.all=0x00;//16位
	run_parameter.status_flag.ubit.senser_state0=0;
	run_parameter.status_flag.ubit.senser_state1=0;
	run_parameter.status_flag.ubit.senser_state2=0;	//000- 未准备好 001-正常运行 010-测量油温 011-基准循环 100-过温		
	run_parameter.status_flag.ubit.reserved7=0;				 
	run_parameter.status_flag.ubit.relay1=0;//继电器1			  
	run_parameter.status_flag.ubit.relay2=0;//继电器2				 
	run_parameter.status_flag.ubit.relay3=0;//继电器3				  		
	run_parameter.status_flag.ubit.reserved6=0;				 
	run_parameter.status_flag.ubit.reserved5=0;				  
	run_parameter.status_flag.ubit.reserved4=0;				  
	run_parameter.status_flag.ubit.reserved3=0;				  
	run_parameter.status_flag.ubit.reserved2=0;				  
	run_parameter.status_flag.ubit.error=0;						  
	run_parameter.status_flag.ubit.reserved1=0;				  
	run_parameter.status_flag.ubit.new_data_available=0; 
	run_parameter.status_flag.ubit.unit_ready=1;
	//112-120保留
	run_parameter.h2_ppm_report_high_h16.hilo = 0;
	run_parameter.h2_ppm_report_high_l16.hilo = 5000;
	e2prom512_write(&run_parameter.h2_ppm_report_high_h16.ubit.lo,4,143*2);
		
	run_parameter.h2_ppm_report_low_h16.hilo = 0;
	run_parameter.h2_ppm_report_low_l16.hilo = 0;
	e2prom512_write(&run_parameter.h2_ppm_report_low_h16.ubit.lo,4,141*2);

	//150
	run_parameter.unit_id.ubit.lo = 1;
	e2prom512_write(&run_parameter.unit_id.ubit.lo,2,150*2);
	//201-210
	strcpy(run_parameter.own_id.own_id_str,"user 01");
	e2prom512_write(&run_parameter.own_id.own_id_str,sizeof(run_parameter.own_id.own_id_str),201*2);
	//211-220
	strcpy(run_parameter.sub_station_id.sub_station_id_str,"Sub-Station 01");
	e2prom512_write(&run_parameter.sub_station_id.sub_station_id_str,sizeof(run_parameter.own_id.own_id_str),211*2);
	//221-230
	strcpy(run_parameter.transformer_id.transformer_id_str,"Transformer 01");
	e2prom512_write(&run_parameter.transformer_id.transformer_id_str,sizeof(run_parameter.own_id.own_id_str),221*2);


	run_parameter.h2_ppm_alarm_low_h16.hilo = 0;
	run_parameter.h2_ppm_alarm_low_l16.hilo = 2500;
	
	run_parameter.h2_ppm_alert_low_h16.hilo = 0;
	run_parameter.h2_ppm_alert_low_l16.hilo = 2500;
	
	run_parameter.OilTemp_Alarm_celsius.hilo = 70;
	
	e2prom512_write(&run_parameter.h2_ppm_alert_low_h16.ubit.lo,4,152*2);
	e2prom512_write(&run_parameter.h2_ppm_alarm_low_h16.ubit.lo,4,154*2);
	e2prom512_write(&run_parameter.OilTemp_Alarm_celsius.ubit.lo,2,156*2);
	
	run_parameter.h2_ppm_out_current_low.hilo = 400;
	run_parameter.h2_ppm_out_current_high.hilo = 2000;
	run_parameter.h2_ppm_error_out_current.hilo = 300;
	run_parameter.h2_ppm_no_ready_out_current.hilo = 200;
	
	e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);

	//231-255保留
	DS1390_CS_H;
	LC512_CS_H;
	
	return 0;
}


/******************************************************************************/
//if(user_parameter.flag.ubit.recept_write==1)
//{RW_ModBus_Data();}
/******************************************************************************/
int RW_ModBus_Data (void)  
{
	unsigned int db_H2ppm;	
	/*--------------------------------ModBus协议常量write-------------------------------------------------------*/
	//UARTprintf("point=%d\r\n",user_parameter.function_point);
	//99-110保留
	switch (user_parameter.function_point)
	{	
		case 101: //读Flash
		{
//			UARTprintf("read FLASH DATA\n");
			Spi_Flash_Data_read();
			break;
		}
		
		case 102: //Write Flash
		{
      UARTprintf("Write FLASH DATA\n");
			M25P16_Write_Sensor_Data();
			break;
		}
		
		case 121:
		{		
			//    UARTprintf("Current H2 value is %5u ppm H2\n",(unsigned int)((SenseData.h2*10000)/20));
			//    cmd_ConfigData.h2cdata_ka=SenseData.h2;		
			//    float_char(cmd_ConfigData.h2cdata_ka,systemInf.Cal_Ka);
			break;
		}
		
		case 122:
		{	
			//首先写126 127   128 129 在写122 写0确认
			db_H2ppm = (run_parameter.h2_ppm_calibration_gas_h16.hilo<<16 |run_parameter.h2_ppm_calibration_gas_l16.hilo);

			//		UARTprintf("Set hydrogen to %d ppm\r\n",db_H2ppm);//126 127	
			//    cmd_ConfigData.h2cdata_kb = (((float)(db_H2ppm*20)/10000.F) - cmd_ConfigData.h2cdata_ka);
			//    float_char(cmd_ConfigData.h2cdata_kb,systemInf.Cal_Kb);
			////自动写入校准时间
			//    cmd_ConfigData.Caldate=(CurrentTime.SpecificTime.year<<16 | CurrentTime.SpecificTime.month<<8 | CurrentTime.SpecificTime.day);		 		
			//    UARTprintf("Calibration Gas finished\n");
			break;		
		}
		
		case 123:
		{	
			//首先写126 127   128 129 在写122 写0确认
			db_H2ppm = (run_parameter.h2_ppm_calibration_gas_h16.hilo<<16 |run_parameter.h2_ppm_calibration_gas_l16.hilo);

			//		UARTprintf("Set hydrogen to %d ppm\r\n",db_H2ppm);//126 127	
			//    cmd_ConfigData.h2cdata_kb = (((float)(db_H2ppm*20)/10000.F) - cmd_ConfigData.h2cdata_ka);
			//    float_char(cmd_ConfigData.h2cdata_kb,systemInf.Cal_Kb);
			////自动写入校准时间
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
			//run_parameter.uint_id.ubit.lo=0x01;//默认ID
			e2prom512_write(&run_parameter.unit_id.ubit.lo,2,150*2);
			break;
		}

		case 133:
		{	
			//130 131 132 133

			RealTime_Modbus.Real_Time[1]=run_parameter.reserved_parameter341>>8;//月
			RealTime_Modbus.Real_Time[2]=run_parameter.reserved_parameter341&0xFF;//日	
			RealTime_Modbus.Real_Time[0]=run_parameter.reserved_parameter35&0xFF;//年

			RealTime_Modbus.Real_Time[4]=run_parameter.reserved_parameter36&0xFF;//时	
			RealTime_Modbus.Real_Time[5]=run_parameter.reserved_parameter37>>8;//分
			RealTime_Modbus.Real_Time[6]=run_parameter.reserved_parameter37&0xFF;//秒
			UARTprintf("Change time to 20%x-%x-%x %x:%x:%x",\
			(long int)RealTime_Modbus.Real_Time[0],\
			(long int)RealTime_Modbus.Real_Time[1],\
			(long int)RealTime_Modbus.Real_Time[2],(long int)RealTime_Modbus.Real_Time[4],\
			(long int)RealTime_Modbus.Real_Time[5],(long int)RealTime_Modbus.Real_Time[6]);		
			//*/
			Write1390(REG_STATUS,0x80); //写入允许
			Write1390(REG_YEAR,RealTime_Modbus.Real_Time[0]); 
			Write1390(REG_MONTH,RealTime_Modbus.Real_Time[1]);
			Write1390(REG_DAY,RealTime_Modbus.Real_Time[2]);
			////	Write1390(REG_WEEK,TimeBCD.SpecificTime.week);	
			Write1390(REG_HOUR,RealTime_Modbus.Real_Time[4]);
			Write1390(REG_MIN,RealTime_Modbus.Real_Time[5]);
			Write1390(REG_SEC,RealTime_Modbus.Real_Time[6]);
			Write1390(REG_STATUS,0x00); //禁止写入		

			//    DS1390_AdjustTime(&RealTime_Modbus);
			break;
		}
		
		case 142:
		{
			//141-142	
			cmd_ConfigData.H2low = (double)(((run_parameter.h2_ppm_report_low_h16.hilo<<16 |run_parameter.h2_ppm_report_low_l16.hilo)/10000.F)*20.F);
			e2prom512_write(&run_parameter.h2_ppm_report_low_h16.ubit.lo,4,141*2);	
			//float_char(cmd_ConfigData.H2low,systemInf.LowH2);		
			break;
		}	
		
		case 144:
		{
			//143-144	
			cmd_ConfigData.H2high = (double)(((run_parameter.h2_ppm_report_high_h16.hilo<<16 |run_parameter.h2_ppm_report_high_l16.hilo)/10000.F)*20.F);
			e2prom512_write(&run_parameter.h2_ppm_report_high_h16.ubit.lo,4,143*2);	
			//float_char(cmd_ConfigData.H2high,systemInf.HighH2);		
			break;
		}
		
		case 148:
		{
			//145 146 147 148
			cmd_ConfigData.LowmA = (double)((run_parameter.h2_ppm_out_current_low.hilo)/100.F);
			cmd_ConfigData.HighmA = (double)((run_parameter.h2_ppm_out_current_high.hilo)/100.F);
			cmd_ConfigData.ErrmA = (double)((run_parameter.h2_ppm_error_out_current.hilo)/100.F);
			cmd_ConfigData.NotRmA = (double)((run_parameter.h2_ppm_no_ready_out_current.hilo)/100.F);

			e2prom512_write(&run_parameter.h2_ppm_out_current_low.ubit.lo,8,145*2);

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
			cmd_ConfigData.MaxAlertH2 = (double)((run_parameter.h2_ppm_alert_low_h16.hilo<<16 |run_parameter.h2_ppm_alert_low_l16.hilo)/10000.F);
			e2prom512_write(&run_parameter.h2_ppm_alert_low_h16.ubit.lo,4,152*2);

			//float_char(cmd_ConfigData.MaxAlertH2,systemInf.AlertH2);		
			break;
		}

		//154 155
		case 155:
		{	
			cmd_ConfigData.MaxAlarmH2 = (double)((run_parameter.h2_ppm_alarm_low_h16.hilo<<16 |run_parameter.h2_ppm_alarm_low_l16.hilo)/10000.F);
			e2prom512_write(&run_parameter.h2_ppm_alarm_low_h16.ubit.lo,4,154*2);
			//float_char(cmd_ConfigData.MaxAlarmH2,systemInf.AlarmH2);		
			break;
		}

		//156
		case 156:
		{	
			cmd_ConfigData.MaxAlarmOil = (double)(run_parameter.OilTemp_Alarm_celsius.hilo/100.F);
			e2prom512_write(&run_parameter.OilTemp_Alarm_celsius.ubit.lo,2,156*2);
			UARTprintf("OilTemp_Alarm_celsius = %d\n",run_parameter.OilTemp_Alarm_celsius.hilo);
			//float_char(cmd_ConfigData.MaxAlarmOil,systemInf.AlarmOil);		
			break;
		}

		case 210:
		{
			//				 wr=Msg8StartAdr;//????0x0100?  ???257????						 
			// 				 strcpy(productInf.Own_id, run_parameter.own_id.own_id_str);//??        write_opera);//    cmd_analyze.processed_buf
			//	       flash_write (&productInf.Own_id[0],EElength, wr);//wr????EE??
			e2prom512_write(&run_parameter.own_id.own_id_sstr.character1,20,201*2);
			break;
		}
		
		case 220:
		{
			//				 wr=Msg9StartAdr;//????0x0100?  ???257????						 
			// 				 strcpy(productInf.Sub_station_id, run_parameter.sub_station_id.sub_station_id_str);//??        write_opera);//    cmd_analyze.processed_buf
			//	       flash_write (&productInf.Sub_station_id[0],EElength, wr);//wr????EE??
			e2prom512_write(&run_parameter.sub_station_id.sub_station_id_sstr.character1,20,211*2);
			break;
		}
		
		case 230:
		{
			//				 wr=Msg10StartAdr;//????0x0100?  ???257????						 
			// 				 strcpy(productInf.Transformer_id,run_parameter.transformer_id.transformer_id_str);//??        write_opera);//    cmd_analyze.processed_buf
			//	       flash_write (&productInf.Transformer_id[0],EElength, wr);//wr????EE??
			e2prom512_write(&run_parameter.transformer_id.transformer_id_sstr.character1,20,221*2);
			break;
		}
		//231-255保留
	}

	user_parameter.flag.ubit.recept_write=0;			
	//return 0;
	//wFLASHconfig(sys_addr,(unsigned char *)&systemInf);
	return 0;
}
