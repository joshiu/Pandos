#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"
#include "../h/vmSupport.h"
#include "../h/sysSupport.h"

/**
 * This file will create all of our needed processes for phase3
 * and it will also initialize all these processes at the user level.
 * (add more)
 * 
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

void debuga(int a){
    int j;
    j =1;
}

/**********************GLOBAL VARIABLES*****************/

int master;
int deviceRegisterSema4[DEVCNT+DEVPERINT];

/*end of globals*/

/**
 * This method will create all of our processes and
 * will initialize all the user level processes. (likely need more)
 * */
void test(){

    /*local variables*/
    int counter;
    int counter2;
    int success;
    int asid;
    master = 0;
    state_t procState;
    static support_t uProc[8];
    /*end of local variables*/

    initTLBsupport();

    for(counter = 0; counter < (DEVCNT+DEVPERINT); counter++){
        deviceRegisterSema4[counter] = 1;
    }

    for(counter = 0;counter < UPROCMAX; counter++){
        asid = counter +1;
        procState.s_sp = (int) USTKPTR;
        procState.s_entryHI = asid <<ASIDBITS;
        procState.s_pc = procState.s_t9 = (memaddr) USTARTADDR;
        procState.s_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON | USERPREVON);

        uProc[asid].sup_asid = asid;
        uProc[asid].sup_exceptContext[GENERALEXCEPT].c_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        uProc[asid].sup_exceptContext[GENERALEXCEPT].c_stackPtr = (int) &(uProc[asid].sup_GeneralStack[500]);
        uProc[asid].sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) userGeneralExceptHandler;

        uProc[asid].sup_exceptContext[PGFAULTEXCEPT].c_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        uProc[asid].sup_exceptContext[PGFAULTEXCEPT].c_stackPtr = (int) &(uProc[asid].sup_PGFaultStack[500]);
        uProc[asid].sup_exceptContext[PGFAULTEXCEPT].c_pc = (memaddr) pageHandler;

        for(counter2 = 0; counter2 < UPGTBSIZE; counter2 ++){
            uProc[asid].sup_pageTable[counter2].pgTE_entryHi = ((0x80000 +counter2) <<VPNBITS) | (asid <<ASIDBITS);
            uProc[asid].sup_pageTable[counter2].pgTE_entryLo = ALLOFF | DIRTYON;
        }

        uProc[asid].sup_pageTable[UPGTBSIZE-1].pgTE_entryHi = (0xBFFFF <<VPNBITS) | (asid<<ASIDBITS);

        success = SYSCALL(MAKEPROCESS, (int)&procState, &(uProc[asid]), 0);

        if(success != OK){
            SYSCALL(KILLPROCESS, 0, 0, 0);
        }
    }

    SYSCALL(PASSERN, (int)&master, 0, 0);

    debuga(999);

}