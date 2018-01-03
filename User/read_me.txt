/***********************************************
@		Description: This file is about record program progress.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/12/22.
***********************************************/

1. ��λ������λ����Ũ�ȵ��ձ仯�ʱ�����Ҫͳһȷ�ϡ�
Ŀǰ��
	run_parameter.h2_ppm_alarm_low_h16.hilo = 0;
	run_parameter.h2_ppm_alarm_low_l16.hilo = 2500; //DayDRC

	run_parameter.h2_ppm_alert_low_h16.hilo = 0;
	run_parameter.h2_ppm_alert_low_l16.hilo = 2500; //H2DG

	run_parameter.OilTemp_Alarm_celsius.hilo = 70;
����취��ȷ�ϱ���

2. �����¸��ڴ����������¶�ʱ������α�����
Intermediate_Data.Operat_temp_alarm = 1
����취����λ��״̬��ʾ��ʹ�ñ�����111״̬�ָ���

3. ������������ظ�
M25P16_Alarm_Log_Records
M25P16_Data_Records
����취����

***4. У׼4-20mAģ�������CI���
***5. ��ͱ궨����Ũ��������ʾ����

6. ��������111״̬�ָ���
��������´�����״̬�ͼ̵���״̬

7. Flash���ݴ洢������5������
�������λ������

8. ϵͳʱ��
IS/RS�����У�����ʱ�����Ϊ��
struct
{
	unsigned char year;				
	unsigned char month;  //112
	unsigned char day;		  
	unsigned char hour;		//113		 
	unsigned char minute;			 	
	unsigned char second;	//114		  																				
}realtime;//114

modbus:
	unsigned short reserved_parameter341;//130 month,day
	unsigned short reserved_parameter35;//131 year
	unsigned short reserved_parameter36;//132 hour
	unsigned short reserved_parameter37;//133 min/second
	
update_modbus:
	run_parameter.reserved_parameter35 = (0x20<<8 | TimeBCD.SpecificTime.year);//?
	run_parameter.reserved_parameter341 = (TimeBCD.SpecificTime.month<<8 | TimeBCD.SpecificTime.day);
	run_parameter.reserved_parameter36 = TimeBCD.SpecificTime.hour;
	run_parameter.reserved_parameter37 = (TimeBCD.SpecificTime.min<<8 | TimeBCD.SpecificTime.sec);
