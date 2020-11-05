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
 * This includes the creation and initialization of processCnt, 
 * softBlockCnt, readyQ, currentproc, devicesema4s (array of integers). 
 * Populates the pass up vector which is part of the BIOS Data Page, 
 * initialization data structures and variables, loads the timer, 
 * and instantiates a single process. It also has a case statement that
 * looks at the cause registers.
 * 
 * Also contains a general exception handler that accesses the cause number
 * to sort exceptions into syscall exceptions, interrupts, program traps, 
 * and TLB exceptions.
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G.
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
cpu_t timeSlice;                            /*amount of time left in the time slice*/

/* END GLOBAL VARIABLES*/


/**
 * This method is only executed once. It performs the Nucleus initialization
 * to set up the system.
 * 
 * Note: devSema4[DEVCNT+DEVPERINT] will be used as our clock sema4
 * */
int main()
{
    /*local variables*/
    passupvector_t *passUpVec;
    int counter;
    pcb_t *newPcb;
    memaddr topRamAdd;
    /*end of local variables*/

    initPcbs(); /*initalize all pcbs and set up free list*/
    initASL(); /*create sema4 free list and ASL*/

    passUpVec = (passupvector_t *)PASSUPVECTOR; /*pointer that points to the PASSUPVECTOR*/

    passUpVec->tlb_refll_handler = (memaddr)uTLB_RefillHandler; /*set refill handler*/
    passUpVec->tlb_refll_stackPtr = STKPTR;
    passUpVec->exception_handler = (memaddr)generalExceptHandler; /*set exception handler*/
    passUpVec->exception_stackPtr = STKPTR;

    processCnt = 0;   /*Initialize process count*/
    softBlockCnt = 0; /*Initialize soft block count*/
    readyQ = mkEmptyProcQ();/*create queue*/
    currentProc = NULL; /*set current process*/

   /*set clock sema4 to 0*/
    devSema4[DEVCNT + DEVPERINT] = 0;
    
    /*Initializing the deviceSema4 list and setting everything in it to 0*/
    for (counter = 0; counter < (DEVCNT + DEVPERINT); counter+=1)
    {
        devSema4[counter] = 0;
    }

    LDIT(STANPSEUDOCLOCK); /*load interval timer with 100000 ms*/

    /*set the top of RAM address*/
    RAMTOP(topRamAdd);

    newPcb = allocPcb();/*create a new process*/

    /*If nothing allocated, we have an issue*/
    if (newPcb == NULL)
    {
        PANIC();
    }

    /*if something allocated, put it on top of the RAM*/
    else
    {
        newPcb->p_s.s_pc = (memaddr)test;
        newPcb->p_s.s_t9 = (memaddr)test;

        newPcb->p_s.s_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        newPcb->p_s.s_sp = topRamAdd;

        processCnt +=1;/*new process ready to go!*/

        insertProcQ(&readyQ, newPcb); /*insert into readyQ*/
        
        scheduleNext(); /*schedule next process*/

    }

    return (0); /*we're done with main*/

} /*end of main*/


/**
 * This method determines the case statement, whether it's an 
 * interrupt, syscall, TLB exception, or program trap and calls
 * the appropriate handler.
 * */
void generalExceptHandler()
{
    /*local variables*/
    state_t *programState;
    int causeNum;
    /*end of local variables*/

    programState = (state_t *)BIOSDATAPAGE; /*check state in BIOSDATAPAGE*/

    causeNum = (int)((programState->s_cause & GETCAUSE) >> 2); /*get cause number*/
    
    if (causeNum == GOTOSYSCALL)
    { 
        /*if cause is 8, go to syscall*/
        syscall();
    }

    else if (causeNum == GOTOINTERRUPTS)
    {
        /*if cause is 0, go to interrupts*/
        interruptHandler();
    }

    else if (causeNum <= PGFAULTCAUSEMAX && causeNum > PGFAULTCAUSEMIN)
    {
        /*if cause between 0 and 3, go to TLBexception*/
        TLBExceptHandler();
    }

    else{
         /*if any other cause number*/
         programTrap();

    }

}/*end generalExceptHandler*/