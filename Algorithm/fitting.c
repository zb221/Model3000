/***********************************************
@		Description: This file is about "The least square method to fitting line equation" data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/11/13.
***********************************************/
#include "Peripherals_LPC2194.h"
#include <stdio.h>
#include "fitting.h"

//#define N 10             //N??????????

extern float OilTemp_Tmp[1000];
extern float H2Resistor_Tmp[1000];
#define N 1000 

//float OilTemp_Tmp[10] = {1.9,0.8,1.1,0.1,-0.1,4.4,4.6,1.6,5.5,3.4};
//float H2Resistor_Tmp[10] = {0.7,-1.0,-0.2,-1.2,-0.1,3.4,0.0,0.8,3.7,2.0};

float H2Resistor_OilTemp_K=0;
float H2Resistor_OilTemp_B=0;
double x_sum_average=0;   //?? X[N] ????? ?????
double y_sum_average=0;   //?? Y[N] ????? ?????
double x_square_sum=0;    //?? X[N] ????????
double x_multiply_y=0;    //?? X[N]?Y[N]???????

double Sum_Average(float d[N])
{
    unsigned int i=0;
    double z=0;
    for(i=0;i<N;i++)
    {
         z = z + d[i];
    }
    z = z/N;
    return z;
}
double X_Y_By(float m[N],float n[N])
{
    unsigned int i=0;
    double z=0;
    for(i=0;i<N;i++)
    {
        z = z + m[i]*n[i];
    }
    return z;
}

double Squre_sum(float c[N])
{
    unsigned int i=0;
    double z=0;
    for(i=0;i<N;i++)
    {
         z = z + c[i]*c[i];
    }
    return z;
}

void Line_Fit(void)
{
  x_sum_average= Sum_Average(OilTemp_Tmp);
  y_sum_average= Sum_Average(H2Resistor_Tmp);

  x_square_sum = Squre_sum(OilTemp_Tmp);
  x_multiply_y = X_Y_By(OilTemp_Tmp,H2Resistor_Tmp);

  H2Resistor_OilTemp_K = ( x_multiply_y - N * x_sum_average * y_sum_average)/( x_square_sum - N * x_sum_average*x_sum_average );
  H2Resistor_OilTemp_B = y_sum_average - H2Resistor_OilTemp_K * x_sum_average;
//	UARTprintf("%.3f,%.4f\n",H2Resistor_OilTemp_K,H2Resistor_OilTemp_B);
}

