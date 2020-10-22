#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/initial.h"
#include "../h/scheduler.h" 
#include "../h/initial.h"

/**
 * This file occurs when the SYSCALL assembly instruction is executed. 
 * It places the appropriate values into registers a0-a3 before executing 
 * the SYSCALL instruction and it will execute based on the value in a0.
 * 
 * Written by Umang Joshi and Amy Kelley
 * */


/**
 * This System Call method occurs when the SYSCALL assembly 
 * intruction is executed. This places the appropriate values in 
 * the general purpose registers (a0-a3) and these values will
 * call the appropriate SYSCALL (1-8)
 * */
void syscall(){

    /*local variables*/
    int sysNum;
    state_t *procState;
    /*int i; counter for copying states -> this is unused*/ 
    /*end of local variables*/

    procState = (state_t *)BIOSDATAPAGE;
    sysNum = procState->s_a0;

    /*are we in kernel mode?*/
    if (sysNum >= 1 && sysNum <= 8 && (procState->s_status & 0x00000008) == 1){/*doesnt like the = 1 here because bitwise comparison
    always = false for some reason?? */

        /*if the program is not in kernel, then make the cause a not privileged instruction*/
        procState->s_cause = (procState->s_cause & 0xFFFFF00) | (10<<2); 
        programTrap(); /*doesnt like the program trap declaration here*/

    }

    copyState(procState, &(currentProc->p_s)); /*this could possibly be loadState instead??*/

    /*we are in kernel mode*/    
    currentProc -> p_s.s_pc +=4;
    
    if (sysNum == 1){
        int returnInt;
        returnInt = sys_1(); /*doesnt like this declaration this is same for all the other sys*/
        currentProc->p_s.s_v1 = returnInt;
        loadState(currentProc);
    }

    if (sysNum == 2){
        sys_2(currentProc);

        scheduleNext(); /* don't return control after a terminate*/
    }

    if (sysNum == 3){
        sys_3();
    }

    if (sysNum == 4){
        sys_4();
        loadState(currentProc);
    }

    if (sysNum == 5){
        int IOStatus = sys_5();
        scheduleNext(); /*we don't return control after a block*/
    }

    if (sysNum == 6){
        cpu_t timeReturn = sys_6();
        currentProc->p_s.s_v0 = timeReturn;
        loadState(currentProc);
    }

    if (sysNum == 7){
        sys_7();
    }

    if (sysNum == 8){
        int info = sys_8();
        currentProc->p_s.s_v0 = info;
        loadState(currentProc);
    }

    passUpOrDie(GENERALEXCEPT);/*doesnt like this declaration */
}


/**
 * When request this service creates a new process
 * */
int sys_1(){
    
    /*local variables*/
    pcb_t *newPcb; 
    support_t *supportData;
    /*end of local variables*/

    newPcb = allocPcb();

    if(newPcb == NULL){
        return(FAILED); /*put thiis in v0 */
    }

    processCnt ++;
    copyState(&(currentProc->p_s), &(newPcb->p_s)); /*copying states from parent to child*/
    supportData = (support_t *) currentProc->p_s.s_a2;

    if(supportData != NULL || supportData !=0){
        newPcb -> p_supportStruct = supportData; 
    }

    insertProcQ(&readyQ, newPcb);
    insertChild(&currentProc, newPcb); /* doesnt like the & but when I tried to do pcb_t **
    like it suggested it dropped an error...*/

    /*time is set in pcb.c*/
    return (OK); /*put this in v0*/

}


/**
 * When requested this service causes the executing process 
 * to cease to exist.
 * */
void sys_2(pcb_t *runningProc){ /*it saids this is a conflicting type hmmm*/

    /*local variables*/
    pcb_t *blockedChild;
    int *semNum; /*saids its not used but it is?*/
    /*end of local variables*/

    /*As long as there are kids, remove them, till we get the the last one */
    while(!(emptyChild(runningProc))){
        sys_2(removeChild(runningProc));
    }

    if(runningProc = currentProc){/*the running one is currentProc -> it suggests () here?? */
        removeChild(runningProc);
    }

    /*if it is on the Q*/
    else if (runningProc->p_semAdd == NULL){
        outProcQ(&readyQ, runningProc);
    }

    /* if it is blocked*/
    else{
        blockedChild = outBlocked(runningProc);

        if(blockedChild != NULL){
            semNum = blockedChild->p_semAdd;

            /*update softblock count or v the semNum*/
        }
    }

    /*once the are pulled off ready Q/unblocked, free them*/
    freePcb(runningProc);
    processCnt --;

}

/**
 * When requested this service tells the Nucleus
 * to perform a P operation on the semaphore.
 * */
void sys_3(){

    /*local variables*/
    int *semAddr;
    cpu_t endTime;
    /*end of local variables*/

    semAddr = (int *)currentProc -> p_s.s_a1; /* */
    /*update CPU for current proc*/
    *semAddr --; /*value computed is not used*/

    if(*semAddr < 0){
        endTime = timeCalc(endTime); /*doesnt like the declaration of timeCalc here*/
        /*endTime’ may be used uninitialized in this function, same in sys5, sys7 and current time in sys6*/
        currentProc->p_time = endTime;
        insertBlocked(&semAddr, currentProc);
        currentProc = NULL;
        scheduleNext();
    }

    loadState(currentProc);

}


