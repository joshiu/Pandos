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
    pcb_t *newProc;
    unsigned int waitState;
    
    /* */
    if(readyQ != NULL){
        newProc = removeProcQ(readyQ);

        timeSlice = 500; /*make this a constant*/
        STCK(startTime); /*ecord new timeslice as starttime*/
        setTIMER(timeSlice); /*set the quantum*/

        loadState(newProc); /*not really sure why this is angry*/
    }

    /*if we have no processes then we are done!*/
    if(processCnt == 0){
        HALT(); /* we done! */
    }

    /* */
    if(processCnt > 0 && softBlockCnt > 0){
        currentProc = NULL;
        setTIMER(MAXINT); /*set timer to infinity*/

        waitState = 0x00000000 | 0x00000001|0x0000FF00|0x08000000; 
        /*turn on current interrupt bit, masking off, and te bit on*/ 

        setSTATUS(waitState);
        WAIT();
    }
    
    /**/
    if(processCnt > 0 && softBlockCnt == 0){
        PANIC();
    }

    return;
}

void setSpecificQuantum(pcb_t *process, cpu_t specificTime){
    STCK(startTime);
    setTIMER(specificTime); /*set the quantum*/
    loadState(process); /*not really sure why this is angry*/
}