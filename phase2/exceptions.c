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
void SYSCALL(){
    int sysNum;
    state_t *procState;
    cpu_t currentTime;
    int i; /*counter for copying states*/

    procState = (state_t *)BIOSDATAPAGE;
    sysNum = procState->s_a0;

    /*are we in kernel mode?*/
    if (sysNum >= 1 && sysNum <= 8 && (procState->s_status & 0x00000008) == 1){
        /*if the program is not in kernel, then make the cause a not privileged instruction*/
        procState->s_cause = (procState->s_cause & 0xFFFFF00) | (10<<2); /* what are we doing here?*/
        programTrap();
    }

    copyState(procState, &(currentProc->p_s));    

    /*we are in kernel mode*/    
    currentProc -> p_s.s_pc = currentProc->p_s.s_pc +4;
    
    if (sysNum == 1)
    {
        int returnInt = SYS1();
        currentProc->p_s.s_v1 = returnInt;
        loadState(currentProc);
    }
    if (sysNum == 2)
    {
        SYS2(currentProc);

        scheduleNext(); /* don't return control after a terminate*/
    }
    if (sysNum == 3)
    {
        SYS3();
    }
    if (sysNum == 4)
    {
        SYS4();
        loadState(currentProc);
    }
    if (sysNum == 5)
    {
        int IOStatus = SYS5();
        scheduleNext(); /*we don't return control after a block*/
    }
    if (sysNum == 6)
    {
        cpu_t timeReturn = SYS6();
        currentProc->p_s.s_v0 = timeReturn;
        loadState(currentProc);
    }
    if (sysNum == 7)
    {
        SYS7();
    }
    if (sysNum == 8)
    {
        int info = SYS8();
        currentProc->p_s.s_v0 = info;
        loadState(currentProc);
    }

    passUpOrDie(GENERALEXCEPT);
}


/**
 * When request this service creates a new process
 * */
int SYS1(){
    int i;
    pcb_t *newPcb; 
    support_t *supportData;

    newPcb = allocPcb();

    if(newPcb == NULL){
        return(-1); /*put thiis in v0 */
    }
    processCnt ++;

    copyState(&(currentProc->p_s), &(newPcb->p_s)); /*copying states from parent to child*/

    supportData = (support_t *) currentProc->p_s.s_a2;

    if(supportData != NULL || supportData !=0){
        newPcb -> p_supportStruct = supportData; 
    }

    insertProcQ(&readyQ, newPcb);
    insertChild(&currentProc, newPcb);
    /*time is set in pcb.c*/
    return 0; /*put this in v0*/
}


/**
 * When requested this service causes the executing process 
 * to cease to exist.
 * */
void SYS2(pcb_t *runningProc){
    pcb_t *blockedChild;
    int *semNum;

    /*As long as there are kids, remove them, till we get the the last one */
    while(!(emptyChild(runningProc))){
        SYS2(removeChild(runningProc));
    }

    if(runningProc = currentProc){/*the running one is currentProc*/
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
void SYS3 (){
    int *semAddr;
    cpu_t endTime;

    semAddr = (int *)currentProc -> p_s.s_a1; /* */
    /*update CPU for current proc*/
    *semAddr --;

    if(*semAddr < 0){
        endTime = adjustTime(endTime);
        currentProc->p_time = endTime;
        insertBlocked(&semAddr, currentProc);
        currentProc = NULL;
        scheduleNext();
        return;
    }
    loadState(currentProc);
}


/**
 * When requested this service tells the Nucleus
 * to perform a V operation on the semaphore.
 * */
void SYS4(){

    int *semAddr;
    pcb_t *removedPcb;

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
int SYS5(){
    /* find the line num and device num*/
    /*convert device num to sema4 number*/
    /*if the interrupt is on line 7, then add dev num to devperint*/
    /*reduce number of devSem by 1*/
    /*if no interrupt, then softblock++, block, do time shit, and invoke scheduler*/
    /*if it has occur, then load dev # to v0 and ldst*/

    insertBlocked(& currentProc->p_semAdd, currentProc);
    return; /* what do we return? this is also incorrect so come back to later */
}

/**
 * When requested, this service requests the Nucleus records 
 * (in the pcb: p time) the amount of processor time used by each process.
 * */
cpu_t SYS6(){
    cpu_t currentTime;

    currentTime= timeCalc(currentTime);
    
    return currentTime;
}

/**
 * This service  service performs a P operation 
 * on the Nucleus maintained Pseudo-clock semaphore.
 * */
void SYS7(){
    cpu_t endTime;

    devSema4[DEVCNT+DEVPERINT] -= 1;
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
int SYS8(){
    if(currentProc -> p_supportStruct == NULL){
        return ((support_t *) NULL);
    }

    return (currentProc -> p_supportStruct);

}

/**
 * This method will handle program traps by derring it 
 * to the support level or killing it. (calling passUpOrDie)
 * */
void programTrap(){
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
void passUpOrDie(int exceptNum){
    if (currentProc->p_supportStruct == NULL)
    {
        SYS2(currentProc);
        /*use LDST*/
    }
}

/*********************************** HELPER METHODS *********************************/

/** Method for copying the states of one entry into the other*/
void copyState(state_t *source, state_t *copy){
    int i;

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
    cpu_t totalTime;
    STCK(time);
    totalTime = currentProc->p_time + (time-startTime);
    return totalTime;
}
