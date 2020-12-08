#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"
#include "../h/initProc.h"


/**
 * This file provides the system support for SYS call 
 * instructions. All appropriate values are placed in registers a0-a3 before executing 
 * the SYSCALL instruction and will execute based on the value in a0. 
 * This file also contains other support functions.
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G and Paul K
 * in debugging.
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
 * It has the same functionality as the generalExceptHandler,
 * but only handles syscalls and program traps. 
 * */
void userGeneralExceptHandler(){

    /*local variables*/
    support_t * supportInfo; /*support information*/
    int causeNum; /*cause number*/
    /*end of locals*/

    /*get support information via sys8*/
    supportInfo = (support_t *) SYSCALL(SUPPORTDATA, 0, 0, 0);

    causeNum = (supportInfo->sup_exceptState[GENERALEXCEPT].s_cause & GETCAUSE) >> 2;

    if(causeNum == GOTOSYSCALL){
        /*if the cause is 0*/

        userSyscall(supportInfo);
    
    }

    /*rest are program traps*/
    uProgramTrap(supportInfo);

}



/**
 * This method handles all user level program
 * traps. It passes the process to killproc.
 * */
void uProgramTrap(support_t *supportInfo){

    killProc(NULL);
}



/**
 * This method will block the process with the
 * given semAddr (using Passern).
 * */
void block(int * semAddr){

    SYSCALL(PASSERN, (int) semAddr, 0, 0);

}



/**
 * This method will call a Verhogen on the
 * process with the given semAddr.
 * */
void unblock(int *semAddr){

    SYSCALL(VERHOGEN, (int)semAddr, 0, 0);

}



/**
 * This method will kill the process
 * with the given semAddr. It will unblock
 * if necessary
 * */
void killProc(int *semAddr){

    if(semAddr != NULL){
        /*if blocked, then unblock it*/    
        
        unblock(semAddr);
    
    }

    SYSCALL(KILLPROCESS, 0, 0, 0);
}



/**
 *  This method is our phase 3 loadState.
 * */
void userLoadState(state_t *loadState){
    
    LDST(loadState);

}



/**
 * This is the user level syscall handler. It will
 * handle syscall 9 through 13. Sys 9 is terminate, 
 * sys 10 gets the clock, sys 11 writes to 
 * the printer, sys 12 write to terminal,
 * sys 13 reads from the terminal. If none of these, 
 * it kills the process. 
 * */
void userSyscall(support_t *supportInfo){
    
    /*local variables*/
    int uSysNum; 
    /*end local variables*/

    /*syscall number in a0 of exceptState*/
    uSysNum = supportInfo ->sup_exceptState[GENERALEXCEPT].s_a0;

    /*look at uSysNum and find the correct case*/
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
        /*if none of these, then kill it*/

        killProc(NULL);
        break;
    
    }
    
    }/*end of switch statement*/

    /*increment program counter to prevent looping*/
    supportInfo ->sup_exceptState[GENERALEXCEPT].s_pc += 4;

    /*loadstate back to process*/
    userLoadState(&(supportInfo->sup_exceptState[GENERALEXCEPT]));
    
}



/**
 * When requested, this service kills the 
 * U-proc via killProc.
 * */
void sys_9(){
    
    killProc(NULL);

}



/**
 * This method when requested takes the number of
 * microseconds since the system was last booted/reset
 * and places it in UProc's v0
 * */
void sys_10(support_t *supportInfo){
    
    /*local variables*/
    cpu_t timeEnded; /*time the system was last booted/reset*/
    /*end of local variables*/

    STCK(timeEnded); /*record time*/

    supportInfo->sup_exceptState[GENERALEXCEPT].s_v0 = timeEnded;

}



/**
 * When requested, this service causes the requesting U-proc to be suspended 
 * until a line of characters has been transmitted to the printer device
 * associated with the U-proc. This method only writes to the printer.
 * */
void sys_11(support_t *supportInfo){

    /*local variables*/
    int counter; /*length counter*/
    int length; /*string length*/
    int status; /*makes sure sys5 occurs*/
    int devSema4Num; /*sema4 number for device*/
    int idNum; /*asid number for device*/
    int error; /*has an error occured?*/
    char *letterToPrint; /*pointer to addr of next letter for print*/
    devregarea_t *deviceRegister; /*device register*/
    /*end local variables*/

    idNum = supportInfo->sup_asid; /*get asid*/

    deviceRegister = (devregarea_t *) RAMBASEADDR;
    devSema4Num = ((PRNTINT-DISKINT) *DEVPERINT) +(idNum - 1); /*get device num*/
    
    /*get chars and length of chars to print*/
    letterToPrint = (char *)supportInfo->sup_exceptState[GENERALEXCEPT].s_a1;
    length = supportInfo->sup_exceptState[GENERALEXCEPT].s_a2;

    /*if length is invalid or char to print invalid*/
    if(((int)letterToPrint < KUSEG) || (length<=0) || (length >MAXWORDLEN)){
        
        /*death*/
        killProc(NULL);
    
    }

    block(&deviceRegisterSema4[devSema4Num]); /*ensure mutual exclusion*/

    /*set counter and error condition*/
    counter = 0;
    error = FALSE;

    while((counter<length) && (!error)){

        /*place char in data0 and transmit it*/
        deviceRegister->devreg[devSema4Num].d_data0 = *letterToPrint;
        deviceRegister -> devreg[devSema4Num].d_command = CHARTRANSMIT;

        /*wait until this is done*/
        status = SYSCALL(WAITIO, PRNTINT, idNum-1, 0);

        if(status != READY){
            error = TRUE;
        }
        else{
            counter+=1;
        }

        /*move onto next char*/
        letterToPrint++;
    }

    /*remove mutual exclusion*/
    unblock(&deviceRegisterSema4[devSema4Num]);

    /*how many characters were printed*/
    supportInfo -> sup_exceptState[GENERALEXCEPT].s_v0 = counter; 
}




