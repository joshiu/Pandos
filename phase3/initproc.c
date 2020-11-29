#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"

/**
 * This file 
 * 
 * 
 * counter
 ritten by Umang Joshi and Amy Kelley
 * */

/**********************GLOBAL VARIABLES*****************/





/*end of globals*/

extern userGeneralExceptHandler();
extern pageHandler();

int master;
int deviceRegisterSema4[DEVCNT+DEVPERINT];

/**
 * 
 * */
void test(){

    int counter;
    int success;
    master = 0;
    state_t procState;
    static support_t uProc[8];

    initTLBsupport();

    for(counter = 0;counter < UPROCMAX ; counter++){
        procState.s_sp = USTKPTR;
        procState.s_entryHI = counter<<ASIDBITS;
        procState.s_pc = procState.s_t9 = USTARTADDR;
        procState.s_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON | USERPREVON);

        uProc[counter].sup_asid = counter+1;
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_stackPtr = USTKPTR; /*wrong*/
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) userGeneralExceptHandler;

        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_stackPtr = USTKPTR; /*wrong*/
        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_pc = (memaddr) pageHandler;

        uProc[counter].sup_pageTable[MAXPROC].pgTE_entryHi = (0xBFFFF <<VPNBITS);

        success = SYSCALL(MAKEPROCESS, (int)&procState, &(uProc[counter]), 0);

        if(success != OK){
            SYSCALL(KILLPROCESS, 0, 0, 0);
        }
    }

    for(counter = 0; counter < UPROCMAX; counter ++){
        SYSCALL(PASSERN, (int)&master, 0, 0);
    }

    SYSCALL(KILLPROCESS, 0, 0, 0);
}