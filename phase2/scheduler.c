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
 * Method comment here
 * */
void scheduleNext(){
    return;
}
/**
 * This method copies the processor state that is being pointed at and moves it to it’s wanted destination
 * */
void moveState(/*need something here*/){
    return;
}

/**
 * This function takes a process 
 * from the ready queue and switches it to the current running process.
 * If the queue is empty, a deadlock is executed.
 * */
void switchProcess(/*need something here*/){

}

/**
 * timeslice of 5 miliseconds
 *  
 * void scheduler(){
 * if(readyQ != NULL){
 * pcb_PTR newProc = removeProcQ(readyQ);
 * insertProcQ(&currentProc, newProc);
 * load 5 miliseconds onto processor local timer;
 * LDST(newProc->p_s);
 * }
 * if(proccnt == 0){
 * HALT(); //good job, we done
 * }
 * if(proccnt>0 && softblockcnt>0){
 * set status to enable interrupts;
 * set PLT = MAXINT;
 * WAIT();
 * }
 * if(proccnt >0 && softblockcnt ==0){
 * PANIC();
 * }
 * }
 * */