/***********************************************
@		Description: This file is header data of Cubic.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/
#ifndef __Cubic_h
#define __Cubic_h


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE -1
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef ERR
#define ERR  -2
#endif

#define Temp_Res 0
#define Hydrogen_Res 1
#define DAC_temp 2

float Cubic_main(float value,unsigned char type);
void Linear_slope(float *slope, float *x, float *y, unsigned char type);

#endif
