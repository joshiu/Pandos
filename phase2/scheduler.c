#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/initial.h"
#include "../h/exceptions.h"
#include "../h/interrupts.h"

/**
 * In this file, the nucleus implements a simple preemptive round-robin 
 * scheduling algorithm with a set time slice value.
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G.
 * */

/**
 * This method loads a process and sets it equal to the currentProcess.
 * Then we load the state of the current process and let it run
 * */
void loadState(pcb_t *process)
{
    debug(404);
    currentProc = process;
    LDST(&(process->p_s));
}

/**
 * * This function takes a process from the ready queue 
 * and makes it the current running process.
 * If the queue is empty, a deadlock is executed.
 * */
void scheduleNext()
{

    /*local variables*/
    pcb_t *newProc;
    unsigned int waitState;
    /*end of local variables*/

    /* insert comment here*/
    if (readyQ != NULL)
    {
        debug(1000);
        newProc = removeProcQ(&readyQ);

        timeSlice = STANQUANTUM;
        STCK(startTime);     /*record new process' starttime*/
        setTIMER(timeSlice); /*set the quantum*/

        debug(1001);
        loadState(newProc);
    }

    /*if we have no processes then we are done!*/
    if (processCnt == 0)
    {
        debug(999);
        HALT(); /* we done! */
    }

    /*insert comment here*/
    if (processCnt > 0 && softBlockCnt > 0)
    {
        debug(110);
        currentProc = NULL;
        setTIMER(MAXINT); /*set timer to infinity*/

        waitState = (ALLOFF | 0x00000001 | 0x0000FF00 | 0x08000000);
        /*turn on current interrupt bit, masking off, and te bit on*/

        setSTATUS(waitState);
        debug(111);
        WAIT();
    }

    /*insert comment here*/
    if (processCnt > 0 && softBlockCnt == 0)
    {
        PANIC();
    }

    return;
}

/**
 * Insert method comment here
 * */
void setSpecificQuantum(pcb_t *process, cpu_t specificTime)
{

    STCK(startTime);
    setTIMER(specificTime); /*set the quantum*/
    loadState(process);
}