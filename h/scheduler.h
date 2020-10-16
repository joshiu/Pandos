#ifndef scheduler
#define scheduler

/************************** SCHEDULER.H ******************************
*
*  The externals declaration file for scheduler.c, the round robin
*  scheduling system for Pandos
*
*  Written by Umang J and Amy K
*/

#include "../h/types.h"

extern void scheduleNext();
extern void loadState();
extern void setSpecificQuantum();

#endif
