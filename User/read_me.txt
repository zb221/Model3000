/***********************************************
@		Description: This file is about record program progress.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/12/22.
***********************************************/

1. 上位机与下位机氢浓度的日变化率变量需要统一确认。
目前：
	run_parameter.h2_ppm_alarm_low_h16.hilo = 0;
	run_parameter.h2_ppm_alarm_low_l16.hilo = 2500; //DayDRC

	run_parameter.h2_ppm_alert_low_h16.hilo = 0;
	run_parameter.h2_ppm_alert_low_l16.hilo = 2500; //H2DG

	run_parameter.OilTemp_Alarm_celsius.hilo = 70;

2. 