/***********************************************
@		Description: This file is about Cubic.c data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/

#include <stdio.h>
#include <math.h>
#include "Cubic.h"

/***********************************************************
Description: Global variable region.
Author: zhuobin
Date: 2017/10/10
***********************************************************/
#define  MAXNUM  50   

/*The relationship between H2 and H2_resistance*/
float H2[] = {0.5,0.8,1.0,2.0,4.0,6.0,8.0,10.0,15.0,20.0,30,40,60,80,100};
float OHM[] = {607.732,608.799,609.422,611.832,615.097,617.59,619.734,621.5,625.587,628.711,634.544,639.727,648.659,656.587,663.789};

/*The relationship between temperature_resistance/H2_resistance and temperature*/
float Temp[] = {30,45,60,75,90};

//float Temp_R[] = {97.952,102.574,107.090,111.605,116.121};

float Temp_R[] = {91.366,96.190,101.087,106.101,111.456};
//float Temp_R[] = {97.952,102.574,107.090,111.605,116.121};	/*zsy*/
//float Temp[] = {63.5,98};
//float Temp_R[] = {164.7,182.3};
float Hydrogen_R[] = {556.448,567.663,578.720,589.298,600.106};

/*The relationship between temperature of sense_temperature_resistance and DAC Din*/
float DAC_Din[] =  {38864,39083,39421,39754,40247,40960,41600};
float Din_temp[] = {35.080,43.085,58.520,75.125,96.450,128.450,157.625};

/*DAC8568 Din data - PCB temp control*/
float PCB_TEMP_Din[] =  {11336,11536};
float PCB_TEMP_SET[] = {41.336,45.835};

typedef struct SPLINE    
{ 
    float x[MAXNUM+1];    
    float y[MAXNUM+1];   
    int point_num;   
    float begin_k1;    
    float end_k1;     
    //float begin_k2;    
    //float end_k2;     
    float k1[MAXNUM+1];    
    float k2[MAXNUM+1];   
    float a3[MAXNUM],a1[MAXNUM];
    float b3[MAXNUM],b1[MAXNUM];
    //Si(x) = a3[i] * {x(i+1) - x}^3  + a1[i] * {x(i+1) - x} +
    //        b3[i] * {x - x(i)}^3 + b1[i] * {x - x(i)}
    //xi?x[i]??,xi_1?x[i+1]??
}SPLINE,*pSPLINE;

SPLINE line1;
pSPLINE pLine1 = &line1;

typedef int RESULT;      

