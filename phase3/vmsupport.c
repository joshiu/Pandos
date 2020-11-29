#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"
#include "../h/vmsupport.h"
#include "../h/syssupport.h"

/**
 * This file is part of the Support Level of the Nucleus. It's job is 
 * to support the address translation/virtual memory. Each U-proc will execute in
 * its own identically structured logical address space (kuseg), with a unique
 * ASID. (Address space identifiers: i.e. process ID)
 * 
 * Written by Umang Joshi and Amy Kelley
 * */




/****************GLOBAL VARIABLES****************/

swap_t swapPool[POOLSIZE]; /*declares the array for the swap pool */
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
        swapPool[counter].sw_asid = -1;
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

    oldState = (state_t *)BIOSDATAPAGE;
    
    pageNumber = ((oldState ->s_entryHI) & GETPAGENUM) >> VPNBITS;
    pageNumber %= MAXPAGE;

    setENTRYHI(currentProc->p_supportStruct->sup_pageTable[pageNumber].pgTE_entryHi);
    setENTRYLO(currentProc->p_supportStruct->sup_pageTable[pageNumber].pgTE_entryLo);

    TLBWR();

    LDST(oldState);

}

/**
 * This method takes the passed up page faults. Then it will
 * follow the appropriate steps to handle the page fault.
 * */
void pageHandler(){

    /*Local Variables*/
    int procASID;
    int frameNum;
    int pageNum;
    int status;
    support_t *suppData;
    int cause;
    pgTableEntry_t *pgTEntry;
    unsigned int address;
    int blockNum;
    int newBlockNum;
    unsigned int statusReg;
    /*End of Local Varaibles*/
 
    suppData = SYSCALL(SUPPORTDATA, 0, 0, 0);

    cause = (suppData->sup_exceptState[PGFAULTEXCEPT].s_cause & GETCAUSE) >>2;
    procASID = suppData->sup_asid;

    if((cause != 2) && (cause != 3)){
        /*TLB invalid*/
        killProc(NULL);
    }

    pageNum = ((suppData->sup_exceptState[PGFAULTEXCEPT].s_entryHI) & GETPAGENUM);

    block(&swapSem);

    frameNum = getFrame();
    address = FRAMEPOOLSTART + (frameNum*PAGESIZE);

    if(swapPool[frameNum].sw_asid != -1){
        
        statusReg = getSTATUS();
        setSTATUS((statusReg & DISABLEALL));

        swapPool[frameNum].sw_pte->pgTE_entryLo = swapPool[frameNum].sw_pte->pgTE_entryLo & 0xFFFFFDFF;
        TLBCLR();
        
        statusReg = getSTATUS();
        setSTATUS((statusReg|0x1));

        blockNum = swapPool[frameNum].sw_pageN;
        blockNum %= MAXPAGE;

        status = writeFlashOperation(((swapPool[frameNum].sw_asid)-1), blockNum, address);

        if(status != READY){
            killProc(&swapSem);
        }
    }

    blockNum = pageNum;
    blockNum %= MAXPAGE;

    status = readFlashOperation((procASID-1), blockNum, address);

    if(status != READY){
        killProc(&swapSem);
    }

    pgTEntry = &(suppData->sup_pageTable[blockNum]);

    swapPool[frameNum].sw_asid = procASID;
    swapPool[frameNum].sw_pageN = pageNum;
    swapPool[frameNum].sw_pte = pgTEntry;

    statusReg = getSTATUS();
    setSTATUS((statusReg & DISABLEALL));

    swapPool[frameNum].sw_pte->pgTE_entryLo = (address | DIRTYON | VALIDON);
    TLBCLR();
    
    statusReg = getSTATUS();
    setSTATUS((statusReg|0x1));

    unblock(&swapSem);

    userLoadState(&(suppData->sup_exceptState[PGFAULTEXCEPT]));
    

}



/**************HELPER FUNCTIONS BELOW*************/

/**
 * round robin frame selector
 * */
int getFrame(){
    static int nextFrame = 0;

    nextFrame = (nextFrame +1) % POOLSIZE;
    return(nextFrame);

}


int readFlashOperation(int deviceNum, int blockNum, int address){
    int status;
    devregarea_t *deviceRegister;
    unsigned int statusReg;

    deviceRegister = (devregarea_t *) RAMBASEADDR;

    statusReg = getSTATUS();
    setSTATUS((statusReg & DISABLEALL));

    deviceRegister->devreg[deviceNum+8].d_data0 = address;
    deviceRegister->devreg[deviceNum+8].d_command = ((blockNum << 8) | TLBWRITE);

    status = SYSCALL(WAITIO, FLASHINT, deviceNum, 0);

    statusReg = getSTATUS();
    setSTATUS((statusReg | 0x1));

    if(status != READY){
        status = 0 - status;
    }

    return status;
}

int writeFlashOperation(int deviceNum, int blockNum, int address){
    int status;
    devregarea_t *deviceRegister;
    unsigned int statusReg;

    deviceRegister = (devregarea_t *) RAMBASEADDR;

    statusReg = getSTATUS();
    setSTATUS((statusReg & DISABLEALL));
    
    deviceRegister->devreg[deviceNum+8].d_data0 = address;
    deviceRegister->devreg[deviceNum+8].d_command = ((blockNum << 8) | TLBREAD);

    status = SYSCALL(WAITIO, FLASHINT, deviceNum, 0);

    statusReg = getSTATUS();
    setSTATUS((statusReg | 0x1));

    if(status != READY){
        status = 0 - status;
    }

    return status;
}