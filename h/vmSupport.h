#ifndef vmsupport
#define vmsupport

/************************** VMSUPPORT.H ******************************
*
*  The externals declaration file for vmsupport.c 
*
*  Written by Umang J and Amy K
*/

#include "../h/types.h"

extern void initTLBsupport();
extern void uTLB_RefillHandler();
extern void pageHandler();
extern swap_t swapPool[POOLSIZE]; /*declares the array for the swap pool */
extern int swapSem; /*declares swap pool sema4*/
#endif