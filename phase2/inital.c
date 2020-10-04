#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"

/**
 * insert file comment here
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/****************************Detailed Pseudo Code*************************************************/

/**
 * contains the main
 * 
 * 
 * ASL(); instantiate Pcb and make the state of the Pcb into another state (ready) and put on readyQ
 * then insert and run scheduler()
 * */

/**
 * extern void uTLB_RefillHandler();
 * 
 * GLOBAL VARIABLES 
 * int processcnt = 0;
 * int softBlockcnt = 0;
 * pcb_t *readyQue = mkEmptyProc();
 * pcb_t *currentProc = NULL;
 * int devicesSema4s[49] = 0; 2 sema4s per device :, 5 devices per terminal : array size is 49
 * initPCB();
 * initASL();
 * 
 * something to populate passup at 0x0FFF.F900
 * xxx->tlb refll handler = (memaddr) uTLB RefillHandler;
 *
 * set stack pointer for nucleus TLB refill event to the top of the nucleus event: Ox2000.1000
 * set xxx-> expection_handler = (memaddr) fooBar; (change foo name)
 * set stack pointer for the expection handler to top of Nucleus 0x2000.1000
 * 
 * initilzate all the nucleus maintained variables
 * set system wide interval timer at 1000 millsecs
 * 
 * pcb_t newPcb = allocPCB();
 * in alloc, do these 
 * set all process tree fields to NULL
 * set p_time = 0;
 * set p_semAdd = NULL;
 * set p_supportstruct = NULL;
 * 
 * insertProcQ(readyQ, newPcb);
 * processcnt ++;
 * init allocate state on PCB (interrupts enable, process local timer enabled, kernel mode on)
 * SP = RAMTOP
 * PCADDRESS = TEST
 * 
 * scheduler();
 * after this never return to main again
 * */
/**
 * genExceptionHandler(take in file state/cause register);
 * create a branch: look at Cause.ExcCode
 * interupt go to interupt, 
 * if syscall go to syscall(interrupt.c), 
 * if program execption go to exceptions.c,
 * */