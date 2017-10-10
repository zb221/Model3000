/******************************************************************************/
/* MEASURE.H: struct type and extern definitions for MEASURE project          */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#ifdef VARIABLE_GLOBALS
#define VARIABLE_EXT
#else
#define VARIABLE_EXT extern
#endif

#ifndef __MODBUS_H__
#define __MODBUS_H__

//宏定义Start  hugo add
//#define LED_RED						(1<<18)
//#define LED_GREEN					(1<<17)
#define EN_485_DE					(1<<16)
#define EN_AT25xx					(1<<25)

#define AT25_CS_H     {   IO0DIR|=(1<<25);IO0SET|=(1<<25); }//P0.25
#define AT25_CS_L     {DS1390_CS_H;LC512_CS_H;  IO0DIR|=(1<<25);IO0CLR|=(1<<25); }//

typedef struct
{
	union
	{
		struct
		{
			unsigned short recept_ok				 :1;				//1  一帧数据接受成功
			unsigned short recept_data_error :1;				//1  一帧数据有数据异常
			unsigned short recept_crc_error  :1;				//1  一帧数据crc检验异�
			unsigned short recept_write			 :1;				//1  一帧数据接受成� only	 write			
			}ubit;
		unsigned short all;	
		
		}flag;
	union
	{
		struct
		{			
			unsigned char lo;
			unsigned char hi;		
		
			}ubit;
		unsigned short hi_lo;	
		
		}start_address;
	union
	{
		struct
		{
			unsigned char lo;				
			unsigned char hi;	
		
			}ubit;
		unsigned short hi_lo;	
		
		}register_count;
	union
	{
		struct
		{							
			unsigned char hi;
			unsigned char lo;
		
			}ubit;
		unsigned short hi_lo;	
		
		}recept_crc;
			
	unsigned char recept_point;//通讯用接受数据游标
	unsigned char recept_data_buffer[8];//通讯用接受数据
	unsigned char recept_function_code;//03 读指定寄存器；06写寄存器
	unsigned short calculate_crc;//接受、发送数据计算CEC值
	unsigned char send_buffer[260];
	unsigned char spi_flash_buffer[256];//flash 数据缓�
	unsigned short function_point;			    //1  一帧数据接受成� only	 write
	unsigned char Alarm_data_buffer[8];
}USER_PARAMETER;
VARIABLE_EXT  USER_PARAMETER  user_parameter;	

typedef struct
{
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_h16;//0 Register
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_l16;//1	
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_dga_h16;//2
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_dga_l16;//3
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_max_h16;//4
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_max_l16;//5	
	union
	{
		struct
		{
			unsigned char lo;//2017.06.16高8低8调换
			unsigned char hi;		
			}ubit;	
		unsigned short hilo;
		}die_temperature_celsius;//6
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;		
			}ubit;	
		unsigned short hilo;
		}pcb_temperature_celsius;//7
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;		
			}ubit;	
		unsigned short hilo;
		}oil_temperature_celsius;//8
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_gas_equivalent_h16;//9
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_gas_equivalent_l16;//10
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_24hour_average_h16;//11
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_24hour_average_l16;//12
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_DRC_h16;//13
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_DRC_l16;//14
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_WRC_h16;//15
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_WRC_l16;//16	
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_MRC_h16;//17
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_MRC_l16;//18
	unsigned short reserved_parameter1;//19
	unsigned short reserved_parameter2;//20
	unsigned short reserved_parameter3;//21
	unsigned short reserved_parameter4;//22	
	unsigned short reserved_parameter5;//23
	unsigned short reserved_parameter6;//24	
	unsigned short reserved_parameter7;//25	
	unsigned short reserved_parameter8;//26
	unsigned short reserved_parameter9;//27
	unsigned short reserved_parameter10;//28
	unsigned short reserved_parameter11;//29
	unsigned short reserved_parameter12;//30
  union
	{
		struct
	  {
		unsigned char character1;
		unsigned char character2;//31
		unsigned char character3;
		unsigned char character4;//32
		unsigned char character5;
		unsigned char character6;//33
		unsigned char character7;
		unsigned char character8;//34
		unsigned char character9;
		unsigned char character10;//35
		unsigned char character11;
		unsigned char character12;//36
		unsigned char character13;
		unsigned char character14;//37
		unsigned char character15;
		unsigned char character16;//38
		unsigned char character17;
		unsigned char character18;//39
		unsigned char character19;
		unsigned char character20;//40
		}modle_number_sstr;
		char modle_number_str[20];		
	}modle_number;
