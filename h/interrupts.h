#ifndef interrupts
#define interrupts


/************************ EXCEPTIONS.H ***************
 * 
 * The externals declaration file for interrupts.c
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

#include "../h/types.h"

extern void interruptHandler();
extern void localInterruptHandler();
extern void pseudoClockInterrupt();
extern void deviceInterrupt();

/*To do: add functions here after you do more of .c file*/


#endif