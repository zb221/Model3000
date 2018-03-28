/***********************************************
@		Description: This file contants all parameters for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: megzheng.
@		Date: 2017/10/19.
***********************************************/
#ifdef VARIABLE_GLOBALS
#define VARIABLE_EXT
#else
#define VARIABLE_EXT extern
#endif

#ifndef __PARAMETER_H__
#define __PARAMETER_H__

typedef struct
{
	union
	{
		struct
		{
			unsigned short recept_ok				 :1;				//1  ????????
			unsigned short recept_data_error :1;				//1  ?????????
			unsigned short recept_crc_error  :1;				//1  ????crc????
			unsigned short recept_write			 :1;				//1  ????????only	 write			
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
			
	unsigned char recept_point;//?????????
	unsigned char recept_data_buffer[8];//???????
	unsigned char recept_function_code;//03 ??????;06????
	unsigned short calculate_crc;//?????????CEC?
	unsigned char send_buffer[260];
	unsigned char spi_flash_buffer[256];//flash ????
	unsigned short function_point;			    //1  ????????only	 write
	unsigned char Alarm_data_buffer[8];
}USER_PARAMETER;
//VARIABLE_EXT  
//USER_PARAMETER  user_parameter;	
VARIABLE_EXT USER_PARAMETER user_parameter;

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
			unsigned char lo;//2017.06.16?8?8??
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

	union
	{
	struct
		{
				 unsigned short A1;//19
				 unsigned short A2;//20
				unsigned short A3;//21
				unsigned short A4;//22
				unsigned short A5;//23
				unsigned short A6;//24
				unsigned short A7;//25
				unsigned short A8;//26
				unsigned short A9;//27
				unsigned short A10;//28
				unsigned short A11;//29
				unsigned short A12;//30

		}A;
//		double Fit_Para_A[3];
		unsigned short Fit_Para_A[12];
	}Sensor_Fit_Para_A;
		
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
		}model_number_sstr;
		char model_number_str[20];		
	}model_number;
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
union
{		
	struct
	  {
		unsigned char character1;
		unsigned char character2;//102
		unsigned char character3;
		unsigned char character4;//103
		unsigned char character5;
		unsigned char character6;//104
		unsigned char character7;
		unsigned char character8;//105
		}factory_sstr;
		char factory_str[8];
	}factory;//99