union
{	
	struct
	  {
		unsigned char character1;
		unsigned char character2;//41
		unsigned char character3;
		unsigned char character4;//42
		unsigned char character5;
		unsigned char character6;//43
		unsigned char character7;
		unsigned char character8;//44
		unsigned char character9;
		unsigned char character10;//45
		unsigned char character11;
		unsigned char character12;//46
		unsigned char character13;
		unsigned char character14;//47
		unsigned char character15;
		unsigned char character16;//48
		unsigned char character17;
		unsigned char character18;//49
		unsigned char character19;
		unsigned char character20;//50
		}product_serial_number_sstr;
		char product_serial_number_str[20];
}product_serial_number;
union
{		
	struct
	  {
		unsigned char character1;
		unsigned char character2;//51
		unsigned char character3;
		unsigned char character4;//52
		unsigned char character5;
		unsigned char character6;//53
		unsigned char character7;
		unsigned char character8;//54
		unsigned char character9;
		unsigned char character10;//55
		unsigned char character11;
		unsigned char character12;//56
		unsigned char character13;
		unsigned char character14;//57
		unsigned char character15;
		unsigned char character16;//58
		unsigned char character17;
		unsigned char character18;//59
		unsigned char character19;
		unsigned char character20;//60
		}sensor_serial_number_sstr;
		char sensor_serial_number_str[20];
	}sensor_serial_number;
union
{	
	struct
	  {
		unsigned char character1;
		unsigned char character2;//61
		unsigned char character3;
		unsigned char character4;//62
		unsigned char character5;
		unsigned char character6;//63
		unsigned char character7;
		unsigned char character8;//64
		unsigned char character9;
		unsigned char character10;//65
		unsigned char character11;
		unsigned char character12;//66
		unsigned char character13;
		unsigned char character14;//67
		unsigned char character15;
		unsigned char character16;//68
		unsigned char character17;
		unsigned char character18;//69
		unsigned char character19;
		unsigned char character20;//70
		}sensor_board_serial_number_sstr;
		char sensor_board_serial_number_str[20];
}sensor_board_serial_number;
union
{
	struct
	  {
		unsigned char character1;
		unsigned char character2;//71
		unsigned char character3;
		unsigned char character4;//72
		unsigned char character5;
		unsigned char character6;//73
		unsigned char character7;
		unsigned char character8;//74
		unsigned char character9;
		unsigned char character10;//75
		unsigned char character11;
		unsigned char character12;//76
		unsigned char character13;
		unsigned char character14;//77
		unsigned char character15;
		unsigned char character16;//78
		unsigned char character17;
		unsigned char character18;//79
		unsigned char character19;
		unsigned char character20;//80
		}interface_board_serial_number_sstr;
		char interface_board_serial_number_str[20];
}interface_board_serial_number;
	struct
	{
		unsigned char day;//81		
		unsigned char month;
		unsigned short year;//82
		}manufacturing_date;
//union
//{	
	struct
	{
		unsigned char day;//83		
		unsigned char month;
		unsigned short year;//84
//		}factory_calibration_date_sstr;
//	  unsigned char factory_calibration_date_str[4];
}factory_calibration_date;	
	struct
	{
		unsigned char day;//85		
		unsigned char month;
		unsigned short year;//86
  }field_calibration_date;	
		
	struct
	{
		unsigned char day;//87		
		unsigned char month;
		unsigned short year;//88
		}dissolved_gas_calibration_date;
