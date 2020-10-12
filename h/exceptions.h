#ifndef exceptions 
#define exceptions

/************************ EXCEPTIONS.H ***************
 * 
 * The externals declaration file for exceptions.c
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

#include "../h/types.h"

extern void programTrap();
extern void TLBExceptHandler();
void passUpOrDie();
void SYSCALL();
/*To do: add sys1-8*/


#endif
