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
extern int sys_1();
extern void sys_2(pcb_t *runningProc);
extern void sys_3();
extern void sys_4();
extern int sys_5();
extern cpu_t sys_6();
extern void sys_7();
extern support_t sys_8();
extern void copyState(state_t *source, state_t *copy);
extern cpu_t timeCalc(cpu_t time);

#endif
