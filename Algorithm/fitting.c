/***********************************************
@		Description: This file is about "The least square method to fitting line equation" data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/11/13.
***********************************************/
#include "Peripherals_LPC2194.h"
#include <stdio.h>
#include "fitting.h"

extern float OilTemp_Tmp[1000];
extern float H2Resistor_Tmp[1000];

extern float DAC_Din[5];
extern float Din_temp[5];

//float OilTemp_Tmp[10] = {1.9,0.8,1.1,0.1,-0.1,4.4,4.6,1.6,5.5,3.4};
//float H2Resistor_Tmp[10] = {0.7,-1.0,-0.2,-1.2,-0.1,3.4,0.0,0.8,3.7,2.0};

float H2Resistor_OilTemp_K = 0;
float H2Resistor_OilTemp_B = 0;

float Din_temp_DAC_Din_K = 0;
float Din_temp_DAC_Din_B = 0;

void Line_Fit(float *X, float *Y)
{
    double x_sum_average = 0;   
    double y_sum_average = 0;
    double x_square_sum = 0;
    double x_multiply_y = 0;
    unsigned int length = 0, i = 0;
    double sum = 0;
	
    if ((X == OilTemp_Tmp) && (Y == H2Resistor_Tmp)){
        if (sizeof(OilTemp_Tmp)/sizeof(OilTemp_Tmp[0]) == sizeof(H2Resistor_Tmp)/sizeof(H2Resistor_Tmp[0])){
            length = sizeof(H2Resistor_Tmp)/sizeof(H2Resistor_Tmp[0]);

            for(i=0;i<length;i++)
            {
                sum += OilTemp_Tmp[i];
            } 
            x_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += H2Resistor_Tmp[i];
            } 
            y_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += OilTemp_Tmp[i]*OilTemp_Tmp[i];
            }
            x_square_sum = sum;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += OilTemp_Tmp[i]*H2Resistor_Tmp[i];
            }
            x_multiply_y = sum;
            
            H2Resistor_OilTemp_K = ( x_multiply_y - length * x_sum_average * y_sum_average)/(x_square_sum - length * x_sum_average * x_sum_average);
            H2Resistor_OilTemp_B = y_sum_average - H2Resistor_OilTemp_K * x_sum_average;
            //	UARTprintf("%.3f,%.4f\n",H2Resistor_OilTemp_K,H2Resistor_OilTemp_B);
        }
    }else if ((X == DAC_Din) && (Y == Din_temp)){
        if (sizeof(DAC_Din)/sizeof(DAC_Din[0]) == sizeof(Din_temp)/sizeof(Din_temp[0])){
            length = sizeof(Din_temp)/sizeof(Din_temp[0]);

            for(i=0;i<length;i++)
            {
                sum += DAC_Din[i];
            } 
            x_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Din_temp[i];
            } 
            y_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += DAC_Din[i]*DAC_Din[i];
            }
            x_square_sum = sum;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += DAC_Din[i]*Din_temp[i];
            }
            x_multiply_y = sum;
            
            Din_temp_DAC_Din_K = ( x_multiply_y - length * x_sum_average * y_sum_average)/(x_square_sum - length * x_sum_average * x_sum_average);
            Din_temp_DAC_Din_B = y_sum_average - Din_temp_DAC_Din_K * x_sum_average;
            //	UARTprintf("%.3f,%.4f\n",Din_temp_DAC_Din_K,Din_temp_DAC_Din_B);
        }
    }
}

