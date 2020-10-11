#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"
/**
 * #include "../h/exceptions.h"
 * #include "../h/interrupts.h"
 * #include "../h/scheduler.h"
 * */

/**
 * insert file comment here
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

extern void uTLB_RefillHandler(); /*Address of the TLB Refill Event Handler*/
extern void test(); /*sets up the nucleus calls that are to be tested*/

HIDDEN void generalExceptHandler(); /* method for handling exceptions*/

extern memaddr getRAMTOP(); /*method to get the top of RAM THIS GOES IN .H file*/
/* GLOBAL VARIABLES */

unsigned int saveState[DEVCNT+DEVPERINT]; /* this is where we save the state when IO*/
int processCnt; /*number of pcbs that have been allocated*/
int softBlockCnt; /*number of processes that have been blocked*/
pcb_t *readyQ; /*queue of processes ready to run*/
pcb_t *currentProc; /*process that is currently running*/
int devSema4[DEVCNT+DEVPERINT+1]; /*array of device semaphores*/
cpu_t startTimeOD; /*beginning of a time unit*/
cpu_t timeSlice; /*amount of time until the time slice*/
#define clockSem devSema4[DEVCNT+DEVPERINT]; /*clock sema4*/

/* END GLOBAL VARIABLES*/

/*--------------------------------------------------------------------------------------------------*/

int main(){
    passupvector_t *passUpVec;
    int counter;
    pcb_t *newPcb;
    memaddr topRAM;

    passUpVec = (passupvector_t *) PASSUPVECTOR; /*a pointer that points to the PASSUPVECTOR*/

    passUpVec-> tlb_refll_handler = (memaddr) uTLB_RefillHandler;
    passUpVec -> tlb_refll_stackPtr = 0x20001000;
    passUpVec-> exception_handler = (memaddr) generalExceptHandler; /*have not created this yet*/
    passUpVec -> exception_stackPtr = 0x200010000;

    initPcbs();
    initASL();

    processCnt = 0; /*Initialize process count*/
    softBlockCnt= 0; /*Initialize soft block count*/
    readyQ = mkEmptyProcQ();
    currentProc = NULL;

    /*loop that initializes all devicesema4s to 0 (idk if this needed)*/
    clockSem = 0;
    for(counter = 0; counter < (DEVCNT+DEVPERINT); counter ++){
        devSema4[counter] = 0;
    }

    LDIT(100); /*load interval timer with 100 ms*/
    /*Need to get top of RAM address*/
    topRAM = getRAMTOP(topRAM);
    newPcb = allocPcb();
    if(newPcb!=NULL){
        newPcb->p_s.s_pc = (memaddr) test;
        newPcb -> p_s.s_t9 = (memaddr) test;
        newPcb->p_s.s_status = ; /* figure out how to turn on timer, interrupts, and keep kernel mode*/
        newPcb->p_time = 0;
        newPcb->p_supportStruct = NULL;
        processCnt += 1;
        insertProcQ(&readyQ, newPcb);

        scheduler();
    }
    else{
        PANIC();
    }
    return (0);
}

void generalExceptHandler(){

}

/**
 * Insert method comment here
 * */
extern void uTLB_RefillHandler(){
    /* calls approiate handlers this includes the program traps
    for example: if it's sysexception then call sys Trap
    so get cause and branch to the correct one needed */
}
memaddr getRAMTOP(memaddr t){
    t = ((* ((int *) RAMBASEADDR))+ (* ((int *) RAMBASESIZE)));
    return t;
}
