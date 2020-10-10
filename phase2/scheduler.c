#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
/*gotta include the .h files for initial-> maybe more*/

/**
 * This file contains a round robin scheduler 
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G.
 * */

/** I added methods I think we need in google doc so look at that!*/
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