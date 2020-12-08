#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"
#include "../h/vmSupport.h"
#include "../h/sysSupport.h"

/**
 * This file is part of the Support Level of the Nucleus. It's job is 
 * to support the address translation/virtual memory. Each U-proc will execute in
 * its own identically structured logical address space (kuseg), with a unique
 * ASID. (Address space identifiers: i.e. process ID)
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G and Paul K
 * in debugging.
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

    /*set all asids to nonexistent*/
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
    int pageNumber; /*current page num*/
    state_t *oldState; /*state from biosdatapage*/
    /*End of Local Variables*/

    oldState = (state_t *)BIOSDATAPAGE;
   
    /*find page number*/
    pageNumber = ((oldState ->s_entryHI) & GETPAGENUM) >> VPNBITS;

    /*adjust to fit correct page*/
    pageNumber = pageNumber % MAXPAGE;

    /*set entry hi and lo*/
    setENTRYHI(currentProc->p_supportStruct->sup_pageTable[pageNumber].pgTE_entryHi);
    setENTRYLO(currentProc->p_supportStruct->sup_pageTable[pageNumber].pgTE_entryLo);

    /*write into random spot*/
    TLBWR();

    userLoadState(oldState);

}

/**
 * This method takes the passed up page faults. Then it will
 * follow the appropriate steps to handle the page fault.
 * */
void pageHandler(){

    /*Local Variables*/
    int procASID; /*process asid number*/
    int frameNum; /*frame number*/
    int pageNum; /*page number*/
    int status; /*status of write/read operation*/
    support_t *suppData; /*support data from process*/
    int cause; /*cause exception*/
    pgTableEntry_t *pgTEntry; /*page table*/
    unsigned int address; /*address of frame*/
    int blockNum; /*page num to block on*/
    unsigned int statusReg; /*status of interrupts*/
    /*End of Local Varaibles*/
 
    /*get support data*/
    suppData = (support_t *) SYSCALL(SUPPORTDATA, 0, 0, 0);

    /*find cause and asid*/
    cause = (suppData->sup_exceptState[PGFAULTEXCEPT].s_cause & GETCAUSE) >>2;
    procASID = suppData->sup_asid;

    /*memory mismanagement*/
    if((cause != 2) && (cause != 3)){
        /*TLB invalid*/

        killProc(NULL);
    }

    /*find page number*/
    pageNum = ((suppData->sup_exceptState[PGFAULTEXCEPT].s_entryHI) & GETPAGENUM)>> VPNBITS;

    /*ensure mutual exclusion*/
    block(&swapSem);

    /*get victim frame and address*/
    frameNum = getFrame();
    address = FRAMEPOOLSTART + (frameNum*PAGESIZE);

    /*if something written in frame, replace it*/
    if(swapPool[frameNum].sw_asid != -1){
        
        /*disable interrupts*/
        statusReg = getSTATUS();
        setSTATUS((statusReg & DISABLEALL));

        /*adjust swap pool and clear entry*/
        swapPool[frameNum].sw_pte->pgTE_entryLo = ((swapPool[frameNum].sw_pte->pgTE_entryLo) & 0xFFFFFDFF);
        TLBCLR();
        
        /*enable interrupts*/
        statusReg = getSTATUS();
        setSTATUS((statusReg|0x1));

        /*set block num to page number of entry*/
        blockNum = swapPool[frameNum].sw_pageN;
        blockNum = blockNum % MAXPAGE;

        /*write entry*/ 
        status = writeFlashOperation(((swapPool[frameNum].sw_asid)-1), blockNum, address);

        if(status != READY){
        
            /*if not ready, kill process*/
            killProc(&swapSem);
        
        }
    }

    /*if no existing process, do only read*/

    /*get block num from page num*/
    blockNum = pageNum;
    blockNum = blockNum % MAXPAGE;
    
    /*read in the entry*/
    status = readFlashOperation((procASID-1), blockNum, address);

    if(status != READY){

        /*if not ready, kill it*/
        killProc(&swapSem);
    
    }

    /*set pagetable entry*/
    pgTEntry = &(suppData->sup_pageTable[blockNum]);

    /*set asid, page num and pagetable entry*/
    swapPool[frameNum].sw_asid = procASID;
    swapPool[frameNum].sw_pageN = pageNum;
    swapPool[frameNum].sw_pte = pgTEntry;

    /*disable interrupts*/
    statusReg = getSTATUS();
    setSTATUS((statusReg & DISABLEALL));

    /*edit tlb entry to turn valid and dirty bit on, then clear*/
    swapPool[frameNum].sw_pte->pgTE_entryLo = (address | DIRTYON | VALIDON);
    TLBCLR();
    
    /*enable interrupts*/
    statusReg = getSTATUS();
    setSTATUS((statusReg|0x1));

    /*remove mutual exclusion*/
    unblock(&swapSem);

    /*go back to process*/
    userLoadState(&(suppData->sup_exceptState[PGFAULTEXCEPT]));
    

}



/**************HELPER FUNCTIONS BELOW*************/



/**
 * This helper function is a round robin frame selector.
 * To get a frame in the POOLSIZE.
 * */
int getFrame(){
    static int nextFrame = 0;

    nextFrame = (nextFrame +1) % POOLSIZE;
    return(nextFrame);

}


/**
 * This method will read the flash operation
 * from the current status. Performs this atomically.
 * */
int readFlashOperation(int deviceNum, int blockNum, int address){

    /*local variables*/
    int status; /*IO status*/
    devregarea_t *deviceRegister; /*device register*/
    unsigned int statusReg; /*interrupt status*/
    /*end of local variables*/

    deviceRegister = (devregarea_t *) RAMBASEADDR;

    /*disable interrupts*/
    statusReg = getSTATUS();
    setSTATUS((statusReg & DISABLEALL));

    /*set address and read from TLB*/
    deviceRegister->devreg[deviceNum+8].d_data0 = address;
    deviceRegister->devreg[deviceNum+8].d_command = ((blockNum << 8) | TLBREAD);

    /*wait until IO done*/
    status = SYSCALL(WAITIO, FLASHINT, deviceNum, 0);

    /*enable interrupts*/
    statusReg = getSTATUS();
    setSTATUS((statusReg | 0x1));

    if(status != READY){
        status = 0 - status;
    }

    return status;
}


/**
 * This method will write the flash operation
 * from the current status. Performs this atomically.
 * */
int writeFlashOperation(int deviceNum, int blockNum, int address){

      /*local variables*/
    int status; /*IO status*/
    devregarea_t *deviceRegister; /*device register*/
    unsigned int statusReg; /*interrupt status*/
    /*end of local variables*/

    deviceRegister = (devregarea_t *) RAMBASEADDR;

    /*disable interrupts*/
    statusReg = getSTATUS();
    setSTATUS((statusReg & DISABLEALL));

    /*set address and read from TLB*/
    deviceRegister->devreg[deviceNum+8].d_data0 = address;
    deviceRegister->devreg[deviceNum+8].d_command = ((blockNum << 8) | TLBWRITE);

    /*wait until IO done*/
    status = SYSCALL(WAITIO, FLASHINT, deviceNum, 0);

    /*enable interrupts*/
    statusReg = getSTATUS();
    setSTATUS((statusReg | 0x1));

    if(status != READY){
        status = 0 - status;
    }

    return status;

}