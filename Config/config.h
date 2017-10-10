/****************************************Copyright (c)**************************************************
**--------------File Info-------------------------------------------------------------------------------
** File Name: config.h
** Last modified Date:  2017-10-13
** Last Version: 1.0
** Descriptions: User Configurable File
**
**------------------------------------------------------------------------------------------------------
** Created By: megzheng
** Created date:   2017-10-13
** Version: 1.0
** Descriptions: First version
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
********************************************************************************************************/

#ifndef __CONFIG_H 
#define __CONFIG_H

#include <LPC21xx.H>
//M=4
#define Fosc            12000000                    //Crystal frequence,10MHz~25MHz��should be the same as actual status. 
						    //Ӧ����ʵ��һ������Ƶ��,10MHz~25MHz��Ӧ����ʵ��һ��
#define Fcclk           (Fosc * 4)                  //System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz. 
						    //ϵͳƵ�ʣ�����ΪFosc��������(1~32)����<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO frequence,should be 2��4��8��16 multiples of Fcclk, ranged from 156MHz to 320MHz. 
						    //CCOƵ�ʣ�����ΪFcclk��2��4��8��16������ΧΪ156MHz~320MHz
#define Fpclk           (Fcclk / 4) * 1             //VPB clock frequence , must be 1��2��4 multiples of (Fcclk / 4).


#endif


