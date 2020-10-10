#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
/* need to include inital.h and scheduler.h*/

/**
 * Main comment here
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/**
 * void interruptHandler(){
 * who interrupted me, look at Cause.ExcCode.
 * Look at Cause.IP to find line number;
 *  if line 1, it's localTimerInt();
 *  if line 2, it's pseudoClockInt();
 *  if 3-7, then deviceInterrupt();
 * }
 * */

/**
 * void deviceInterrupt(){
 * which device (look at Bus register)
 * find bit number with priority for specific device 
 *  (bit 1 has highest priority while bit 7 has low)
 * save the status code
 * acknowledge the interrupt (so write acknowledge in device register)
 * pcb_t newPcb = SYS4 on the device sema4
 * newPcb ->v0 = status code
 * insertProcQ(readyQ, newPcb);
 * newPcb status = ready;
 * LDST(saved exception state);
 * }
 * */

/**
 * QUANTUM IS OVER!
 * 
 * void localTimerInt(){
 * PLT Interrupt ⇒ you have run out of time
 * acknowledge the interrupt by refilling timer
 * Copy state into currentProc -> p_s 
 * insertProcQ(readyQ, currentProc);
 * Change currentProc state to ready state
 * scheduler();
 * }
 * */

/**
 * PseudoClock is done!
 * 
 * void pseudoClockInt(){
 * We sys4 everything that SYS7 on the pseudoClock
 * reset pseudoClock to 0
 * LDST(currentProc) (could also be a wait, so make sure to include this is scheduler)
 * }
 * */
