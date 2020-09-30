#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"

/**
 * This file conatins a round robin scheduler 
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G.
 * */

/**
 * timeslice of 5 miliseconds
 *  
 * public void scheduler(){
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