#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"

/**
 * This file provides the system support for SYS call 
 * instructions. All appropriate values are placed in registers a0-a3 before executing 
 * the SYSCALL instruction and will execute based on the value in a0. 
 * This file also contains other support functions.
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/*********************FILE SPECIFIC METHODS******************************/

/* NOTE: (READ ME): I know all of these aren't void and they don't have have the right 
pramaters but I wanted to set everything up or maybe i just fucked everything up idk*/
HIDDEN void userSyscall();
HIDDEN void sys_9();
HIDDEN void sys_10();
HIDDEN void sys_11(support_t *supportStruct);
HIDDEN void sys_12(support_t *supportStruct);
HIDDEN void sys_13(support_t *supportStruct);

/**end of file specific methods**/

/**
 * Insert method comment here
 * Gen except for u procs
 * */
void userGeneralExceptHandler(){
    /*local variables*/
    support_t * supportInfo;
    int causeNum;
    /*end of locals*/

    supportInfo = (support_t *) SYSCALL(SUPPORTDATA, 0, 0, 0);

    causeNum = (supportInfo->sup_exceptState[GENERALEXCEPT].s_cause & GETCAUSE) >> 2;

    if(causeNum == GOTOSYSCALL){
        userSyscall(supportInfo);
    }

    uProgramTrap(supportInfo);

}

void uProgramTrap(support_t *supportInfo){

    killProc(NULL);
}

/**
 * Helper method comment
 * */
void block(int * semAddr){

    SYSCALL(PASSERN, (int) semAddr, 0, 0);

}

/**
 * Helper method comment
 * */
void unblock(int *semAddr){

    SYSCALL(VERHOGEN, (int)semAddr, 0, 0);

}

/**
 * Helper method comment
 * */
void killProc(int *semAddr){

    if(semAddr != NULL){
        /*if blocked, then unblock it*/    
        unblock(semAddr);
    }
    
    unblock(&masterSem); /*we need to make this in initproc.c*/
    
    SYSCALL(KILLPROCESS, 0, 0, 0);
}

/**
 * Helper method comment
 * */
void enableInterrupts(int turnON){
    unsigned int statusReg;

    statusReg = getSTATUS();

    if(turnON){
        statusReg = statusReg|0x1;
    }
    else{
        statusReg = statusReg & DISABLEALL;
    }

    setSTATUS(statusReg);

}

/**
 * Helper method comment
 * */

void userLoadState(state_t *loadState){
    
    LDST(loadState);

}
/**
 * Not sure this is needed but just in case (method comment here)
 * We also may need a new name for this, but I wasn't sure if it mattered
 * */
void userSyscall(support_t *supportInfo){
    /*local variables*/
    int uSysNum; 
    cpu_t endTime;
    /*end local variables*/

    uSysNum = supportInfo ->sup_exceptState[GENERALEXCEPT].s_a0;

    switch (uSysNum)
    {
    case TERMINATE: {
        sys_9(NULL);
        break;
    }

    case GETTOD: {
        sys_10();
        break;
    }

    case WRITETOPRINTER: {
        sys_11(supportInfo);
        break;
    }

    case WRITETOTERMINAL: {
        sys_12(supportInfo);
        break;
    }

    case READFROMTERMINAL: {
        sys_13(supportInfo);
        break;
    }

    default: {
        killProc(NULL);
        break;
    }/*end of switch statement*/

    supportInfo ->sup_exceptState[GENERALEXCEPT].s_pc = supportInfo->sup_exceptState[GENERALEXCEPT];

    userLoadState(&(supportInfo->sup_exceptState[GENERALEXCEPT]));
    
}

/**
 * When requested, this service ceases the 
 * U-proc to exist.
 * */
void sys_9(){

    /*READ ME: I understand what to do here, (similar to sys2) but I'm not 100% sure on the new
    U-proc delcarations and everything. Which also make doing sys 10 weird*/

}

/**
 * This method when requested places the number of
 * microseconds since the system was last booted/reset
 * and places it in UProc's v0
 * */
void sys_10(){
    
    /*local variables*/
    cpu_t timeStarted; /*time the system was last booted/reset
    /*end of local variables*/

    STCK(timeStarted); /*is this how we read the clock for UProc? or is there something else?*/

    currentProc->p_s.s_v0 = timeStarted;
    loadState(currentProc);
}

/**
 * When requested, this service causes the requesting U-proc to be suspended until
 * a line of output (string of characters) has been transmitted to the printer device
 * associated with the U-proc
 * */
void sys_11(support_t *supportStruct){

}


/**
 * When requested, this service causes the requesting U-proc to be suspended until
 * a line of output (string of characters) has been transmitted to the terminal device
 * associated with the U-proc
 * */
void sys_12(support_t *supportStruct){

}


/**
 * When requested, this service causes the requesting U-proc to be suspended
 *  until a line of input (string of characters) has been transmitted from the 
 * terminal device associated with the U-proc.
 * */
void sys_13(support_t *supportStruct){

}
