/***********************************************
@		Description: This file is about "The least square method to fitting line equation" data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/11/13.
***********************************************/
#include "Peripherals_LPC2194.h"
#include <stdio.h>
#include "fitting.h"
#include "parameter.h"
#include <math.h>

void Line_Fit(float *X, float *Y)
{
    double x_sum_average = 0;   
    double y_sum_average = 0;
    double x_square_sum = 0;
    double x_multiply_y = 0;
    unsigned int length = 0, i = 0;
    double sum = 0;
	
		if ((X == Intermediate_Data.Temp_R) && (Y == Intermediate_Data.Temp)){
			if (sizeof(Intermediate_Data.Temp)/sizeof(Intermediate_Data.Temp[0]) == sizeof(Intermediate_Data.Temp_R)/sizeof(Intermediate_Data.Temp_R[0])){
					length = sizeof(Intermediate_Data.Temp)/sizeof(Intermediate_Data.Temp[0]);

					for(i=0;i<length;i++)
					{
							sum += Intermediate_Data.Temp_R[i];
					} 
					x_sum_average = sum / length;

					sum = 0;
					for(i=0;i<length;i++)
					{
							sum += Intermediate_Data.Temp[i];
					} 
					y_sum_average = sum / length;

					sum = 0;
					for(i=0;i<length;i++)
					{
							sum += Intermediate_Data.Temp_R[i]*Intermediate_Data.Temp_R[i];
					}
					x_square_sum = sum;

					sum = 0;
					for(i=0;i<length;i++)
					{
							sum += Intermediate_Data.Temp[i]*Intermediate_Data.Temp_R[i];
					}
					x_multiply_y = sum;
					
					Intermediate_Data.Temp_R_K = ( x_multiply_y - length * x_sum_average * y_sum_average)/(x_square_sum - length * x_sum_average * x_sum_average);
					Intermediate_Data.Temp_R_B = y_sum_average - Intermediate_Data.Temp_R_K * x_sum_average;
					//	UARTprintf("%.3f,%.4f\n",Temp_R_K,Temp_R_B);
			}
    }else if ((X == Intermediate_Data.OilTemp_Tmp) && (Y == Intermediate_Data.H2Resistor_Tmp)){
        if (sizeof(Intermediate_Data.OilTemp_Tmp)/sizeof(Intermediate_Data.OilTemp_Tmp[0]) == sizeof(Intermediate_Data.H2Resistor_Tmp)/sizeof(Intermediate_Data.H2Resistor_Tmp[0])){
            length = sizeof(Intermediate_Data.H2Resistor_Tmp)/sizeof(Intermediate_Data.H2Resistor_Tmp[0]);

            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.OilTemp_Tmp[i];
            } 
            x_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.H2Resistor_Tmp[i];
            } 
            y_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.OilTemp_Tmp[i]*Intermediate_Data.OilTemp_Tmp[i];
            }
            x_square_sum = sum;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.OilTemp_Tmp[i]*Intermediate_Data.H2Resistor_Tmp[i];
            }
            x_multiply_y = sum;
            
            Intermediate_Data.H2Resistor_OilTemp_K = ( x_multiply_y - length * x_sum_average * y_sum_average)/(x_square_sum - length * x_sum_average * x_sum_average);
            Intermediate_Data.H2Resistor_OilTemp_B = y_sum_average - Intermediate_Data.H2Resistor_OilTemp_K * x_sum_average;
            //UARTprintf("%.3f,%.4f\n",Intermediate_Data.H2Resistor_OilTemp_K,Intermediate_Data.H2Resistor_OilTemp_B);
        }
    }else if ((X == Intermediate_Data.Din_temp) && (Y == Intermediate_Data.DAC_Din)){
        if (sizeof(Intermediate_Data.DAC_Din)/sizeof(Intermediate_Data.DAC_Din[0]) == sizeof(Intermediate_Data.Din_temp)/sizeof(Intermediate_Data.Din_temp[0])){
            length = sizeof(Intermediate_Data.Din_temp)/sizeof(Intermediate_Data.Din_temp[0]);

            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.Din_temp[i];
            } 
            x_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.DAC_Din[i];
            } 
            y_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.Din_temp[i]*Intermediate_Data.Din_temp[i];
            }
            x_square_sum = sum;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.DAC_Din[i]*Intermediate_Data.Din_temp[i];
            }
            x_multiply_y = sum;
            
            Intermediate_Data.Din_temp_DAC_Din_K = ( x_multiply_y - length * x_sum_average * y_sum_average)/(x_square_sum - length * x_sum_average * x_sum_average);
            Intermediate_Data.Din_temp_DAC_Din_B = y_sum_average - Intermediate_Data.Din_temp_DAC_Din_K * x_sum_average;
            //	UARTprintf("%.3f,%.4f\n",Din_temp_DAC_Din_K,Din_temp_DAC_Din_B);
        }
    }else if ((X == Intermediate_Data.PCB_TEMP_SET) && (Y == Intermediate_Data.PCB_TEMP_Din)){
        if (sizeof(Intermediate_Data.PCB_TEMP_Din)/sizeof(Intermediate_Data.PCB_TEMP_Din[0]) == sizeof(Intermediate_Data.PCB_TEMP_SET)/sizeof(Intermediate_Data.PCB_TEMP_SET[0])){
            length = sizeof(Intermediate_Data.PCB_TEMP_Din)/sizeof(Intermediate_Data.PCB_TEMP_Din[0]);

            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.PCB_TEMP_SET[i];
            } 
            x_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.PCB_TEMP_Din[i];
            } 
            y_sum_average = sum / length;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.PCB_TEMP_SET[i]*Intermediate_Data.PCB_TEMP_SET[i];
            }
            x_square_sum = sum;

            sum = 0;
            for(i=0;i<length;i++)
            {
                sum += Intermediate_Data.PCB_TEMP_Din[i]*Intermediate_Data.PCB_TEMP_SET[i];
            }
            x_multiply_y = sum;
            
            Intermediate_Data.PCB_TEMP_Din_K = ( x_multiply_y - length * x_sum_average * y_sum_average)/(x_square_sum - length * x_sum_average * x_sum_average);
            Intermediate_Data.PCB_TEMP_Din_B = y_sum_average - Intermediate_Data.PCB_TEMP_Din_K * x_sum_average;
            //	UARTprintf("%.3f,%.4f\n",PCB_TEMP_Din_K,PCB_TEMP_Din_B);
        }
    }
}

/***********************************************************
Function: quadratic polynomial.
Input: 
Output:
Author: zhuobin
Date: 2017/11/30
Description: H2-OHM.
Linear model Poly2:
       f(x) = p1*x^2 + p2*x + p3

***********************************************************/
float quadratic_polynomial(float data)
{
	static unsigned char number = 0;
  float tmp = 0;
	double p1 = 0, p2 = 0, p3 = 0;
	
	if (number == 0)
	    number = sizeof(Intermediate_Data.OHM)/sizeof(Intermediate_Data.OHM[0]);
	
	if (data >= Intermediate_Data.OHM[0] && data < Intermediate_Data.OHM[4]){
	  p1 = 121.8358416;
    p2 = -233115.092865;
		p3 = 111507922.798009;
	}else if (data >= Intermediate_Data.OHM[4] && data < Intermediate_Data.OHM[8]){
	  p1 = 256.222696;
    p2 = -490933.428946;
		p3 = 235162655.440190;
	}else if (data >= Intermediate_Data.OHM[8] && data < Intermediate_Data.OHM[12]){
	  p1 = 409.052293;
    p2 = -785623.724501;
		p3 = 377220150.322393;
	}
	tmp = p1 * data * data + p2 * data + p3;
	return tmp;
}
