#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/exceptions.h"
#include "../h/interrupts.h"
#include "../h/scheduler.h"

/**
 * This file is the entry point and performs Nucleus initialization. 
 * This entry point (main) is only executed once. 
 * This includes the process count, soft-block count, ready queue, current process, device sema4s (array of integers). 
 * Populates the pass up vector which is part of the BIOS Data Page, initialization data structures 
 * and variables, loads the timer, and instantiates a single process. 
 * It also has a case statement that looks at the cause registers.
 * 
 * Written by Umang Joshi and Amy Kelley
 * */


/************ FILE SPECIFIC METHODS *********************/

extern void uTLB_RefillHandler();   /*Address of the TLB Refill Event Handler*/
extern void test();                 /*sets up the nucleus calls that are to be tested*/
HIDDEN void generalExceptHandler(); /* method for handling exceptions*/

/*end of file specific methods*/

/** GLOBAL VARIABLES **/

unsigned int saveState[DEVCNT + DEVPERINT]; /* this is where we save the state when IO*/
int processCnt;                             /*number of pcbs that have been allocated*/
int softBlockCnt;                           /*number of processes that have been blocked*/
pcb_t *readyQ;                              /*queue of processes ready to run*/
pcb_t *currentProc;                         /*process that is currently running*/
int devSema4[DEVCNT + DEVPERINT + 1];       /*array of device semaphores*/
cpu_t startTime;                            /*beginning of a time unit*/
cpu_t timeSlice;                            /*amount of time until the time slice*/

/* END GLOBAL VARIABLES*/

/*--------------------------Debug Method-------------------------------------------------------------------*/
void debug(int a)
{
    return;
}

/**
 * This method is only executed once. It performs the Nucleus initialization to set up the system.
 * */
int main()
{
    debug(10);

    /*local variables*/
    passupvector_t *passUpVec;
    int counter;
    pcb_t *newPcb;
    memaddr topRamAdd;
    /*end of local variables*/

    passUpVec = (passupvector_t *)PASSUPVECTOR; /*a pointer that points to the PASSUPVECTOR*/

    passUpVec->tlb_refll_handler = (memaddr)uTLB_RefillHandler;
    passUpVec->tlb_refll_stackPtr = STKPTR;
    passUpVec->exception_handler = (memaddr)generalExceptHandler; /*have not created this yet*/
    passUpVec->exception_stackPtr = STKPTR;

    initPcbs();
    initASL();

    processCnt = 0;   /*Initialize process count*/
    softBlockCnt = 0; /*Initialize soft block count*/
    readyQ = mkEmptyProcQ();
    currentProc = NULL;

    /*loop that initializes all devicesema4s to 0 (idk if this needed)*/
    devSema4[DEVCNT + DEVPERINT] = 0;
    
    /*Initializing the deviceSema4 list and setting everything in it to 0*/
    for (counter = 0; counter < (DEVCNT + DEVPERINT); counter++)
    {
        devSema4[counter] = 0;
    }

    LDIT(STANPSEUDOCLOCK); /*load interval timer with 1000 ms*/

    /*Need to get top of RAM address*/
    RAMTOP(topRamAdd);
    newPcb = allocPcb();

    /*If the PCB is null, there is an issue*/
    if (newPcb == NULL)
    {
        PANIC();
    }

    /*else we put on the PCB on top of the RAM*/
    else
    {
        newPcb->p_s.s_pc = (memaddr)test;
        newPcb->p_s.s_t9 = (memaddr)test;
        newPcb->p_s.s_status = (ALLOFF | 0x00000004 | 0x0000FF00 | 0x08000000);
        newPcb->p_s.s_sp = topRamAdd;
        /* figure out how to turn on timer, interrupts, and keep kernel mode*/
        /* here, we turn all bits to 0, then turn on the previous interrupt enable it
        then turn on interrupt mask and timer enable bit*/
        /*interrupt mask needs to be turned off(by changing to 1), so when we enable interrupts
         we need to disable the mask when we enable interrupts*/

        processCnt += 1;
        /*set p_time and p_supportStruct in pcb.c*/

        insertProcQ(&readyQ, newPcb);
        debug(1);
        scheduleNext();
    }

    return (0);

} /*end of main*/


/**
 * This method determines the case statement, whether it's an interrupt or syscall and calls the appropriate handler.
 * */
void generalExceptHandler()
{

    /*local variables*/
    state_t *programState;
    int causeNum;
    /*end of local variables*/

    programState = (state_t *)BIOSDATAPAGE;
    causeNum = (int)((programState->s_cause & 0x0000007C) >> 2); /*shift wrong?*/
    debug(causeNum);
    
    /*we need to look at cause reg, then turn off all but bits 2-6 (from the back), then shift right 2*/
    if (causeNum == 8)
    {
        debug(100);
        syscall();
    }

    else if (causeNum == 0)
    {
        debug(200);
        interruptHandler();
    }

    else if (causeNum <= 3 && causeNum > 0)
    {
        debug(321);
        TLBExceptHandler();
    }

    else{
         /*if all else fails*/
         debug(66669);
         programTrap();
    }

}