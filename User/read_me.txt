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
解决办法：确认变量

2. 当油温高于传感器工作温度时，该如何报警？
Intermediate_Data.Operat_temp_alarm = 1
解决办法：上位机状态显示，使用变量表111状态字更新

3. 扇区与变量的重复
M25P16_Alarm_Log_Records
M25P16_Data_Records
解决办法：。

***4. 校准4-20mA模拟输出（CI命令）
***5. 最低标定氢气浓度以下显示问题

6. 变量表中111状态字更新
解决：更新传感器状态和继电器状态

7. Flash数据存储，保存5年数据
解决：上位机调试
