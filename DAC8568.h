/***********************************************
@		Description: This file is about DAC8568.h data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/
#ifndef __DAC8568_h
#define __DAC8568_h

void DAC8568_CS_INIT(void);
void DAC8568_SET(unsigned char PB,unsigned char CB,unsigned char AB,unsigned short DB,unsigned char FB);
void DAC_SET_Chanel_Din(float temperature,int *DAC_DIN);
void DAC8568_INIT_SET(float temperature,float current);

#endif

