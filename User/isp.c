#include <LPC21xx.H>                    /* LPC21xx definitions               */
#include <string.h>
#include "Peripherals_LPC2194.h"
#include "isp.h"
#include "e25LC512.h"
#include "parameter.h"

#undef  MEMMAP
#define MEMMAP (*((volatile unsigned int *) 0xE01FC040))
#undef  IODIR0
#define IODIR0 (*((volatile unsigned int *) 0xE0028008))
#undef  IOCLR0
#define IOCLR0 (*((volatile unsigned int *) 0xE002800C))
#undef  PINSEL0
#define PINSEL0 (*((volatile unsigned int *) 0xE002C000))
#undef  VPBDIV
#define VPBDIV (*((volatile unsigned int *) 0xE01FC100))
#undef  PLLCON
#define PLLCON (*((volatile unsigned int *) 0xE01FC080))
#undef  PLLFEED
#define PLLFEED (*((volatile unsigned int *) 0xE01FC08C))
#undef  VICINTENCLR
#define VICINTENCLR (*((volatile unsigned int *) 0xfffff014))
#undef  TIMER1_PR
#define TIMER1_PR (*((volatile unsigned int *) 0xE000800C))
#undef  TIMER1_MCR
#define TIMER1_MCR (*((volatile unsigned int *) 0xE0008014))
#undef  TIMER1_CCR
#define TIMER1_CCR (*((volatile unsigned int *) 0xE0008028))

void (*bootloader_entry)(void);

void ISPbyCode_init(void)
{
	unsigned long ISPtemp;
	ISPtemp = PINSEL0;
	PINSEL0 = ISPtemp & 0xFFFFFFF3;
	ISPtemp = IODIR0;
	ISPtemp = ISPtemp | 0x4000;
	ISPtemp = ISPtemp & 0xFFFFFFFD;
	IODIR0 = ISPtemp;
	IOCLR0 = 0x4000;
	VICIntEnClr = (1<<16)|(1<<17)|(1<<9); 
	VICIntEnClr = 0x00000040;         
	PLLCON =  0x0;
	PLLFEED = 0xAA;
	PLLFEED=  0x55;
	VPBDIV = 0x0;
	TIMER1_PR=0x0;
	TIMER1_MCR=0x0;
	TIMER1_CCR=0x0;
	MEMMAP = 0x0;
	bootloader_entry = (void (*)(void))(0x0);
}

void ISPbyCode(void)
{
	unsigned int temp_tmp = 0xffffffff;
	unsigned char model_teype = 255;
	e2prom512_write(&model_teype,1,160*2);
	
	
	e2prom512_write((unsigned char*)&temp_tmp,4,(260+(0*2))*2);
	e2prom512_write((unsigned char*)&temp_tmp,4,(260+(2*2))*2);
	e2prom512_write((unsigned char*)&temp_tmp,4,(260+(4*2))*2);
	
	ISPbyCode_init();
	while(1)
	bootloader_entry();
}

