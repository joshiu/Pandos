#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"

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
/* GLOBAL VARIABLES */
int processCnt;
int softBlockCnt;
pcb_t *readyQ;
pcb_t *currentProc;
int devicesSema4s[49]; /*2 sema4s per device :, 5 devices per terminal, so array size is 49*/
passupvector_t *passUpVec;
int counter;
/* END GLOBAL VARIABLES*/

extern void uTLB_RefillHandler();
extern void test();

main()
{ 
    passUpVec = PASSUPVECTOR; /*a pointer that points to the PASSUPVECTOR*/

    passUpVec-> tlb_refll_handler = (memaddr) uTLB_RefillHandler;
    passUpVec -> tlb_refll_stackPtr = 0x20001000;
    passUpVec-> exception_handler = (memaddr) exceptionHandler; /*have not created this yet*/
    passUpVec -> exception_stackPtr = 0x200010000;

    initPcbs();
    initASL();

    processCnt = 0;
    softBlockCnt= 0;
    readyQ = mkEmptyProcQ();
    currentProc = NULL;

    /*loop that initializes all devicesema4s to 0 (idk if this needed)*/
    for(counter = 0; counter < 49; counter ++){
        devicesSema4s[counter] = 0;
    }

    LDIT(100); /*load interval timer with 100 ms*/
    
    pcb_t *newPcb = allocPcb();
    
    /* we need to set the state such that interrupts, PLT, kernel mode enabled*/
    /* also need to set SP to RAMTOP and PC to test*/

    newPcb ->p_s.s_pc = &(test());/*set PC*/
    newPcb -> p_s.s_t9 = &(test());/*for technical reasons, we do this when we set the pc*/
    newPcb -> p_s.s_TE = 1; /*timer enable bit*/
    newPcb ->p_s.s_sp = RAMTOP; /*stack pointer*/
    newPcb -> p_s.s_KUp = 0;/*enable kernel mode*/
    newPcb ->p_s.s_IEp = 1; /*enable interrupts*/
    
    newPcb->p_time = 0;
    newPcb->p_supportStruct = NULL;
    insertProcQ(&readyQ, newPcb);
    processCnt ++;
    scheduler();
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
 * 
 * after this never return to main again
 * */
/**
 * genExceptionHandler(take in file state/cause register);
 * create a branch: look at Cause.ExcCode
 * interupt go to interupt, 
 * if syscall go to syscall(interrupt.c), 
 * if program execption go to exceptions.c,
 * */