union
{		
	struct
	  {
		unsigned char character1;
		unsigned char character2;//106
		unsigned char character3;
		unsigned char character4;//107
		unsigned char character5;
		unsigned char character6;//108
		unsigned char character7;
		unsigned char character8;//109
		}hardware_version_sstr;
		char hardware_version_str[8];
	}hardware_version;
	unsigned short reserved_parameter24;//110
	union
	{
		struct
		{
			unsigned short senser_state0			:1;			
			unsigned short senser_state1			:1;
			unsigned short senser_state2			:1;
			unsigned short reserved7				  :1;
			unsigned short relay1				  		:1;		
			unsigned short relay2				  		:1;			
			unsigned short relay3				  		:1;			
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
	
		
		struct
		{
			unsigned char year;				
			unsigned char month;  //112
			unsigned char day;		  
			unsigned char hour;		//113		 
			unsigned char minute;			 	
			unsigned char second;	//114		  																				
			}realtime;//114

	unsigned short reserved_parameter28;//115 -> M25P16_Alarm_Log_Records: Intermediate_Data.Alarm_page
	unsigned short reserved_parameter29;//116 -> M25P16_Data_Records: Intermediate_Data.M25P16_Data_Addr
	unsigned short reserved_parameter30;//117 -> M25P16_Data_Records: Intermediate_Data.M25P16_Data_Addr
			
	unsigned short reserved_parameter31;//118 -> CI cmd current cal
	unsigned short reserved_parameter32;//119 -> CI cmd current cal
	unsigned short reserved_parameter33;//120 -> OilTemp_b from upper
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
		
	unsigned short reserved_parameter341;//130 month,day
	unsigned short reserved_parameter35;//131 year
	unsigned short reserved_parameter36;//132 hour
	unsigned short reserved_parameter37;//133 min/second
		
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
		}	unit_id;//150
	unsigned short operating_mode;//151	 ????
		
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
		}h2_ppm_alarm_low_h16;//154 DRC alarm?
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
//		union
//	{
//		struct
//		{
//			unsigned char lo;
//			unsigned char hi;		
//			}ubit;	
//		unsigned short hilo;
//	}h2_ppm_alarm_DRC;//157		
				
	unsigned short temp_cal;//157
	unsigned short temp_cal_flag;//158
	unsigned short reserved_parameter52;//159
	unsigned short MODEL_TYPE;//160 MODEL_TYPE
	union
	{
		struct
		{
			unsigned char lo;
			unsigned char hi;	
			}ubit;	
		unsigned short hilo;
		}die_temperature_counts_h16;//161 sensor temperature count
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

		union
		{
		struct
			{
				unsigned short B1;//179
				unsigned short B2;//180
				unsigned short B3;//181
				unsigned short B4;//182
				unsigned short B5;//183
				unsigned short B6;//184
				unsigned short B7;//185
				unsigned short B8;//186
				unsigned short B9;//187
				unsigned short B10;//188
				unsigned short B11;//189
				unsigned short B12;//190
			}B;
			 unsigned short Fit_Para_B[12];
		}Sensor_Fit_Para_B;
		
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
		unsigned char character20;//230
		}transformer_id_sstr;
		char transformer_id_str[20];
}transformer_id;

		union
		{
		struct
			{
				unsigned short C1;//231
				unsigned short C2;//232
				unsigned short C3;//233
				unsigned short C4;//234
				unsigned short C5;//235
				unsigned short C6;//236
				unsigned short C7;//237
				unsigned short C8;//238
				unsigned short C9;//239
				unsigned short C10;//240
				unsigned short C11;//241
				unsigned short C12;//242

			}C;
			unsigned short Fit_Para_C[12];
		}Sensor_Fit_Para_C;
		
		union
		{
		struct
			{
				unsigned short hi;//243
				unsigned short lo;//244
			}ubit;
			unsigned short point0[2];
		}Piecewise_point0;

		union
		{
		struct
			{
				unsigned short hi;//245
				unsigned short lo;//246
			}ubit;
			unsigned short point1[2];
		}Piecewise_point1;

		union
		{
		struct
			{
				unsigned short hi;//247
				unsigned short lo;//248
			}ubit;
			unsigned short point2[2];
		}Piecewise_point2;

		union
		{
		struct
			{
				unsigned short hi;//249
				unsigned short lo;//250
			}ubit;
			unsigned short point3[2];
		}Piecewise_point3;
		
		unsigned short Sensor_Fit_Para_Done;//251
		
		unsigned short Block_mark_Done;//252
		
		unsigned short reserved_parameter77;//253
		unsigned short reserved_parameter78;//254
		unsigned short reserved_parameter100;//255
}RUN_PARAMETER;
//RUN_PARAMETER  run_parameter;
VARIABLE_EXT  RUN_PARAMETER  run_parameter;//moudlebus ??????		

VARIABLE_EXT unsigned long FlashData_Addr;
VARIABLE_EXT unsigned long long int Runtimes;	

typedef struct CMD_ConfigData
{  
 unsigned int   slaveID;//={1};	
 float     H2low;//=0;
 float     H2high;//=40000;
 float     MaxAlertH2;
 float     MaxAlarmH2;	
 unsigned long  Caldate;//=20160621;	
 unsigned int   inter_time;//=0;
 float          LowmA;//cmd_ConfigData.LowmA;
 float          HighmA;
 float          ErrmA;
 float          NotRmA;	
// float          LowVout;//cmd_ConfigData.LowmA;
// float          HighVout;
// float          ErrVout;
// float          NotRVout;		
// float          h2cdata_ka;//=1;//?????1
// float          h2cdata_kb;//=1;
// float          h2cdata_ka_temp;//=1;//?????1
// float          h2cdata_kb_temp;//=1; 
// float          OutCi_ka;//=1;//?????1
// float          OutCi_kb;//=1;
// float          OutCi_ka_temp;//=1;//?????1
// float          OutCi_kb_temp;//=1;
// float          OutCv_ka;//=1;//?????1
// float          OutCv_kb;//=1;
// float          OutCv_ka_temp;//=1;//?????1
// float          OutCv_kb_temp;//=1;
 float     MaxAlarmOil;
}ConfigFlashEE;
VARIABLE_EXT ConfigFlashEE   cmd_ConfigData;


