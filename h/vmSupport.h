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

#endif