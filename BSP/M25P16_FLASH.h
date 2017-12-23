/**********************************************
@		Description: This file is about M25P16 FLASH defines.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/17.
***********************************************/

#ifndef __M25P16_FLASH_h
#define __M25P16_FLASH_h

#define SE 0xD8
#define BE 0xC7

void Init_M25P16(void);
void M25P16_TEST(void);
void M25P16_Data_Records(void);
void M25P16_Read_Data(unsigned char *Rec_buffer,unsigned int data_nub,unsigned int start_address);
void M25P16_Write_Data(unsigned char *write_buffer,unsigned int data_nub,unsigned int start_address);
void M25P16_erase_map(unsigned int block_address, unsigned char erase_type);
void M25P16_Alarm_Log_Records(void);
#endif
