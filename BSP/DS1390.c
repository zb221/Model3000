/***********************************************
@		Description: This file is about RTC DS1390.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: megzheng.
@		Date: 2017/10/25.
***********************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions               */
#include <stdio.h>
#include "Peripherals_LPC2194.h"
#include "DS1390.h"
REALTIMEINFO CurrentTime;

void DS1390_Delay(unsigned int n)
{
	int i, j;              
	for(i=0;i<n;++i)
	for(j=0;j<100;++j);
}

void DS1390InputByte(unsigned char wr_data) 	
{ 
    unsigned char n=8;       
    while(n--)
	  {
       DS1390_SCK_L;                     
       if((wr_data&0x80) == 0x80)         
	      {
           DS1390_DIN_H;                 
	      }
	     else
        {
	       DS1390_DIN_L;                 
	      }
       	wr_data = wr_data<<1;                  
        DS1390_SCK_H;                    
        DS1390_Delay(300);								
	  }	
	
}

unsigned char DS1390OutputByte(void) 	
{  
int i;
unsigned char  tdata=0;
      for(i=0;i<8;i++)
      {
         DS1390_Delay(1);
         DS1390_SCK_L;
         tdata=tdata<<1;
         if(DS1390_DOUT_IN!=0)
            tdata=tdata|0x01;
         else
            tdata=tdata&0xFE;
         DS1390_SCK_H;
      }
return(tdata);
}

void Write1390(unsigned char ucAddr, unsigned char ucDa)	
{
	DS1390_CS_L;	
//	DS1390InputByte(ucAddr);       
//	DS1390InputByte(ucDa);
  DS1390_Delay(1);	
	SPI1_SendDate(ucAddr);
	SPI1_SendDate(ucDa);
	DS1390_CS_H;
} 

unsigned char Read1390(unsigned char ucAddr)	
{
	unsigned char ucData;
	DS1390_CS_L;		
//	DS1390InputByte(ucAddr & 0x0F);  
	DS1390_Delay(1);
	SPI1_SendDate(ucAddr & 0x0F);
	ucData=SPI1_SendDate(0x00);
	//ucData = DS1390OutputByte();          
	DS1390_CS_H;
	return(ucData);
}

void DS1390_GetTime(REALTIMEINFO *Time)  
{
	unsigned char Temp;
	unsigned char High_4, Low_4;
	Temp = Read1390(REG_SEC);		
	Low_4 = Temp & 0x0F;
	High_4  = (Temp & 0x70) >> 4;
	Time->SpecificTime.sec = High_4 * 10 + Low_4;
	Temp = Read1390(REG_MIN);		
	Low_4 = Temp & 0x0F;
	High_4  = (Temp & 0x70) >> 4;
	Time->SpecificTime.min = High_4 * 10 + Low_4;
	Temp = Read1390(REG_HOUR);	
	Low_4 = Temp & 0x0F;
	High_4  = (Temp & 0x70) >> 4;
	Time->SpecificTime.hour = High_4 * 10 + Low_4;
	Temp = Read1390(REG_DAY);		
	Low_4 = Temp & 0x0F;
	High_4  = (Temp & 0x70) >> 4;
	Time->SpecificTime.day = High_4 * 10 + Low_4;
	Temp = Read1390(REG_MONTH);  
	Low_4 = Temp & 0x0F;
	High_4  = (Temp & 0x70) >> 4;
	Time->SpecificTime.month = High_4 * 10 + Low_4;
	Temp = Read1390(REG_YEAR);	
	Low_4 = Temp & 0x0F;
	High_4  = (Temp & 0x70) >> 4;
	Time->SpecificTime.year = High_4 * 10 + Low_4;
}

void Initial_DS1390(void)   
{   
	unsigned char Second;
	DS1390_CS_H;	
	Second = Read1390(REG_STATUS);
	if((Second & 0x80) == 0x80)	      
	{
		Write1390(REG_CONTROL,0x18); 
		Write1390(REG_YEAR,0x17); 
		Write1390(REG_MONTH,0x04);
		Write1390(REG_DAY,0x27);
		Write1390(REG_WEEK,0x04);
		Write1390(REG_HOUR,0x19);
		Write1390(REG_MIN,0x43);
		Write1390(REG_SEC,0x30);
		Write1390(REG_STATUS,0x00); 
	}	
}

void DS1390_AdjustTime(REALTIMEINFO *Time)
{
	unsigned char Temp;
	unsigned char Tens, units;
	REALTIMEINFO TimeBCD;   
        
	Temp = Time->SpecificTime.sec;		
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.sec = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.min;		
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.min = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.hour;		
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.hour = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.week;		
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.week = ((Tens << 4) & 0xF0) | (units & 0x0F);	
	
	Temp = Time->SpecificTime.day;		
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.day = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.month;		
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.month = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Temp = Time->SpecificTime.year;		
	Tens = Temp / 10;
	units = Temp % 10;
	TimeBCD.SpecificTime.year = ((Tens << 4) & 0xF0) | (units & 0x0F);

	Write1390(REG_STATUS,0x80); 
	Write1390(REG_YEAR,TimeBCD.SpecificTime.year); 
	Write1390(REG_MONTH,TimeBCD.SpecificTime.month);
	Write1390(REG_DAY,TimeBCD.SpecificTime.day);
	Write1390(REG_WEEK,TimeBCD.SpecificTime.week);	
	Write1390(REG_HOUR,TimeBCD.SpecificTime.hour);
	Write1390(REG_MIN,TimeBCD.SpecificTime.min);
	Write1390(REG_SEC,TimeBCD.SpecificTime.sec);
	Write1390(REG_STATUS,0x00); 
}