union
{		
	struct
	  {
		unsigned char character1;
		unsigned char character2;//89
		unsigned char character3;
		unsigned char character4;//90
		unsigned char character5;
		unsigned char character6;//91
		unsigned char character7;
		unsigned char character8;//92
		unsigned char character9;
		unsigned char character10;//93
		unsigned char character11;
		unsigned char character12;//94
		unsigned char character13;
		unsigned char character14;//95
		unsigned char character15;
		unsigned char character16;//96
		unsigned char character17;
		unsigned char character18;//97
		unsigned char character19;
		unsigned char character20;//98
		}firmware_revesion_sstr;
		char firmware_revesion_str[20];
}firmware_revesion;
	unsigned short reserved_parameter13;//99
	unsigned short reserved_parameter14;//100
	unsigned short reserved_parameter15;//101
	unsigned short reserved_parameter16;//102
	unsigned short reserved_parameter17;//103
	unsigned short reserved_parameter18;//104
	unsigned short reserved_parameter19;//105
	unsigned short reserved_parameter20;//106
	unsigned short reserved_parameter21;//107
	unsigned short reserved_parameter22;//108
	unsigned short reserved_parameter23;//109
	unsigned short reserved_parameter24;//110
	union
	{
		struct
		{
			unsigned short senser_state0			:1;			
			unsigned short senser_state1			:1;
			unsigned short senser_state2			:1;
			unsigned short reserved7				  :1;
			unsigned short relay1				  		:1;		//1 触发
			unsigned short relay2				  		:1;		//1 触发	
			unsigned short relay3				  		:1;		//1 触发	
			unsigned short reserved6				  :1;
			unsigned short reserved5				  :1;
			unsigned short reserved4				  :1;
			unsigned short reserved3				  :1;
			unsigned short reserved2				  :1;
			unsigned short error						  :1;
			unsigned short reserved1				  :1;
			unsigned short new_data_available :1;
			unsigned short unit_ready				  :1;																		
			}ubit;
		unsigned short all;	
		
		}status_flag;//111
	unsigned short reserved_parameter25;//112
	unsigned short reserved_parameter26;//113
	unsigned short reserved_parameter27;//114
	unsigned short reserved_parameter28;//115
	unsigned short reserved_parameter29;//116
	unsigned short reserved_parameter30;//117
	unsigned short reserved_parameter31;//118
	unsigned short reserved_parameter32;//119
	unsigned short reserved_parameter33;//120
	unsigned short da_command;//121	
	unsigned short db_command;//122
	unsigned short dx_command;//123
	unsigned short reserved_parameter34;//124
	union
	{
		struct
		{
						
			unsigned short clear 						:1;	//0  field      1 DGA	
			unsigned short reserved				  :15;				
			}ubit;
			unsigned short all;
		}x_command;	//125
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_calibration_gas_h16;//126
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_calibration_gas_l16;//127 
	struct
	{
		unsigned char day;		
		unsigned char month;
		unsigned short year;
		}calibration_date;//128 129
	
////	struct
////	{
////		unsigned char day;
////		unsigned char month;//130
////		unsigned short year;//131
////		
////		unsigned char hour;
////		unsigned char week;//132
////		unsigned char seconds;
////		unsigned char minutes;//133
////		}RTC;		
		
	unsigned short reserved_parameter341;//130
	unsigned short reserved_parameter35;//131
	unsigned short reserved_parameter36;//132
	unsigned short reserved_parameter37;//133
	unsigned short reserved_parameter38;//134
	unsigned short reserved_parameter39;//135
	unsigned short reserved_parameter40;//136
	unsigned short reserved_parameter41;//137
	unsigned short reserved_parameter42;//138
	unsigned short reserved_parameter43;//139
	unsigned short reserved_parameter44;//140
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_report_low_h16;//141
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_report_low_l16;//142
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_report_high_h16;//143
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_report_high_l16;//144	
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;		
			}ubit;	
		unsigned short hilo;
		}h2_ppm_out_current_low;//145
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;		
			}ubit;	
		unsigned short hilo;
		}h2_ppm_out_current_high;//146
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;		
			}ubit;	
		unsigned short hilo;
		}h2_ppm_error_out_current;//147	
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;		
			}ubit;	
		unsigned short hilo;
		}h2_ppm_no_ready_out_current;//148
	unsigned short sw_to_command;//149
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;			
			}ubit;
		unsigned short hi_lo;	
		}	uint_id;//150
	unsigned short operating_mode;//151	 参照协议
		
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_alert_low_h16;//152
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_alert_low_l16;//153		

	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_alarm_low_h16;//154
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_alarm_low_l16;//155

	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;		
			}ubit;	
		unsigned short hilo;
	}OilTemp_Alarm_celsius;//156		
		
