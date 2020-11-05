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
 * scheduling algorithm with a set time slice value. If the queue is empty,
 * this file halts, waits, or panics, depending on the process count and
 * soft block count. 
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G.
 * */



/**
 * This method loads the process and sets the currentProcess equal to it.
 * Then we load the state of the process and let it run.
 * */
void loadState(pcb_t *process)
{
    currentProc = process;

    LDST(&(process->p_s));

}


/**
 * This method takes a process from the ready queue and makes
 * it the current running process, assigning it a quantum and 
 * start time.
 * 
 * If the queue is empty, a deadlock, wait, or halt is executed,
 * depending on the process count and soft block count.
 * */
void scheduleNext()
{

    /*local variables*/
    pcb_t *newProc;
    unsigned int waitState;
    /*end of local variables*/

    /*if readyQ is not Null remove newProc from readyQ*/
    if (readyQ != NULL)
    {
        newProc = removeProcQ(&readyQ);/*remove process from queue*/

        STCK(startTime);     /*record new process' starttime*/
        setTIMER(STANQUANTUM); /*set the quantum*/
        
        loadState(newProc);
    }

    /*if we have no processes then we are done!*/
    if (processCnt == 0)
    {
        HALT(); /* we done! */
    }

    /*wait if process is more than 0 and soft block is more than zero*/
    if (processCnt > 0 && softBlockCnt > 0)
    {
        currentProc = NULL;
        setTIMER(MAXINT); /*set timer to literal infinity*/
    
        /*turn on current interrupt bit, masking, and timer enable bit*/
        waitState = (ALLOFF | IECURRENTON | IMASKON | TIMEREBITON);

        setSTATUS(waitState);

        WAIT();
    }

    /*something is wrong if softBlock is 0*/
    if (processCnt > 0 && softBlockCnt == 0)
    {
        PANIC();
    }

    return;
    
}/*end scheduler*/

/**
 * This method sets the quantum of the current to 
 * a specific time, then loads that process.
 * */
void setSpecificQuantum(cpu_t specificTime)
{
    STCK(startTime); /*record new start time*/
    setTIMER(specificTime); /*set the quantum*/

    loadState(currentProc);
}