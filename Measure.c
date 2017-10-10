/******************************************************************************/
/* MEASURE.C: Remote Measurement Recorder                                     */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>                       /* standard I/O .h-file              */
#include <ctype.h>                       /* character functions               */

#include <LPC21xx.H>                   /* LPC21xx definitions               */


#include "measure.h"                     /* global project definition file    */

const char menu[] =
   "\n"
   "+***************** REMOTE MEASUREMENT RECORDER *****************+\n"
   "| This program is a simple Measurement Recorder. It is based on |\n"
   "| the LPC2129 and records the state of Port 0 and the voltage   |\n"
   "| on the four analog inputs AIN0 trough AIN3.                   |\n"
   "+ command -+ syntax -----+ function ----------------------------+\n"
   "| TEST_SENSE     | A       | Display Sense Resistance|\n"
   "| Read     | R [n]       | read <n> recorded measurements       |\n"
   "| Display  | D           | display current measurement values   |\n"
   "| Time     | T hh:mm:ss  | set time                             |\n"
   "| Interval | I mm:ss.ttt | set interval time                    |\n"
   "| Clear    | C           | clear measurement records            |\n"
   "| Quit     | Q           | quit measurement recording           |\n"
   "| Start    | S           | start measurement recording          |\n"
   "+----------+-------------+--------------------------------------+\n";

struct interval setinterval;                /* interval setting values        */
struct interval interval;                   /* interval counter               */

volatile int measurement_interval = 0;      /* measurement interval over      */
volatile int mdisplay = 0;                  /* measurement display requested  */
volatile int startflag = 0;                 /* start measurement recording    */

struct mrec current;                        /* current measurements           */

#define SCNT  6                             /* number of records              */

struct mrec save_record[SCNT];              /* buffer for measurements        */
int sindex;                                 /* save index                     */
int savefirst;                              /* save first index               */

char ERROR [] = "\n*** ERROR: %s\n";        /* ERROR message string in code   */

#define WRONGINDEX 0xffff                   /* error signal for wrong index   */


/******************************************************************************/
/*               Save current measurements in save_record                     */
/******************************************************************************/
static void save_current_measurements (void) {
  save_record[sindex++] = current;             /* copy current measurements   */
  if (sindex == SCNT) sindex = 0;              /* check bounds of sindex      */
  if (sindex == savefirst)  {                  /* check circular buffer limits*/
    if (++savefirst == SCNT)  savefirst = 0;   /* check bounds of savefirst   */
  }
}


/* Default Interrupt Function: may be called when timer ISR is disabled */
void DefISR (void) __irq  {
	printf("This is Test 1\n");
}

/******************************************************************************/
/*                Timer Counter 0 interrupt service function                  */
/*                executes each 1ms                                           */
/******************************************************************************/
__irq void TC0_IR (void) {
  unsigned int val;
  unsigned int crval;
  int i;
//printf("This is Test 2\n");
  if (measurement_interval)  {                 /* measurement done ?          */
    save_current_measurements ();              /* yes -> save measurements    */
    measurement_interval = 0;                  /* Save measurements done      */
  }
                                               /* check if interval is over   */
  if (interval.min  == 0 &&
      interval.sec  == 0 &&
      interval.msec == 0     )  {
    interval = setinterval;                    /* set interval time again     */
    measurement_interval = startflag;          /* determine measurement flag  */
  }
  else  {                                      /* update interval time        */
    if (interval.msec-- == 0)  {               /* calculate millisecond       */
      interval.msec = 999;
      if (interval.sec-- == 0)  {              /* calculate second            */
        interval.sec = 59;
        interval.min--;                        /* calculate minute            */
      }
    }
  }
                                               /* update current time         */
  if (++current.time.msec == 1000)  {          /* update millisecond cnt      */
    current.time.msec = 0;

    if (++current.time.sec == 60)  {           /* update second counter       */
      current.time.sec = 0;

      if (++current.time.min == 60)  {         /* update minute counter       */
        current.time.min = 0;

        if (++current.time.hour == 24)  {      /* update hour counter         */
          current.time.hour = 0;
        }
      }
    }
  }	/* end of if( ++current.time.msec... */

  if (measurement_interval || mdisplay)  {     /* process measurement         */
    current.port0 = IOPIN0;                    /* read port 0                 */
    for (i = 0; i != 4; i++) {                 /* loop for 4 A/D inputs       */
      crval = 0x01000000 | (1<<i);
      ADCR |= crval;                           /* Start A/D Conversion        */
      do {
        val = ADDR;                            /* Read A/D Data Register      */
      } while ((val & 0x80000000) == 0);       /* Wait for end of A/D Convers.*/
      ADCR &= ~crval;                          /* Stop A/D Conversion         */
      val = (val >> 8) & 0xFF;                 /* Extract AIN Value           */
      current.analog[i] = val;                 /* result of A/D process       */ 
    }
    mdisplay = 0;                              /* mdisplay = 0 for ready sig. */
  }

  T0IR = 1;                                    /* Clear interrupt flag        */
  VICVectAddr = 0;                             /* Acknowledge Interrupt       */
}