//	unsigned short reserved_parameter45;//152
//	unsigned short reserved_parameter46;//153
		
//	unsigned short reserved_parameter47;//154
//	unsigned short reserved_parameter48;//155
		
//	unsigned short reserved_parameter49;//156
		
	unsigned short reserved_parameter50;//157
	unsigned short reserved_parameter51;//158
	unsigned short reserved_parameter52;//159
	unsigned short reserved_parameter53;//160
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}die_temperature_counts_h16;//161
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}die_temperature_counts_l16;//162
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}pcb_temperature_counts_h16;//163
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}pcb_temperature_counts_l16;//164
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}heater_temperature_counts_h16;//165
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}heater_temperature_counts_l16;//166
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_resistor_counts_h16;//167
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_resistor_counts_l16;//168
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}adjusted_resistor_counts_h16;//169
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}adjusted_resistor_counts_l16;//170
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_resistor_h16;//171
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_resistor_l16;//172
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_legacy_h16;//173
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}h2_ppm_legacy_l16;//174
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}run_time_in_secends_hh32;//175
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}run_time_in_secends_h32;//176
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}run_time_in_secends_ll32;//177
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}run_time_in_secends_l32;//178		
	unsigned short reserved_parameter54;//179
	unsigned short reserved_parameter55;//180
	unsigned short reserved_parameter56;//181
	unsigned short reserved_parameter57;//182
	unsigned short reserved_parameter58;//183	
	unsigned short reserved_parameter59;//184
	unsigned short reserved_parameter60;//185
	unsigned short reserved_parameter61;//186
	unsigned short reserved_parameter62;//187
	unsigned short reserved_parameter63;//188
	unsigned short reserved_parameter64;//189	
	unsigned short reserved_parameter65;//190
	unsigned short reserved_parameter66;//191
	unsigned short reserved_parameter67;//192
	unsigned short reserved_parameter68;//193
	unsigned short reserved_parameter69;//194
	unsigned short reserved_parameter70;//195
	unsigned short reserved_parameter71;//196
	unsigned short reserved_parameter72;//197
	unsigned short reserved_parameter73;//198
	unsigned short reserved_parameter74;//199
	unsigned short reserved_parameter75;//200
