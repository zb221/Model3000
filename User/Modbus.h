
#ifndef __MODBUS_H__
#define __MODBUS_H__

#define EN_485_DE					(1<<16)
#define EN_AT25xx					(1<<25)
void Data_Analysis(unsigned char tata);
unsigned short crc16(unsigned char *puchMsg, unsigned short usDataLen);
void Data_Ack_Processor(void);//hugo add

void UpData_ModbBus(REALTIMEINFO *Time);
void Init_io(void); 
int Init_ModBus (void);
int RW_ModBus_Data (void);


#endif