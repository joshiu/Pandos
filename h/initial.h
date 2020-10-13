#ifndef initial
#define initial

/************************** INITIAL.H ******************************
*
*  The externals declaration file for Initial.c or the Nucleus of 
*   the Pandos Project
*
*  Written by Umang J and Amy K
*/

#include "../h/types.h"

extern int main();
extern memaddr getRAMTOP();

/* GLOBAL VARIABLES*/
extern pcb_t *currentProc;
extern int processCnt; 
extern int softBlockCnt; 
extern pcb_t *readyQ;
extern int devSema4[DEVCNT+DEVPERINT+1];
extern cpu_t startTime;
extern cpu_t timeSlice;

#endif
