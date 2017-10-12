/******************************************************************************/
/* MEASURE.H: struct type and extern definitions for MEASURE project          */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

struct clock {                           /* structure of the clock record     */
  unsigned char    hour;                 /* hour                              */
  unsigned char     min;                 /* minute                            */
  unsigned char     sec;                 /* second                            */
  unsigned short   msec;                 /* milliseconds                      */
};

struct mrec  {                           /* structure for measurement records */
  struct   clock   time;                 /* time of measurement               */
  unsigned long   port0;                 /* state of port 0                   */
  unsigned char  analog [4];             /* voltage on analog Pins AD0 .. AD3 */
};

struct interval  {                       /* structure for interval record     */
  unsigned char     min;                 /* minute                            */
  unsigned char     sec;                 /* second                            */
  unsigned short   msec;                 /* milli second                      */
};

