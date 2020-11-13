#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"

/**
 * This file is part of the Support Level of the Nucleus. It's job is 
 * to support the address translation/virtual memory. Each U-proc will execute in
 * its own identically structured logical address space (kuseg), with a unique
 * ASID. (Address space identifiers: i.e. process ID)
 * 
 * Written by Umang Joshi and Amy Kelley
 * */




/****************GLOBAL VARIABLES****************/

swap_t *swapPool[POOLSIZE]; /*declares the array for the swap pool */
int swapSem; /*declares swap pool sema4*/

/****************END OF GLOBALS*****************/



/**
 * This method initilzates the global shared page.
 * */
void initTLBsupport(){

    /*Local Variables*/
    int counter;
    /*End local variables*/

    swapSem = 1;

    for(counter = 0; counter < POOLSIZE; counter++){
        swapPool[counter]->sw_asid = -1;
    }
}


/**
 * This method is an event that occurs when there's a cache-miss and it's job is
 * to insert the  the TLB the missing Page Table entry and restart the instruction.
 * */
void uTLB_RefillHandler(){

    /*Local Variables*/
    int pageNumber;
    state_t *oldState;
    /*End of Local Variables*/

    oldState = ((state_t *)BIOSDATAPAGE);
    /*
    pageNumber = ((oldState ->s_entryHI) & VMGETPG) >> VPNSHIFT;
    note VMGETPG and VPNSHIFT are bit code
    pageNumber %= MAXPAGE;

    oldState->s_entryHI->sup_pageTable[pageNumber].entryHI; 
    oldState->s_entryHI->sup_pageTable[pageNumber].entryLo;
    */

    TLBWR();

    LDST(oldState);

}

/**
 * This method takes the passed up page faults. Then it will
 * follow the appropriate steps to handle the page fault.
 * */
void pageHandler(){

    /*Local Variables*/
    support_t *suppData;
    /*End of Local Varaibles*/
 
    SYSCALL(SUPPORTDATA, (int)NULL, (int)NULL, (int)NULL);

    /*supportstruct v0*/
    
    /*suppData = &(((state_t *)BIOSDATAPAGE)->s_reg.s_v0); copy the struct over*/

    /*get asid*/
    
    /*check to see pgFault*/
    /*if it isn't we die, else continue*/

    /*get missing page num and do SYS3 on swap sema4*/
    /*we get a victim (round robin portion)*/
    /*get the real address*/
    /*if not empty, then we turn off interrupts, adjust valid
    nuke TLB, and enable interrupts*/

    /*refresh backing store and write*/
    /*is write not ready, then killllllllllllllllll*/

    /*find missing page number, and mod with MAXPAGE*/
    /*read it (if not ready, then die)*/

    /*update pg talbe, TLB (interrupts off)*/
    /*then update backing, sys4 on swap sema4 and LDSTs*/

}

/**************HELPER FUNCTIONS BELOW*************/