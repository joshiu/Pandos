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
 * */
main(){
    extern void uTLB_RefillHandler();

    /* GLOBAL VARIABLES */
    int processCnt = 0;
    int softBlockCnt = 0;
    pcb_t *readyQ = mkEmptyProc();
    pcb_t *currentProc = NULL;
    int devicesSema4s[49] = 0; /*2 sema4s per device :, 5 devices per terminal, so array size is 49*/
    /* END GLOBAL VARIABLES*/
    
    initPCB();
    initASL();
    pcb_t *newPcb = allocPcb();
    newPcb->p_time = 0;
    newPcb->p_supportStruct = NULL;
}

/**
 * extern void uTLB_RefillHandler();
 * 
 * GLOBAL VARIABLES 
 * int processCnt = 0;
 * int softBlockCnt = 0;
 * pcb_t *readyQ = mkEmptyProc();
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
 * newPcb -> p_time = 0;
 * newPcb -> p_semAdd = newPcb->s_a0;
 * newPcb -> p_supportstruct = NULL;
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