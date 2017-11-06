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


void M25P16_CS_INIT(void);
void M25P16_TEST(void);
void M25P16_read_data_anywhere(unsigned short data_count,unsigned int start_address);
void M25P16_reset(void);
void M25P16_read_ID(unsigned char *pbuffer);

#endif
