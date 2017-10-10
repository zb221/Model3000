/***********************************************
@		Description: This file is header data of AD7738.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/
#ifndef __AD7738_h
#define __AD7738_h


#define TURE 1


void AD7738_CS_INIT(void);
void M25P16_CS_INIT(void);
void AD7738_write(unsigned char Register,unsigned char data);
void AD7738_read(unsigned char Register,unsigned char *data);
void AD7738_read_channel_data(unsigned char Register,unsigned char *buf0,unsigned char *buf1,unsigned char *buf2);
void AD7738_SET(void);
void ADC7738_acquisition(unsigned char channel);
void ADC7738_acquisition_output(unsigned char channel);

#endif

