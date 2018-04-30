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
#define  MAXNUM  15   

typedef struct SPLINE    
{
    float x[MAXNUM+1];    
    float y[MAXNUM+1];    
    unsigned int point_num;
    double begin_k1;
    double end_k1;

    double k1[MAXNUM+1];
    double k2[MAXNUM+1];

    double a3[MAXNUM],a1[MAXNUM];
    double b3[MAXNUM],b1[MAXNUM];
}SPLINE;

SPLINE line1;
SPLINE *pLine1 = &line1;

/***********************************************************
Function: Spline3.
Input:	none
Output: none
Author: zhuobin
Date: 2017/10/10
Description: .
***********************************************************/
void Spline3(void)
{
    double H[MAXNUM] = {0};     //??????
    double Fi[MAXNUM] = {0};     //???
    double U[MAXNUM+1] = {0};    //???
    float A[MAXNUM+1] = {0};    //???
    double D[MAXNUM+1] = {0};    //???
    double M[MAXNUM+1] = {0};    //M??
    float B[MAXNUM+1] = {0};    //??????
    float Y[MAXNUM+1] = {0};    //???????
    int i = 0;

    UARTprintf("pLine1->point_num = %d\n", pLine1->point_num);
    if((pLine1->point_num < 3) || (pLine1->point_num > MAXNUM + 1))
    {
        UARTprintf("point num error\n");
    }

    for(i = 0;i <= pLine1->point_num - 2;i++)
    {
        H[i] = pLine1->x[i+1] - pLine1->x[i];
        Fi[i] = (pLine1->y[i+1] - pLine1->y[i]) / H[i];
    }

    for(i = 1;i <= pLine1->point_num - 2;i++)
    {
        U[i] = H[i-1] / (H[i-1] + H[i]);
        A[i] = H[i] / (H[i-1] + H[i]);
        D[i] = 6 * (Fi[i] - Fi[i-1]) / (H[i-1] + H[i]);
    }

    U[i] = 1;
    A[0] = 1;
    D[0] = 6 * (Fi[0] - pLine1->begin_k1) / H[0];
    D[i] = 6 * (pLine1->end_k1 - Fi[i-1]) / H[i-1];

    //U[i] = 0;
    //A[0] = 0;
    //D[0] = 2 * begin_k2;
    //D[i] = 2 * end_k2;

    B[0] = A[0] / 2;
    for(i = 1;i <= pLine1->point_num - 2;i++)
    {
        B[i] = A[i] / (2 - U[i] * B[i-1]);
    }

    Y[0] = D[0] / 2;
    for(i = 1;i <= pLine1->point_num - 1;i++)
    {
        Y[i] = (D[i] - U[i] * Y[i-1]) / (2 - U[i] * B[i-1]);
    }

    M[pLine1->point_num - 1] = Y[pLine1->point_num - 1];
    for(i = pLine1->point_num - 1;i > 0;i--)
    {
        M[i-1] = Y[i-1] - B[i-1] * M[i];
    }

    for(i=0;i<=pLine1->point_num-2;i++)
    {
        pLine1->a3[i] = M[i] / (6 * H[i]);
        pLine1->a1[i] = (pLine1->y[i] - M[i] * H[i] * H[i] / 6) / H[i];
        pLine1->b3[i] = M[i+1] / (6 * H[i]);
        pLine1->b1[i] = (pLine1->y[i+1] - M[i+1] * H[i] * H[i] / 6) /H[i];
//        UARTprintf("a3[%d]=%f,a1[%d]=%f: b3[%d]=%f,b1[%d]=%f\n",i,pLine1->a3[i],i,pLine1->a1[i],i,pLine1->b3[i],i,pLine1->b1[i]);
    }
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
	unsigned int number = 0, i = 0;
	float Si = 0;

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
		case Hydrogen_Res_70:
			if (sizeof(Intermediate_Data.H2_70)/sizeof(Intermediate_Data.H2_70[0]) != sizeof(Intermediate_Data.H2_R_70)/sizeof(Intermediate_Data.H2_R_70[0]))
					UARTprintf("input data ERROR!\n");
			else{
					number = sizeof(Intermediate_Data.H2_R_70)/sizeof(Intermediate_Data.H2_R_70[0]);
					line1.point_num = number;
				  UARTprintf("number of H2_R_70 array =%d\n", line1.point_num);
			}

			for (i=0;i<number;i++){
					line1.y[i] = Intermediate_Data.H2_R_70[i];
					line1.x[i] = Intermediate_Data.H2_70[i];
				  UARTprintf("line1.x[%d] = %.3f, line1.y[%d] = %.3f\n",i,line1.x[i],i,line1.y[i]);
			}
			break;
		default: break;
	}
		
	line1.begin_k1 = ((line1.y[1]-line1.y[0])/(line1.x[1]-line1.x[0]))*0.9;
	line1.end_k1 = ((line1.y[number-1]-line1.y[number-2])/(line1.x[number-1]-line1.x[number-2]))*1.1;

  UARTprintf("1->pLine1->point_num = %d\n", pLine1->point_num);
	Spline3();

	if (value>=line1.x[0] && value<=line1.x[number-1]){
	    for (i=0;i<number-1;i++){
	        if ((value>=line1.x[i] && value< line1.x[i+1]) || (value>line1.x[i] && value<=line1.x[i+1]))
	        {
	            Si = pLine1->a3[i]*pow((line1.x[i+1]-value),3) + pLine1->a1[i]*(line1.x[i+1]-value) + pLine1->b3[i]*pow((value-line1.x[i]),3) + pLine1->b1[i]*(value-line1.x[i]);
	        }
	    }
	}
  return Si;
}
