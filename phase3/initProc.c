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
 * This method also contains a master semaphore that remains blocked
 * to ensure the program terminates in kernel panic(). It also utilizes
 * a deviceRegistersema4 list that keeps track of all the sema4s for the
 * device registers. 
 * 
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G and Paul K 
 * in debugging.
 * */


/**********************GLOBAL VARIABLES*****************/

int master; /*semaphore for master process*/
int deviceRegisterSema4[DEVCNT+DEVPERINT]; /*sema4 list for device register*/

/*end of globals*/

/**
 * This method will create all of our processes and
 * will initialize all the user level processes. This
 * is done by the master process we make in init.c. 
 * It will also initialize the swap pool table
 * and swap pool semaphore. It will also create (via sys1), 
 * initalize all 8 user processes with the necessary data.
 * Then, it will block the master process indefinitely, ensuring
 * the program will die in kernel panic().
 * */
void test(){

    /*local variables*/
    int counter; /*counter for creating the processes and dev register sema4*/
    int counter2; /*second counter to set the page table*/
    int success; /*variable to indicate successfull creation*/
    int asid; /*variable to keep track of asid*/
    master = 0; /*initializing master*/
    state_t procState; /*state of processes*/
    static support_t uProc[8]; /*list of all possible user processes*/
    /*end of local variables*/

    initTLBsupport(); /*initialize swapPool and swapSem*/

    for(counter = 0; counter < (DEVCNT+DEVPERINT); counter++){
        /*init device register sema4s*/

        deviceRegisterSema4[counter] = 1;
    
    }

    for(counter = 0;counter < UPROCMAX; counter++){
        asid = counter +1; /*set asid*/

        /*setup process' states*/
        procState.s_sp = (int) USTKPTR;
        procState.s_entryHI = asid <<ASIDBITS;
        procState.s_pc = procState.s_t9 = (memaddr) USTARTADDR;
        procState.s_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON | USERPREVON);

        /*set up user process*/
        uProc[counter].sup_asid = asid;
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_stackPtr = (int) &(uProc[counter].sup_GeneralStack[500]);
        uProc[counter].sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) userGeneralExceptHandler;

        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_status = (ALLOFF | IEPREVON | IMASKON | TIMEREBITON);
        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_stackPtr = (int) &(uProc[counter].sup_PGFaultStack[500]);
        uProc[counter].sup_exceptContext[PGFAULTEXCEPT].c_pc = (memaddr) pageHandler;

        /*setup page table for user proc*/
        for(counter2 = 0; counter2 < UPGTBSIZE; counter2 ++){
            uProc[counter].sup_pageTable[counter2].pgTE_entryHi = ((0x80000 +counter2) <<VPNBITS) | (asid <<ASIDBITS);
            uProc[counter].sup_pageTable[counter2].pgTE_entryLo = ALLOFF | DIRTYON;
        }

        /*setup 2nd last page to be stack*/
        uProc[counter].sup_pageTable[UPGTBSIZE-1].pgTE_entryHi = (0xBFFFF <<VPNBITS) | (asid<<ASIDBITS);

        /*birth the process with above fields*/
        success = SYSCALL(MAKEPROCESS, (int)&procState, &(uProc[counter]), 0);

        if(success != OK){
            SYSCALL(KILLPROCESS, 0, 0, 0);
        }
    }

    /*once all processes made, block master to ensure eventual death*/
    SYSCALL(PASSERN, (int)&master, 0, 0);    

}