/***********************************************************
Function: Spline3.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
RESULT Spline3(pSPLINE pLine)
{
    float H[MAXNUM] = {0};     
    float Fi[MAXNUM] = {0};     
    float U[MAXNUM+1] = {0};   
    float A[MAXNUM+1] = {0};   
    float D[MAXNUM+1] = {0};    
    float M[MAXNUM+1] = {0};    
    float B[MAXNUM+1] = {0};    
    float Y[MAXNUM+1] = {0};    
    int i = 0;

    if((pLine->point_num < 3) || (pLine->point_num > MAXNUM + 1))
    {
        return ERR;       
    }

    for(i = 0;i <= pLine->point_num - 2;i++)
    {          
        H[i] = pLine->x[i+1] - pLine->x[i];
        Fi[i] = (pLine->y[i+1] - pLine->y[i]) / H[i]; //?F[x(i),x(i+1)]
    }

    for(i = 1;i <= pLine->point_num - 2;i++)
    {          
        U[i] = H[i-1] / (H[i-1] + H[i]);
        A[i] = H[i] / (H[i-1] + H[i]);
        D[i] = 6 * (Fi[i] - Fi[i-1]) / (H[i-1] + H[i]);
    }

    U[i] = 1;
    A[0] = 1;
    D[0] = 6 * (Fi[0] - pLine->begin_k1) / H[0];
    D[i] = 6 * (pLine->end_k1 - Fi[i-1]) / H[i-1];
    //U[i] = 0;
    //A[0] = 0;
    //D[0] = 2 * begin_k2;
    //D[i] = 2 * end_k2;

    B[0] = A[0] / 2;
    for(i = 1;i <= pLine->point_num - 2;i++)
    {
        B[i] = A[i] / (2 - U[i] * B[i-1]);
    }
    Y[0] = D[0] / 2;
    for(i = 1;i <= pLine->point_num - 1;i++)
    {
        Y[i] = (D[i] - U[i] * Y[i-1]) / (2 - U[i] * B[i-1]);
    }
    M[pLine->point_num - 1] = Y[pLine->point_num - 1];
    for(i = pLine->point_num - 1;i > 0;i--)
    {
        M[i-1] = Y[i-1] - B[i-1] * M[i];
    }

    for(i = 0;i <= pLine->point_num - 2;i++)
    {
        pLine->a3[i] = M[i] / (6 * H[i]);
        pLine->a1[i] = (pLine->y[i] - M[i] * H[i] * H[i] / 6) / H[i];
        pLine->b3[i] = M[i+1] / (6 * H[i]);
        pLine->b1[i] = (pLine->y[i+1] - M[i+1] * H[i] * H[i] / 6) /H[i];
//        UARTprintf("a3[%d]=%f,a1[%d]=%f: b3[%d]=%f,b1[%d]=%f\n",i,pLine->a3[i],i,pLine->a1[i],i,pLine->b3[i],i,pLine->b1[i]);

    }

    return TRUE;
}

/***********************************************************
Function: Cubic_main.
Input: value and type
Output: Si for Cubic spline calculation results
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
float Cubic_main(float value,unsigned char type)
{
	int number = 0, i = 0;
	float Si = 0;
	pLine1 = &line1;

	switch (type){
		case Temp_Res:
			if (sizeof(Temp)/sizeof(Temp[0]) != sizeof(Temp_R)/sizeof(Temp_R[0]))
					UARTprintf("input data ERROR!\n");
			else{
					number = sizeof(Temp_R)/sizeof(Temp_R[0]);
					line1.point_num = number;
			}

			for (i=0;i<number;i++){
					line1.x[i] = Temp_R[i];
					line1.y[i] = Temp[i];
			}
			break;
		case Hydrogen_Res:
			if (sizeof(Temp)/sizeof(Temp[0]) != sizeof(Hydrogen_R)/sizeof(Hydrogen_R[0]))
					UARTprintf("input data ERROR!\n");
			else{
					number = sizeof(Hydrogen_R)/sizeof(Hydrogen_R[0]);
					line1.point_num = number;
			}

			for (i=0;i<number;i++){
					line1.x[i] = Hydrogen_R[i];
					line1.y[i] = Temp[i];
			}
			break;
		default: break;
	}
		
	line1.begin_k1 = ((line1.y[1]-line1.y[0])/(line1.x[1]-line1.x[0]));
	line1.end_k1 = ((line1.y[number-1]-line1.y[number-2])/(line1.x[number-1]-line1.x[number-2]));

	Spline3(pLine1);

	if (value>=line1.x[0] && value<=line1.x[number-1]){
	    for (i=0;i<number-1;i++){
	        if ((value>=line1.x[i] && value< line1.x[i+1]) || (value>line1.x[i] && value<=line1.x[i+1]))
	        {
	            Si = pLine1->a3[i]*pow((line1.x[i+1]-value),3) + pLine1->a1[i]*(line1.x[i+1]-value) + pLine1->b3[i]*pow((value-line1.x[i]),3) + pLine1->b1[i]*(value-line1.x[i]);
	        }
	    }
	}else UARTprintf("value outside of temp. ");

	return Si;
}

/***********************************************************
Function: Linear_slope.
Input: *slope x y type
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void Linear_slope(float *slope, float *x, float *y, unsigned char type)
{
	unsigned char number = 0, i = 0;
	
	switch (type){
		case Temp_Res:
			if (sizeof(Temp)/sizeof(Temp[0]) != sizeof(Temp_R)/sizeof(Temp_R[0]))
				UARTprintf("input data ERROR!\n");
			else{
				number = sizeof(Temp_R)/sizeof(Temp_R[0]);
			}
			for (i=0;i<number-1;i++){
				*slope += (Temp_R[i+1] - Temp_R[i])/(Temp[i+1] - Temp[i]);
			}
			*slope = *slope/(number-1);
			*x = Temp[0];
			*y = Temp_R[0];
			break;

		case DAC_temp:
			if (sizeof(DAC_Din)/sizeof(DAC_Din[0]) != sizeof(Din_temp)/sizeof(Din_temp[0]))
				UARTprintf("input data ERROR!\n");
			else{
				number = sizeof(Din_temp)/sizeof(Din_temp[0]);
			}
			for (i=0;i<number-1;i++){
				*slope += (Din_temp[i+1] - Din_temp[i])/(DAC_Din[i+1] - DAC_Din[i]);
			}
			*slope = *slope/(number-1);
			*x = DAC_Din[0];
			*y = Din_temp[0];
			break;
			
		case PCB_TEMP:
			if (sizeof(PCB_TEMP_Din)/sizeof(PCB_TEMP_Din[0]) != sizeof(PCB_TEMP_SET)/sizeof(PCB_TEMP_SET[0]))
				UARTprintf("input data ERROR!\n");
			else{
				number = sizeof(PCB_TEMP_SET)/sizeof(PCB_TEMP_SET[0]);
			}
			for (i=0;i<number-1;i++){
				*slope += (PCB_TEMP_SET[i+1] - PCB_TEMP_SET[i])/(PCB_TEMP_Din[i+1] - PCB_TEMP_Din[i]);
			}
			*slope = *slope/(number-1);
			*x = PCB_TEMP_Din[0];
			*y = PCB_TEMP_SET[0];
			break;

			default: break;
	}
}

