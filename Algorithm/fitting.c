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

extern float PCB_TEMP_Din[3];
extern float PCB_TEMP_SET[3];

extern float Temp[4];
extern float Temp_R[4];

//float OilTemp_Tmp[10] = {1.9,0.8,1.1,0.1,-0.1,4.4,4.6,1.6,5.5,3.4};
//float H2Resistor_Tmp[10] = {0.7,-1.0,-0.2,-1.2,-0.1,3.4,0.0,0.8,3.7,2.0};

float H2Resistor_OilTemp_K = 0;
float H2Resistor_OilTemp_B = 0;

float Din_temp_DAC_Din_K = 0;
float Din_temp_DAC_Din_B = 0;

float PCB_TEMP_Din_K = 0;
float PCB_TEMP_Din_B = 0;

float Temp_R_K = 0;
float Temp_R_B = 0;

void Line_Fit(float *X, float *Y)
{
    double x_sum_average = 0;   
    double y_sum_average = 0;
    double x_square_sum = 0;
    double x_multiply_y = 0;
    unsigned int length = 0, i = 0;
    double sum = 0;
	
		if ((X == Temp_R) && (Y == Temp)){
			if (sizeof(Temp)/sizeof(Temp[0]) == sizeof(Temp_R)/sizeof(Temp_R[0])){
					length = sizeof(Temp)/sizeof(Temp[0]);

					for(i=0;i<length;i++)
					{
							sum += Temp_R[i];
					} 
					x_sum_average = sum / length;

					sum = 0;
					for(i=0;i<length;i++)
					{
							sum += Temp[i];
					} 
					y_sum_average = sum / length;

					sum = 0;
					for(i=0;i<length;i++)
					{
							sum += Temp_R[i]*Temp_R[i];
					}
					x_square_sum = sum;

					sum = 0;
					for(i=0;i<length;i++)
					{
							sum += Temp[i]*Temp_R[i];
					}
					x_multiply_y = sum;
					
					Temp_R_K = ( x_multiply_y - length * x_sum_average * y_sum_average)/(x_square_sum - length * x_sum_average * x_sum_average);
					Temp_R_B = y_sum_average - Temp_R_K * x_sum_average;
					//	UARTprintf("%.3f,%.4f\n",Temp_R_K,Temp_R_B);
			}
    }else if ((X == OilTemp_Tmp) && (Y == H2Resistor_Tmp)){
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
    }else if ((X == Din_temp) && (Y == DAC_Din)){
        if (sizeof(DAC_Din)/sizeof(DAC_Din[0]) == sizeof(Din_temp)/sizeof(Din_temp[0])){
            length = sizeof(Din_temp)/sizeof(Din_temp[0]);

            for(i=0;i<length;i++)
            {
                sum += Din_temp[i];
            } 
            x_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += DAC_Din[i];
            } 
            y_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Din_temp[i]*Din_temp[i];
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
    }else if ((X == PCB_TEMP_SET) && (Y == PCB_TEMP_Din)){
        if (sizeof(PCB_TEMP_Din)/sizeof(PCB_TEMP_Din[0]) == sizeof(PCB_TEMP_SET)/sizeof(PCB_TEMP_SET[0])){
            length = sizeof(PCB_TEMP_Din)/sizeof(PCB_TEMP_Din[0]);

            for(i=0;i<length;i++)
            {
                sum += PCB_TEMP_SET[i];
            } 
            x_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += PCB_TEMP_Din[i];
            } 
            y_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += PCB_TEMP_SET[i]*PCB_TEMP_SET[i];
            }
            x_square_sum = sum;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += PCB_TEMP_Din[i]*PCB_TEMP_SET[i];
            }
            x_multiply_y = sum;
            
            PCB_TEMP_Din_K = ( x_multiply_y - length * x_sum_average * y_sum_average)/(x_square_sum - length * x_sum_average * x_sum_average);
            PCB_TEMP_Din_B = y_sum_average - PCB_TEMP_Din_K * x_sum_average;
            //	UARTprintf("%.3f,%.4f\n",PCB_TEMP_Din_K,PCB_TEMP_Din_B);
        }
    }
}

