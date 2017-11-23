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

#endif
