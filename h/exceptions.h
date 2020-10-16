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
extern void passUpOrDie();
extern void SYSCALL();
extern int SYS1();
extern void SYS2();
extern void SYS3();
extern void SYS4();
extern int SYS5();
extern cpu_t SYS6();
extern void SYS7();
extern support_t SYS8();
extern void copyState();
extern cpu_t timeCalc();

#endif
