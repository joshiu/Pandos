#ifndef syssupport
#define syssupport

/************************** SYSSUPPORT.H ******************************
*
*  The externals declaration file for syssupport.c 
*
*  Written by Umang J and Amy K
*/

#include "../h/types.h"

extern void userGeneralExceptHandler();
extern void block(int *semAddr);
extern void unblock(int *semAddr);
extern void userLoadState(state_t *loadState);
extern void enableInterrupts(int turnON);
extern void killProc(int *semAddr);



#endif