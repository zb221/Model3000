#include "DS1390.h"
#ifndef __MODBUS_H__
#define __MODBUS_H__

#define EN_485_DE					(1<<16)
#define EN_AT25xx					(1<<25)

//#define LC512_CS_H     {   IO0DIR|=(1<<7);IO0SET|=(1<<7); }//P0.7    //all defination below should be changed in future
//#define LC512_CS_L     {DS1390_CS_H;AT25_CS_H;      IO0DIR|=(1<<7);IO0CLR|=(1<<7); }//0.7
//#define LC512_SI_H    {   IO0DIR|=(1<<19);IO0SET|=(1<<19); }//P0.19
//#define LC512_SI_L    {   IO0DIR|=(1<<19);IO0CLR|=(1<<19); }//    
//#define LC512_SCK_H    {   IO0DIR|=(1<<17);IO0SET|=(1<<17); }//P0.17
//#define LC512_SCK_L    {   IO0DIR|=(1<<17);IO0CLR|=(1<<17); }//
//#define LC512_SO_IN    ( IO0PIN&(1<<18) )//P0.18
#define AT25_CS_H     {   IO0DIR|=(1<<25);IO0SET|=(1<<25); }//P0.25
#define AT25_CS_L     {DS1390_CS_H;LC512_CS_H;  IO0DIR|=(1<<25);IO0CLR|=(1<<25); }//



void Data_Analysis(unsigned char tata);
unsigned short crc16(unsigned char *puchMsg, unsigned short usDataLen);
void Data_Ack_Processor(void);//hugo add

void UpData_ModbBus(REALTIMEINFO *Time);
void Init_io(void); 
int Init_ModBus (void);
int RW_ModBus_Data (void);


#endif