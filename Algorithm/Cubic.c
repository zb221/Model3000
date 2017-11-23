/***********************************************
@		Description: This file is about Cubic.c data.
@		Copyright: Hydrogen Sense(Suzhou)  Technology Co.,Ltd. All rights reserved.
@		Author: zhuobin.
@		Date: 2017/9/22.
***********************************************/

#include <stdio.h>
#include <math.h>
#include "Cubic.h"
#include "Peripherals_LPC2194.h"
#include "parameter.h"

/***********************************************************
Description: Global variable region.
Author: zhuobin
Date: 2017/10/10
***********************************************************/
#define  MAXNUM  50   

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
		case Hydrogen_Res:
			if (sizeof(Intermediate_Data.H2)/sizeof(Intermediate_Data.H2[0]) != sizeof(Intermediate_Data.OHM)/sizeof(Intermediate_Data.OHM[0]))
					UARTprintf("input data ERROR!\n");
			else{
					number = sizeof(Intermediate_Data.OHM)/sizeof(Intermediate_Data.OHM[0]);
					line1.point_num = number;
			}

			for (i=0;i<number;i++){
					line1.x[i] = Intermediate_Data.OHM[i];
					line1.y[i] = Intermediate_Data.H2[i];
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
	}else UARTprintf("Range out of measurement. \n");

	return Si;
}