/**
 * When requested this service tells the Nucleus
 * to perform a V operation on the semaphore.
 * */
void sys_4(){
    
    /*local variables*/
    int *semAddr;
    pcb_t *removedPcb;
    /*end of local variables*/

    semAddr = (int *) currentProc ->p_s.s_a1;
    semAddr++;

    if(semAddr <= 0){
        removedPcb = removeBlocked(semAddr);
        insertProcQ(&readyQ, removedPcb);
        return;
    }

}

/**
 * When requested, this serivce is used to transition the 
 * Current Process from the “running” state to a “blocked”state.
 * */
int sys_5(){

    /*local variables*/
    int lineNum;
    int deviceNum;
    cpu_t endTime;
    /*end of local variables*/
    
    /* find the line num and device num*/
    lineNum = currentProc->p_s.s_a1;
    deviceNum = currentProc ->p_s.s_a2;

    deviceNum += ((lineNum-DISKINT)*DEVPERINT); /*find which device we in*/

    /*if the interrupt is on line 7, then correct deviceNum*/
    if((deviceNum == TERMINT) && (currentProc->p_s.s_a3)){
        deviceNum = deviceNum + DEVPERINT; 
    } 

    devSema4[deviceNum] -=1;
    
    /*no interrupt happened, so block process and move on*/
    if(devSema4[deviceNum]<0){
        softBlockCnt +=1;
        endTime = timeCalc(endTime);
        currentProc->p_time = endTime;
        insertBlocked(&deviceNum, currentProc);
        currentProc = NULL;
        scheduleNext();
    }
    
    /*so interrupt happened and ACK-ed, so load savedState and return*/
    else{
        currentProc->p_s.s_v0 = saveState[deviceNum];
        scheduleNext();
    }
} /*"control reaches end of non-void function" what? */


/**
 * When requested, this service requests the Nucleus records 
 * (in the pcb: p time) the amount of processor time used by each process.
 * */
cpu_t sys_6(){

    cpu_t currentTime; /*local variable*/

    currentTime= timeCalc(currentTime);
    
    return currentTime;
}

/**
 * This service  service performs a P operation 
 * on the Nucleus maintained Pseudo-clock semaphore.
 * */
void sys_7(){

    cpu_t endTime; /*local variable*/

    devSema4[DEVCNT+DEVPERINT] -= 1;
    
    /*insert comment here*/
    if(devSema4[DEVCNT+DEVPERINT]<0){
        softBlockCnt++;
        endTime = timeCalc(endTime);
        currentProc->p_time = currentProc->p_time + endTime;
        insertBlocked(&(devSema4[DEVCNT+DEVPERINT]), currentProc); /*wait on clock semaphore*/
        scheduleNext();
    }

    loadState(currentProc);
    
}

/**
 * This service requests a pointer to the Current Process’s Support Structure. 
 * Hence,this service returns the value of p supportStruct from the Current Process’s pcb.
 * If there is no value, it returns NULL.
 * */
int sys_8(){

    /*insert comment here*/
    if(currentProc -> p_supportStruct == NULL){
        return ((support_t *) NULL); /*it doesnt like this because of this wack words:
       returning ‘support_t *’ {aka ‘struct support_t *’} 
       from a function with return type ‘int’ makes integer from pointer without a cast */
    }

    return (currentProc -> p_supportStruct); /*same reason for this one*/

}

/**
 * This method will handle program traps by derring it 
 * to the support level or killing it. (calling passUpOrDie)
 * */
void programTrap(){ /*conflicting types*/

    passUpOrDie(GENERALEXCEPT);

}

/**
 * This method passes the memory management issue to passUpOrDie.
 * */
void TLBExceptHandler(){

    passUpOrDie(PGFAULTEXCEPT);

}

/**
 * This method will take all SYSCALLs 9 and above
 * and will either "pass up" to phase 3 if the currentProc's p_supportStruct
 * is not NULL. Otherwise it will call SYS2 and terminate it. ("die")
 * */
void passUpOrDie(int exceptNum){ /*conflicting types*/

    /*insert comment here*/
    if (currentProc->p_supportStruct == NULL){
        SYS2(currentProc);
        scheduleNext();
    }

    copyState((state_t *)BIOSDATAPAGE, &(currentProc->p_supportStruct->sup_exceptState[exceptNum]));
    /*conflicitng types for copyState*/
    LDCXT(currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_stackPtr,
    currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_status,
    currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_pc);

    
}

/*********************************** HELPER METHODS *********************************/

/** 
 * Method for copying the states of one entry into the other
 * */
void copyState(state_t *source, state_t *copy){

    int i; /*local variable*/

    /*insert comment here*/
    for(i = 0; i<STATEREGNUM; i++){
        copy->s_reg[i] = source->s_reg[i];
    }

    copy->s_cause = source->s_cause;
    copy->s_entryHI = source->s_entryHI;
    copy->s_status = source->s_status;
    copy->s_pc = source->s_pc;

}

/**
 * This method to finds the total time used
 * */
cpu_t timeCalc(cpu_t time){

    cpu_t totalTime; /*local variable*/

    STCK(time);
    totalTime = currentProc->p_time + (time-startTime);

    return totalTime;
}