/******************************************************************************/
/***************************      MAIN PROGRAM      ***************************/
/******************************************************************************/
int main (void)  
{                             /* main entry for program      */
	char cmdbuf [15];                            /* command input buffer        */
	int i;                                       /* index for command buffer    */
	int idx;                                     /* index for circular buffer   */
	unsigned char temperature = 0;

	/* setup the timer counter 0 interrupt */
	T0MR0 = 14999*1000*2;                               /* 1mSec = 15.000-1 counts     */
	T0MCR = 3;                                   /* Interrupt and Reset on MR0  */
	T0TCR = 1;                                   /* Timer0 Enable               */
	VICVectAddr0 = (unsigned long)TC0_IR;           /* set interrupt vector in 0   */
	VICVectCntl0 = 0x20 | 4;                     /* use it for Timer 0 Interrupt*/
	VICIntEnable = 0x00000010;                   /* Enable Timer0 Interrupt     */
	VICDefVectAddr = (unsigned long) DefISR;     /* un-assigned VIC interrupts  */

	printf ( menu );                             /* display command menu        */
	while (1)  
	{                                 /* loop forever                */
		printf ("\nCommand: ");
		cmdbuf[0] = 'A';
		
//		getline (&cmdbuf[0], sizeof (cmdbuf));     /* input command line          */
		printf("cmdbuf[0]=%c\n",cmdbuf[0]);

		for (i = 0; cmdbuf[i] != 0; i++)  {        /* convert to upper characters */
		cmdbuf[i] = toupper(cmdbuf[i]);
		}

		for (i = 0; cmdbuf[i] == ' '; i++);        /* skip blanks                 */

		printf("commond=%s\n",cmdbuf);
		

		switch (cmdbuf[i])  {                      /* proceed to command function */
			case 'A':
				printf ("\nDisplay current Measurements: (ESC to abort)\n");
				printf ("Want Temperature:");
				getline (cmdbuf, sizeof (cmdbuf));     

				if (strlen(cmdbuf)==2)
				temperature = (cmdbuf[0]-48)*10+(cmdbuf[1]-48);
				else if  (strlen(cmdbuf)==3)
				temperature = (cmdbuf[0]-48)*100+(cmdbuf[1]-48)*10+(cmdbuf[2]-48);
				else printf("Set temperature error.\n ");

				printf("Set temperature=%d OK\n",temperature);

				DAC8568_INIT_SET(temperature);
				do  {
					while (!(U1LSR & 0x01))  {
						DelayNS(10);
						Read_channel(1);
						Read_channel(2);
					}
				} while (getkey () != 0x1B);           /* escape terminates command   */
				printf ("\n\n");
				break;

			default:                                 /* Error Handling              */
				printf (ERROR, "UNKNOWN COMMAND");
				printf (menu);                         /* display command menu        */
				break;
		}
	}
}
