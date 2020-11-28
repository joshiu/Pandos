#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"
#include "../h/initproc.h"

/**
 * This file provides the system support for SYS call 
 * instructions. All appropriate values are placed in registers a0-a3 before executing 
 * the SYSCALL instruction and will execute based on the value in a0. 
 * This file also contains other support functions.
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/*********************FILE SPECIFIC METHODS******************************/

HIDDEN void userSyscall();
HIDDEN void uProgramTrap(support_t *supportInfo);
HIDDEN void sys_9();
HIDDEN void sys_10();
HIDDEN void sys_11(support_t *supportInfo);
HIDDEN void sys_12(support_t *supportInfo);
HIDDEN void sys_13(support_t *supportInfo);

/**end of file specific methods**/




/**
 * This is the generalExceptHandler for phase3 (or uProcs)
 * It will do the same thing as the phase2 verison.
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

/**
 * This method will kill the program.
 * */
void uProgramTrap(support_t *supportInfo){

    killProc(NULL);
}

/**
 * This method will block the semAddr (using Passern)
 * */
void block(int * semAddr){

    SYSCALL(PASSERN, (int) semAddr, 0, 0);

}

/**
 * This method will call a V on the semAddr
 * */
void unblock(int *semAddr){

    SYSCALL(VERHOGEN, (int)semAddr, 0, 0);

}

/**
 * This method will kill the currentProc.
 * This is needed for sys9.
 * */
void killProc(int *semAddr){

    if(semAddr != NULL){
        /*if blocked, then unblock it*/    
        unblock(semAddr);
    }
    
    unblock(&master); /*we need to make this in initproc.c*/
    
    SYSCALL(KILLPROCESS, 0, 0, 0);
}

/**
 * This method enables interrupts for phase 3 which is required.
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
 *  This method will loadState.
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
    /*end local variables*/

    uSysNum = supportInfo ->sup_exceptState[GENERALEXCEPT].s_a0;

    switch (uSysNum)
    {
    case TERMINATE: {
        sys_9();
        break;
    }

    case GETTOD: {
        sys_10(supportInfo);
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
    }
    
    }/*end of switch statement*/

    supportInfo ->sup_exceptState[GENERALEXCEPT].s_pc = supportInfo->sup_exceptState[GENERALEXCEPT];

    userLoadState(&(supportInfo->sup_exceptState[GENERALEXCEPT]));
    
}

/**
 * When requested, this service ceases the 
 * U-proc to exist.
 * */
void sys_9(){

    killProc(NULL);

}

/**
 * This method when requested places the number of
 * microseconds since the system was last booted/reset
 * and places it in UProc's v0
 * */
void sys_10(support_t *supportInfo){
    
    /*local variables*/
    cpu_t timeEnded; /*time the system was last booted/reset
    /*end of local variables*/

    STCK(timeEnded); /*is this how we read the clock for UProc? or is there something else?*/

    supportInfo->sup_exceptState[GENERALEXCEPT].s_v0 = timeEnded;
}

/**
 * When requested, this service causes the requesting U-proc to be suspended until
 * a line of output (string of characters) has been transmitted to the printer device
 * associated with the U-proc
 * */
void sys_11(support_t *supportInfo){
    /*local variables*/
    int counter;
    int length;
    int status;
    int devSema4Num;
    int idNum;
    int error;
    char *letterToPrint;
    devregarea_t *deviceRegister;
    /*end local variables*/

    idNum = supportInfo->sup_asid;

    deviceRegister = (devregarea_t *) RAMBASEADDR;
    devSema4Num = ((PRNTINT-DISKINT) *DEVPERINT) +(idNum - 1);
    
    letterToPrint = (char *)supportInfo->sup_exceptState[GENERALEXCEPT].s_a1;
    length = supportInfo->sup_exceptState[GENERALEXCEPT].s_a2;

    if(((int)letterToPrint < KUSEG) || (length<=0) || (length >WORDLEN)){
        killProc(NULL);
    }

    block(&globalDevSemReg[devSema4Num]);

    counter = 0;
    error = FALSE;

    while((counter<length) && (!error)){

        deviceRegister->devreg[devSema4Num].d_data0 = *letterToPrint;
        deviceRegister -> devreg[devSema4Num].d_command = TRANSMITCHAR;
        status = SYSCALL(WAITIO, PRNTINT, idNum-1, 0);

        if(status != READY){
            error = TRUE;
        }
        else{
            counter+=1;
        }

        letterToPrint++;
    }

    unblock(&globalDevSemReg[devSema4Num]);

    /*how many characters were printed*/
    supportInfo -> sup_exceptState[GENERALEXCEPT].s_v0 = counter; 
}


