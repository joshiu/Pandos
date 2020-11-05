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
extern void passUpOrDie(int exceptNum);
extern void syscall();
extern void copyState(state_t *source, state_t *copy);

#endif
