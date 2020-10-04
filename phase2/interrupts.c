#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"

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

/**
 * idk where to put this, so it's going here!
 * 
 * void passUpOrDie(){
 * if currentProc(p_SupportStruct == NULL) -> sys2
 * Else:
 * two tasks: copy and save the exception state into a location accessible 
 * pass control to a routine specified by the Support Level
 * 
 * Support level: two locations for saved exception states, two addresses for handlers.
 * One state t/PC address pair for both TLB exceptions & one for all other exceptions
 * 
 * To pass up the handling of an exception:
 * Copy the saved exception state from the BIOS 
 * Give to the correct sup exceptState field of the Current Process.
 * 
 * Important: The Current Process’s pcb should point to a non-null support t.
 * 
 * Perform a LDCXT using the fields from the correct sup exceptContext field of the Current Process
 * }
 * */