/**
 * When requested, this service causes the requesting U-proc to be suspended until
 * a line of chars has been transmitted to the terminal device
 * associated with the U-proc. This method only prints to terminal.
 * */
void sys_12(support_t *supportInfo){

    /*local variables*/
    int counter; /*length counter*/
    int length; /*string length*/
    int status; /*makes sure sys5 occurs*/
    int devSema4Num; /*sema4 number for device*/
    int idNum; /*asid number for device*/
    int error; /*has an error occured?*/
    char *letterToPrint; /*pointer to addr of next letter for print*/
    devregarea_t *deviceRegister; /*device register*/
    unsigned int statusReg; /*interrupt status register*/
    /*end local variables*/

    idNum = supportInfo->sup_asid; /*get asid*/

    deviceRegister = (devregarea_t *) RAMBASEADDR;
    devSema4Num = ((TERMINT-DISKINT) *DEVPERINT) +(idNum - 1); /*get dev num*/
    
    /*get chars to print and length of chars*/
    letterToPrint = (char *)supportInfo->sup_exceptState[GENERALEXCEPT].s_a1;
    length = supportInfo->sup_exceptState[GENERALEXCEPT].s_a2;

    /* if length invalid or char invalid for uproc*/
    if(((int)letterToPrint < KUSEG) || (length<=0) || (length >MAXWORDLEN)){
        
        /*death*/
        killProc(NULL);
    
    }

    /*ensure mutual exclusion*/ 
    block(&deviceRegisterSema4[devSema4Num]);

    /*set counter and error condition*/
    counter = 0;
    error = FALSE;

    while((counter<length) && (!error)){

        /*get ready to transmit the char*/
        deviceRegister->devreg[devSema4Num].t_transm_command = *letterToPrint <<BYTELENGTH |CHARTRANSMIT;
        
        /*write it into data0*/
        deviceRegister->devreg[devSema4Num].d_data0 = *letterToPrint;

        /*turn off interrupts*/
        statusReg = getSTATUS();
        setSTATUS((statusReg & DISABLEALL));

        /*transmit char*/
        deviceRegister -> devreg[devSema4Num].d_command = CHARTRANSMIT;
        
        /*wait until this is done*/
        status = SYSCALL(WAITIO, TERMINT, idNum-1, 0);

        /* turn ON interrupts*/
        statusReg = getSTATUS();
        setSTATUS((statusReg|0x1));

        /*check the status of process*/
        if((status & 0xFF) != CHARTRANSGOOD){
            error = TRUE;
        }
        else{
            counter+=1;
        }

        letterToPrint++;
    }

    /*remove mutual exclusion*/
    unblock(&deviceRegisterSema4[devSema4Num]);

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
    int counter; /*length counter*/
    int status; /*makes sure sys5 occurs*/
    int done; /*condition to see if read is done*/
    int devSema4Num; /*sema4 number for device*/
    int idNum; /*asid number for device*/
    int error; /*has an error occured?*/
    char *letterAddr; /*pointer to addr of next letter for print*/
    devregarea_t *deviceRegister; /*device register*/
    unsigned int statusReg; /*interrupt status register*/
    /*end local variables*/

    idNum = supportInfo->sup_asid;/*get asid*/

    deviceRegister = (devregarea_t *) RAMBASEADDR;
    devSema4Num = ((TERMINT-DISKINT) *DEVPERINT) +(idNum - 1);/*get device num*/
    
    /*get addr of char to read*/
    letterAddr = (char *)supportInfo->sup_exceptState[GENERALEXCEPT].s_a1;

    /*if invalid char*/
    if(((int)letterAddr < KUSEG)){

        /*death*/
        killProc(NULL);

    }

    /*ensure mutual exclusion*/
    block(&deviceRegisterSema4[devSema4Num +DEVPERINT]);

    /*set counter, error condition, and done condition*/
    counter = 0;
    error = FALSE;
    done = FALSE;

    while((!done) && (!error)){

        /*turn off interrupts*/
        statusReg = getSTATUS();
        setSTATUS((statusReg & DISABLEALL));

        /*read the char*/
        deviceRegister ->devreg[devSema4Num].t_recv_command = CHARTRANSMIT;
        
        /*wait for this to be done*/
        status = SYSCALL(WAITIO, TERMINT, idNum-1, 1);

        /*turn ON interrupts*/
        statusReg = getSTATUS();
        setSTATUS((statusReg|0x1));
        

        if((status & 0xFF) != CHARTRANSGOOD){

            error = TRUE;
        
        }

        /*if reading successful*/ 
        else{
            /*move counter and letteraddr; check done*/
            counter+=1; 

            *letterAddr = status >>8;
            letterAddr++;
            
            if((status>>8) == 0x0A){
                done =TRUE;
            }
        }
    }

    /*remove mutual exclusion*/
    unblock(&deviceRegisterSema4[devSema4Num+DEVPERINT]);

    if(error){
        counter = 0 - (status & 0xFF);
    }

    /*how many characters were read*/
    supportInfo -> sup_exceptState[GENERALEXCEPT].s_v0 = counter; 

}
