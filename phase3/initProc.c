#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"
#include "../h/vmSupport.h"
#include "../h/sysSupport.h"

/**
 * This file will create all of our needed processes for phase3
 * and it will also initialize all these processes at the user level.
 * It will implement and execute the test(); method once. It also 
 * exports the Support level's global variabes and a masterSemaphore.
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
 * will initialize all the user level processes. 
 * It will also initialize the swap pool table
 * and swap pool semaphore. It will also create (via sys1), 
 * initalize and launch 1-8 Uprocs. Then, it will block the master.
 * Since the master is blocked, it will die in kernel panic()
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

        uProc[counter].sup_asid = asid;
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_stackPtr = (int) &(uProc[counter].sup_GeneralStack[500]);
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) userGeneralExceptHandler;

        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_stackPtr = (int) &(uProc[counter].sup_PGFaultStack[500]);
        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_pc = (memaddr) pageHandler;

        for(counter2 = 0; counter2 < UPGTBSIZE; counter2 ++){
            uProc[counter].sup_pageTable[counter2].pgTE_entryHi = ((0x80000 +counter2) <<VPNBITS) | (asid <<ASIDBITS);
            uProc[counter].sup_pageTable[counter2].pgTE_entryLo = ALLOFF | DIRTYON;
        }

        uProc[counter].sup_pageTable[UPGTBSIZE-1].pgTE_entryHi = (0xBFFFF <<VPNBITS) | (asid<<ASIDBITS);

        success = SYSCALL(MAKEPROCESS, (int)&procState, &(uProc[counter]), 0);

        if(success != OK){
            SYSCALL(KILLPROCESS, 0, 0, 0);
        }
    }

    SYSCALL(PASSERN, (int)&master, 0, 0);

    debuga(999);

}