/**
 * When requested, this service causes the requesting U-proc to be suspended until
 * a line of output (string of characters) has been transmitted to the terminal device
 * associated with the U-proc
 * */
void sys_12(support_t *supportInfo){
    /*local variables*/
    int counter;
    int length;
    int status;
    int devSema4Num;
    int idNum;
    int error;
    char *letterToPrint;
    devregarea_t *deviceRegister;
    /*end local variables*/

    idNum = supportInfo->sup_asid;

    deviceRegister = (devregarea_t *) RAMBASEADDR;
    devSema4Num = ((TERMINT-DISKINT) *DEVPERINT) +(idNum - 1);
    
    letterToPrint = (char *)supportInfo->sup_exceptState[GENERALEXCEPT].s_a1;
    length = supportInfo->sup_exceptState[GENERALEXCEPT].s_a2;

    if(((int)letterToPrint < KUSEG) || (length<=0) || (length >WORDLEN)){
        killProc(NULL);
    }

    block(&globalDevSemReg[devSema4Num]);

    counter = 0;
    error = FALSE;

    while((counter<length) && (!error)){
        /*can we do 2 separate here*/

        deviceRegister->devreg[devSema4Num].t_transm_command = *letterToPrint <<BYTELENGTH |TRANSMITCHAR;
        
        deviceRegister->devreg[devSema4Num].d_data0 = *letterToPrint;
        deviceRegister -> devreg[devSema4Num].d_command = TRANSMITCHAR;
        
        status = SYSCALL(WAITIO, TERMINT, idNum-1, 0);

        if((status & 0xFF) != OKCHARTRANS){
            error = TRUE;
        }
        else{
            counter+=1;
        }

        letterToPrint++;
    }

    unblock(&globalDevSemReg[devSema4Num]);

    if(error){
        counter = 0 - (status & 0xFF);
    }

    /*how many characters were printed*/
    supportInfo -> sup_exceptState[GENERALEXCEPT].s_v0 = counter; 
}


/**
 * When requested, this service causes the requesting U-proc to be suspended
 *  until a line of input (string of characters) has been transmitted from the 
 * terminal device associated with the U-proc.
 * */
void sys_13(support_t *supportInfo){
    /*local variables*/
    int counter;
    int status;
    int done;
    int devSema4Num;
    int idNum;
    int error;
    char *letterToPrint;
    devregarea_t *deviceRegister;
    /*end local variables*/

    idNum = supportInfo->sup_asid;

    deviceRegister = (devregarea_t *) RAMBASEADDR;
    devSema4Num = ((TERMINT-DISKINT) *DEVPERINT) +(idNum - 1);
    
    letterToPrint = (char *)supportInfo->sup_exceptState[GENERALEXCEPT].s_a1;

    if(((int)letterToPrint < KUSEG)){
        killProc(NULL);
    }

    block(&globalDevSemReg[devSema4Num +DEVPERINT]);

    counter = 0;
    error = FALSE;
    done = FALSE;

    while((!done) && (!error)){
        /*can we do 2 separate here*/

        deviceRegister ->devreg[devSema4Num].t_recv_command = TRANSMITCHAR;
        
        status = SYSCALL(WAITIO, TERMINT, idNum-1, TRUE);

        if((status & 0xFF) != OKCHARTRANS){
            error = TRUE;
        }
        else{
            counter+=1;
            *letterToPrint = status >>8;
            letterToPrint++;
            if((status>>8) == 0x0A){
                done =TRUE;
            }
        }
    }

    unblock(&globalDevSemReg[devSema4Num+DEVPERINT]);

    if(error){
        counter = 0 - (status & 0xFF);
    }

    /*how many characters were printed*/
    supportInfo -> sup_exceptState[GENERALEXCEPT].s_v0 = counter; 

}