typedef union __real_time_info__ 
{
	unsigned char Real_Time[10];
	struct
	{
		unsigned char	  year;						//秒
		unsigned char   month;					//分
		unsigned char   day;					//
		unsigned char   hour;					//日			///unsigned char   week;	
    unsigned char   min;					//月
		unsigned char	  sec;					//年     
		unsigned char	  week;
	}SpecificTime;	
}REALTIMEINFO;
VARIABLE_EXT REALTIMEINFO CurrentTime;

typedef struct Output_Data
{
	unsigned char MODEL_TYPE; /*1->normal model; 2->debug model; 3->calibrate model*/

	int temperature;    /*sense heating temperature*/
	int PCB_temp;	    /*PCB control default temperature*/

	float PcbTemp;
	float OilTemp;
	float TempResistor;
	float H2Resistor;
	float H2R;

	float H2AG;
	float H2AG1;
	float H2DG;
	float H2G;
	float H2SldAv;
	float DayROC;
	float WeekROC;
	float MonthROC;
	float SensorTemp;

}OUTPUT_Parameters;
VARIABLE_EXT OUTPUT_Parameters output_data;

typedef struct Intermediate_Data
{
	unsigned char flag1, flag2, flag3, flag4, flag5;
	unsigned char Start_day, Start_week, Start_month;
	unsigned char Power_On,Start_print_H2R, wait_1min,Start_print_calibrate_H2R,wait_1min_oil,current_cal;
	
	float Heat_V;

	float H2G_tmp[48];
	float PCB_TEMP_Din[3];
	float PCB_TEMP_SET[3];
	
	float Temp[4];      
	float Temp_R[4]; 
	float DAC_Din[5]; 
	float Din_temp[5]; 
	
	float H2[13];
	float OHM[13];

	float OilTemp_Tmp[1000];
	float H2Resistor_Tmp[1000];
	float H2Resistor_Tmp_1[200];    /* Data for filtering processing */
	float H2Resistor_Tmp_2[30];    /* Data for filtering processing */
	
	float SensorTemp_tmp[10];
	
	float H2Resistor_OilTemp_K;
	float H2Resistor_OilTemp_B;

	float Din_temp_DAC_Din_K;
	float Din_temp_DAC_Din_B;

	float PCB_TEMP_Din_K;
	float PCB_TEMP_Din_B;

	float Temp_R_K;
	float Temp_R_B;
	
	unsigned int da_H2ppm;
	unsigned int db_H2ppm;
	
	unsigned char unready_current;
	
	unsigned int M25P16_Data_Addr;
//	unsigned char sector;
//	unsigned short page;
	unsigned short Alarm_page;
	
	unsigned int count6, count7;
	
	unsigned char Operat_temp_alarm;
	
	unsigned char Oiltemp_Cal_flag;
	
	unsigned char Oiltemp_Cal_OK;
	
	float intercept;

}Intermediate_Parameters;
VARIABLE_EXT Intermediate_Parameters Intermediate_Data;

//typedef struct Sensor_Data
//{
//	unsigned char original_data[80];
//	unsigned char Sensor_Fit_Para[72];
//	
//	double p1;
//	double p2;
//	double p3;
//	double p4;
//	double p5;
//	double p6;
//	double p7;
//	double p8;
//	double p9;
//	
//}Sensor_Parameters;
//VARIABLE_EXT Sensor_Parameters Sensor_Data;
#ifndef DEBUG
#define DEBUG
#endif
#endif

