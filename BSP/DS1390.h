
#include "parameter.h"

#ifdef VARIABLE_GLOBALS
#define VARIABLE_EXT
#else
#define VARIABLE_EXT extern
#endif

#ifndef __DS1390_H__
#define __DS1390_H__

#define REG_SEC		  0x81
#define REG_MIN		  0x82
#define REG_HOUR    0x83
#define REG_WEEK 	  0x84
#define REG_DAY		  0x85
#define REG_MONTH	  0x86
#define REG_YEAR    0x87
#define REG_CONTROL	0x8D
#define REG_STATUS	0x8E

//VARIABLE_EXT unsigned char hide_sec,hide_min,hide_hour,hide_day,hide_week,hide_month,hide_year;  //秒,分,时到日,月,年位闪的计数
//VARIABLE_EXT unsigned char UDT_Flag;
//VARIABLE_EXT unsigned char DST_Flag;
//VARIABLE_EXT unsigned char DST_EN;
//VARIABLE_EXT unsigned char D60_Flag;
//VARIABLE_EXT unsigned int time_s;
//VARIABLE_EXT unsigned char D60_sec;
//VARIABLE_EXT unsigned char D60_min;
//VARIABLE_EXT unsigned char D60_hour;
//VARIABLE_EXT unsigned char DST_sec;//静态时间 动态时间
//VARIABLE_EXT unsigned char DST_min;
//VARIABLE_EXT unsigned char DST_hour;
//VARIABLE_EXT unsigned char Timer_Ds1390_sec;//静态时间 动态时间
//VARIABLE_EXT unsigned char Timer_Ds1390_min;
//VARIABLE_EXT unsigned char Timer_Ds1390_hour;
//VARIABLE_EXT unsigned long long int Runtimes;	

#define DS1390_CS_H    {   IO0DIR|=(1<<13);IO0SET|=(1<<13); }//P0.13
#define DS1390_CS_L    { IO0DIR|=(1<<7);IO0SET|=(1<<7);IO0DIR|=(1<<25);IO0SET|=(1<<25);IO0DIR|=(1<<13);IO0CLR|=(1<<13); }//{LC512_CS_H;AT25_CS_H;   IO0DIR|=(1<<13);IO0CLR|=(1<<13); }
#define DS1390_DIN_H    {   IO0DIR|=(1<<19);IO0SET|=(1<<19); }//P0.19
#define DS1390_DIN_L    {   IO0DIR|=(1<<19);IO0CLR|=(1<<19); }//    
#define DS1390_SCK_H    {   IO0DIR|=(1<<17);IO0SET|=(1<<17); }//P0.17
#define DS1390_SCK_L    {   IO0DIR|=(1<<17);IO0CLR|=(1<<17); }//
#define DS1390_DOUT_IN   (   IO0PIN&(1<<18)  )//P0.18


VARIABLE_EXT void TASK_DS1390_Update(void);
VARIABLE_EXT void DS1390InputByte(unsigned char wr_data);
VARIABLE_EXT unsigned char DS1390OutputByte(void);
VARIABLE_EXT void Write1390(unsigned char ucAddr, unsigned char ucDa);
VARIABLE_EXT unsigned char Read1390(unsigned char ucAddr);
VARIABLE_EXT void DS1390_GetTime(REALTIMEINFO *Time);
VARIABLE_EXT void DS1390_Delay(unsigned int n);
VARIABLE_EXT void Initial_DS1390(void);
VARIABLE_EXT void DS1390_AdjustTime(REALTIMEINFO *Time);
//VARIABLE_EXT void TimeToStr(REALTIMEINFO *Time);
//VARIABLE_EXT void DateToStr(REALTIMEINFO *Time);
//VARIABLE_EXT void DateTimeToStr(REALTIMEINFO *Time);
//VARIABLE_EXT void Timer_DS1390_Init(void);
//VARIABLE_EXT void Timer_DS1390_Init(void);
//VARIABLE_EXT void Timer_DS1390(unsigned char hour,unsigned char min,unsigned char sec,unsigned char EN_T);
//VARIABLE_EXT void Timer_SetCount(unsigned int t_inter, unsigned char EN_T);
//VARIABLE_EXT void Timer_Cal(unsigned int t_inter, unsigned char EN_T);
//VARIABLE_EXT void Timer_Public(unsigned int t_inter, unsigned char EN_T);
//void Timer_Delay(unsigned int t_inter, unsigned char EN_T);
#endif
