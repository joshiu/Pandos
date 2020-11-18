#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/const.h"
#include "../h/types.h"
#include "../h/initial.h"

/**
 * This file 
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/*********************FILE SPECIFIC METHODS******************************/

/* NOTE: (READ ME): I know all of these aren't void and they don't have have the right 
pramaters but I wanted to set everything up or maybe i just fucked everything up idk*/
HIDDEN void sys_9();
HIDDEN void sys_10();
HIDDEN void sys_11(support_t *supportStruct);
HIDDEN void sys_12(support_t *supportStruct);
HIDDEN void sys_13(support_t *supportStruct);

/**end of file specific methods**/


/**
 * Not sure this is needed but just in case (method comment here)
 * We also may need a new name for this, but I wasn't sure if it mattered
 * */
void syscall(){

    int sysNum2; /*local varible for calling syscalls*/

    if(sysNum2 ==  TERMINATE){
        sys_9();
    }

    if(sysNum2 == GETTOD){
        sys_10();
    }

    if(sysNum2 == WRITETOPRINTER){
        sys_11();
    }

    if(sysNum2 == WRITETOTERMINAL){
        sys_12();
    }

    if(sysNum2 == READFROMTERMINAL){
        sys_13();
    }

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
