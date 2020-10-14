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
void loadState(pcb_t *process){

    currentProc = process;
    LDST(&(process->p_s));
}

/**
 * * This function takes a process from the ready queue 
 * and makes it the current running process.
 * If the queue is empty, a deadlock is executed.
 * */
void scheduleNext(){

    /* */
    if(readyQ != NULL){
        pcb_PTR newProc = removeProcQ(readyQ);
        insertProcQ(&currentProc, newProc);
        /* load timeslice (5miliseconds) onto processor local timer */
        LDST(newProc -> p_s); /*not really sure why this is angry*/
    }

    /*if we have no processes then we are done!*/
    if(processCnt == 0){
        HALT(); /* we done! */
    }

    /* */
    if(processCnt > 0 && softBlockCnt > 0){
        /*set status to enable interrupts*/
        /*set PLT = MAXINT */
        WAIT();
    }
    
    /**/
    if(processCnt > 0 && softBlockCnt == 0){
        PANIC();
    }

    return;
}