union
{
	struct
	  {
		unsigned char character1;
		unsigned char character2;//201
		unsigned char character3;
		unsigned char character4;//202
		unsigned char character5;
		unsigned char character6;//203
		unsigned char character7;
		unsigned char character8;//204
		unsigned char character9;
		unsigned char character10;//205
		unsigned char character11;
		unsigned char character12;//206
		unsigned char character13;
		unsigned char character14;//207
		unsigned char character15;
		unsigned char character16;//208
		unsigned char character17;
		unsigned char character18;//209
		unsigned char character19;
		unsigned char character20;//210
		}own_id_sstr;
		char own_id_str[20];
}own_id;
union
{
	struct
	  {
		unsigned char character1;
		unsigned char character2;//211
		unsigned char character3;
		unsigned char character4;//212
		unsigned char character5;
		unsigned char character6;//213
		unsigned char character7;
		unsigned char character8;//214
		unsigned char character9;
		unsigned char character10;//215
		unsigned char character11;
		unsigned char character12;//216
		unsigned char character13;
		unsigned char character14;//217
		unsigned char character15;
		unsigned char character16;//218
		unsigned char character17;
		unsigned char character18;//219
		unsigned char character19;
		unsigned char character20;//220
		}sub_station_id_sstr;
		char sub_station_id_str[20];
}sub_station_id;
union
{
	struct
	  {
		unsigned char character1;
		unsigned char character2;//221
		unsigned char character3;
		unsigned char character4;//222
		unsigned char character5;
		unsigned char character6;//223
		unsigned char character7;
		unsigned char character8;//224
		unsigned char character9;
		unsigned char character10;//225
		unsigned char character11;
		unsigned char character12;//226
		unsigned char character13;
		unsigned char character14;//227
		unsigned char character15;
		unsigned char character16;//228
		unsigned char character17;
		unsigned char character18;//229
		unsigned char character19;
		}transformer_id_sstr;
		char transformer_id_str[20];
}transformer_id;		
		unsigned short reserved_parameter76;//231
		unsigned short reserved_parameter77;//232
		unsigned short reserved_parameter78;//233
		unsigned short reserved_parameter79;//234
		unsigned short reserved_parameter80;//235
		unsigned short reserved_parameter81;//236
		unsigned short reserved_parameter82;//237
		unsigned short reserved_parameter83;//238
		unsigned short reserved_parameter84;//239
		unsigned short reserved_parameter85;//240
		unsigned short reserved_parameter86;//241
		unsigned short reserved_parameter87;//242
		unsigned short reserved_parameter88;//243
		unsigned short reserved_parameter89;//244
		unsigned short reserved_parameter90;//245
		unsigned short reserved_parameter91;//246
		unsigned short reserved_parameter92;//247
		unsigned short reserved_parameter93;//248
		unsigned short reserved_parameter94;//249
		unsigned short reserved_parameter95;//250
		unsigned short reserved_parameter96;//251
		unsigned short reserved_parameter97;//252
		unsigned short reserved_parameter98;//253
		unsigned short reserved_parameter99;//254
		unsigned short reserved_parameter100;//255
}RUN_PARAMETER;
VARIABLE_EXT  RUN_PARAMETER  run_parameter;//moudlebus 接口用数据量		


//变量定义Stop  hugo add

VARIABLE_EXT void Read_Flash(void);
VARIABLE_EXT unsigned long FlashData_Addr;
VARIABLE_EXT unsigned char SpiFlash_Addr[5];
VARIABLE_EXT unsigned int EEpromData_Addr;
VARIABLE_EXT unsigned char EEPROM_Addr[3];
VARIABLE_EXT unsigned int EEpromAlarmData_Addr;
VARIABLE_EXT unsigned char EEPROM_ALARM_Addr[3];
VARIABLE_EXT void Spi_Flash_Addr_RW(unsigned long ddr);
VARIABLE_EXT unsigned long Spi_Flash_Addr_R(void);
VARIABLE_EXT unsigned int save_parameter(unsigned int address);

//guwei add
//VARIABLE_EXT void init_serial (void);
VARIABLE_EXT void Data_Analysis(unsigned char tata);
VARIABLE_EXT unsigned short crc16(unsigned char *puchMsg, unsigned short usDataLen);
VARIABLE_EXT void Data_Ack_Processor(void);//hugo add

VARIABLE_EXT void UpData_ModbBus(REALTIMEINFO *Time);
VARIABLE_EXT void Init_io(void);
VARIABLE_EXT int Init_ModBus (void);
VARIABLE_EXT int RW_ModBus_Data (void);

VARIABLE_EXT void Delay_ms(unsigned short value);
VARIABLE_EXT void Delay_us(unsigned short value);
VARIABLE_EXT void Spi_write_data(unsigned char data);
VARIABLE_EXT void AT25df16_reset(void);
VARIABLE_EXT unsigned char Spi_read_data(void);
VARIABLE_EXT void AT25df16_write_data(unsigned char *pbuffer,unsigned short data_count,unsigned int start_address);
VARIABLE_EXT void AT25df16_erase_4Kmap(unsigned int block_address);
VARIABLE_EXT void AT25df16_write_data_anywhere(unsigned short data_count,unsigned int start_address);
VARIABLE_EXT void AT25df16_erase_block(unsigned int block_address);
VARIABLE_EXT void AT25df16_read_data(unsigned char *pbuffer,unsigned short data_count,unsigned int start_address);
VARIABLE_EXT void AT25df16_read_data_anywhere(unsigned short data_count,unsigned int start_address);
VARIABLE_EXT void AT25df16_read_ID(unsigned char *pbuffer);
VARIABLE_EXT void Spi_Flash_Data_read(void);//?SPI??

#endif

