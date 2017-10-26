/***********************************************
@		Description: This file is main input for Model3000 project.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/10/10.
***********************************************/

#include <stdio.h>                      /* standard I/O .h-file              */
#include <ctype.h>                      /* character functions               */
#include <LPC21xx.H>                    /* LPC21xx definitions               */
#include <string.h>

#include "main.h"                    /* global project definition file    */
#include "config.h"
#include "Peripherals_LPC2194.h"
#include "AD7738.h"
#include "DAC8568.h"
#include "Command.h"
#include "app.h"
#include "M25P16_FLASH.h"

/*-------------------------Global variable region----------------------*/
extern unsigned char flag_command;
extern unsigned char flag_function;
extern unsigned char rcv_buf[100];
extern volatile unsigned char rcv_new;
extern unsigned int rcv_cnt;
extern unsigned char cmd_tmp[CMD_LEN];
extern unsigned char cmd_buf[CMD_LEN];

unsigned char flag_screen=0;
extern int flag1, flag2, flag3;

extern unsigned char MODEL_TYPE;
extern float OilTemp;
extern float PcbTemp;
extern float H2_SENSE_Resistance;
int temperature = 70;

const char print_menu[] = 
	"\n"
	"OilTemp    H2_SENSE_Resistance    PcbTemp\n";
/***********************************************************
Function:	init peripherals.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: setup the timer counter 0 interrupt.
***********************************************************/
void init_peripherals(void)
{
	LED_init();
	init_serial();
	init_timer();
	init_PWM();
	SPI0_INIT();
	SPI1_INIT();
	AD7738_CS_INIT();
	DAC8568_CS_INIT();
}

/***********************************************************
Function:	Main function.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: main function for Model3000 project.
***********************************************************/
int main (void)  
{
	unsigned char buffer;
	unsigned char a;
	FrecInit();
	init_peripherals();
	UARTprintf("model3000 test\n");
	
//	M25P16_TEST();
	DAC8568_INIT_SET(temperature,0xF000);

	while (1)  
	{
		if(rcv_new==1)//���Դ����շ�
		{
		  rcv_new=0;
			UART0_SendData(rcv_buf,rcv_cnt);
			//UARTprintf("\n");
			//������յ�����
			a=get_true_char_stream(cmd_tmp,rcv_buf);
			UART0_SendData(cmd_tmp,a);
			UARTprintf("\n");
			UARTprintf("a=%d\n",a);
			memset(rcv_buf,0,rcv_cnt);
			rcv_cnt=0;
		}
		if(findcmdfunction(cmd_tmp)==1)
		{
			memset(cmd_tmp,0,a);
			flag_screen=1;
			UARTprintf("guanbihuixian\n");
		}
		switch(flag_command){
			case 1:
			{
			alarm_arg();			
			}
			break;
			case 2:
			
			break;
			case 3:
			break;
			case 4:
			break;
			case 5:
			break;
			case 6:
			break;
			case 7:
			break;
			case 8:
			break;
			case 9:
			break;
			case 10:
			break;
			case 11:
			break;
			case 12:
			break;
			case 13:
			break;
			case 14:
			break;
			case 15:
			break;
			default:
			break;			
		}

		switch (flag1)  {
			case 1:
			/* 1-4min capture 3min oil temp */
			flag1 = 0;
			UARTprintf("1-4min capture 3min oil temp\n");
			break;

			case 2:
			/* 4-1H4min set 50 temp, keep 1H  */
			DAC8568_INIT_SET(50,0xF000);
			flag1 = 0;
			UARTprintf("4-1H4min set 50 temp, keep 1H\n");
			break;

			case 3:
			/* 1H4min-1H7min stop heating, capture 3min oil temp */
			DAC8568_INIT_SET(0,0);
			flag1 = 0;
			UARTprintf("1H4min-1H7min stop heating, capture 3min oil temp\n");
			break;

			case 4:
			/* 1H7min-2H7min set 50 temp and keep 1H */
			DAC8568_INIT_SET(50,0xF000);
			flag1 = 0;
			UARTprintf("1H7min-2H7min set 50 temp and keep 1H\n");
			break;

			case 5:
			/* 2H7min-2H10min stop heating and capture oil temp 3min */
			DAC8568_INIT_SET(0,0);
			flag1 = 0;
			UARTprintf("2H7min-2H10min stop heating and capture oil temp 3min\n");
			break;

			case 6:
			/* 2H10min-3H40min set 70 temp and keep 1.5H */
			DAC8568_INIT_SET(70,0xF000);
			flag1 = 0;
			UARTprintf("2H10min-3H40min set 70 temp and keep 1.5H\n");
			break;

			case 7:
			/* 3H40min-4H10min set 50 temp and keep 0.5H */
			DAC8568_INIT_SET(50,0xF000);
			flag1 = 0;
			UARTprintf("3H40min-4H10min set 50 temp and keep 0.5H\n");
			break;

			case 8:
			/* 4H10min-4H13min stop heating and capture oil temp 3min */
			DAC8568_INIT_SET(0,0);
			flag1 = 0;
			UARTprintf("4H10min-4H13min stop heating and capture oil temp 3min\n");
			break;

			default:                                 /* Error Handling              */
			break;
		}

		switch (flag2)  {
			case 1:
			/*200ms LED*/
			LED_RED_SET
			LED_BLUE_SET
			LED_RED_CLR
			LED_BLUE_CLR
			flag2 = 0;
//			UARTprintf("200ms LED\n");
			break;

			case 2:
			/*300ms ADC*/
			ADC7738_acquisition(1);
			ADC7738_acquisition_output(1);
			ADC7738_acquisition(2);
			ADC7738_acquisition_output(2);
			ADC7738_acquisition(3);
			ADC7738_acquisition_output(3);
			UARTprintf(print_menu);
			UARTprintf("%.3f     %.3f          %.3f\n",OilTemp,H2_SENSE_Resistance,PcbTemp);
			flag2 = 0;
//			UARTprintf("300ms ADC\n");
			break;

			case 3:
			/*600 ms checkself*/
			device_checkself();
			flag2 = 0;
//			UARTprintf("600 ms checkself\n");
			break;

			case 4:
			/*800ms DS1390 */
			flag2 = 0;
//			UARTprintf("800ms DS1390\n");
			break;

			default:                                 /* Error Handling              */
			break;
		}

		if (flag3 == 1)
		{
			/*30min FLASH*/
			flag3 = 0;
//			UARTprintf("30min FLASH\n");
		}
	}
}

