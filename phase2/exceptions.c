#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/initial.h"
#include "../h/scheduler.h" 

/**
 * This file occurs when the SYSCALL assembly instruction is executed. 
 * It places the appropriate values into registers a0-a3 before executing 
 * the SYSCALL instruction and it will execute based on the value in a0.
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/**
 * Side notes from Amy:
 * pass up or die method goes in this file 
 * also include the trap programs here (that are needed for uTLB_RefillHandler) and so syscall
 * is the systrap program?
 * the ifs are fine but he may yell at us for not using causes (shrug) this way makes more sense to me
 * 
 * may need helper functions
 * */

    /*write the biosdatapage to the currentProc -> p_supportStruct->sup_exceptState[exceptNum]*/
    /*do a LDCXT() on 3 things:
  currentProc->p_supportStruct->sup_exceptState[exceptNum].c_stackPtr;
  currentProc->p_supportStruct->sup_exceptState[exceptNum].c_status;
  currentProc->p_supportStruct->sup_exceptState[exceptNum].c_pc;
  */

void SYSCALL(){ /*find out how to call a0*/
    int sysNum;
    state_t *procState;
    cpu_t currentTime;

    procState = (state_t *)BIOSDATAPAGE;
    sysNum = procState->s_a0;

    /*are we in kernel mode?*/
    if (sysNum >= 1 && sysNum <= 8 && (procState->s_status & 0x00000008) == 1){
        /*if the program is not in kernel, then make the cause a not privileged instruction*/
        procState->s_cause = (procState->s_cause & 0xFFFFF00) | (10<<2); /* what are we doing here?*/
        programTrap();
    }

    /*we are in kernel mode*/    
    currentProc -> p_s.s_pc = currentProc->p_s.s_pc +4;
    
    if (sysNum == 1)
    {
        int returnInt = SYS1();
        currentProc->p_s.s_v1 = returnInt;
        /*run load state*/
    }
    if (sysNum == 2)
    {
        SYS2(currentProc);

        scheduleNext(); /* don't return control after a terminate*/
    }
    if (sysNum == 3)
    {
        SYS3();
        scheduleNext(); /*we don't return control after a block*/
    }
    if (sysNum == 4)
    {
        SYS4();
        /*load state*/
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
        /*load state*/
    }
    if (sysNum == 7)
    {
        SYS7();
        scheduleNext(); /*we don't return control after a block*/
    }
    if (sysNum == 8)
    {
        int info = SYS8();
        currentProc->p_s.s_v0 = info;
        /*load state*/
    }

    passUpOrDie(GENERALEXCEPT);
}
/**
 * this is the situation where either kernel mode is not TRUE or a0 is not 1-8
 * Cause.ExcCode = RI;
 * programTrapExcept(GeneralExcept's index value);
 * return;
 **/

/**
 * 
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

    /*copying states to the child*/

    for(i = 0; i<STATEREGNUM; i++){
        newPcb->p_s.s_reg[i] = currentProc->p_s.s_reg[i];
    }
    newPcb->p_s.s_cause = currentProc->p_s.s_cause;
    newPcb->p_s.s_entryHI = currentProc->p_s.s_entryHI;
    newPcb->p_s.s_status = currentProc->p_s.s_status;
    newPcb->p_s.s_pc = currentProc->p_s.s_pc;

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
 * 
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
 * 
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
        return;
    }

    /* if we don't block*/
    /* LDST(currentProc);*/
}


/**
 * 
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

    /* if we don't remove*/
    /*LDST(currentProc);*/

}

/**
 * 
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
 * 
 * */
cpu_t SYS6(){
    cpu_t currentTime;

    currentTime= timeCalc(currentTime);
    
    return currentTime;
}

/**
 * 
 * */
void SYS7(){
    /*need to preform P opertation on psuedoclock semaphore; (SYS3)*/
    insertBlocked(clockSem, currentProc); /*wait on clock semaphore*/
}

/**
 * 
 * */
int SYS8(){
    if(currentProc -> p_supportStruct == NULL){
        return ((support_t *) NULL);
    }

    return (currentProc -> p_supportStruct);

}

/**
 * Method to find the total time used
 * */

cpu_t timeCalc(cpu_t time){
    cpu_t totalTime;
    STCK(time);
    totalTime = currentProc->p_time + (time-startTime);
    return totalTime;
}

void programTrap(){
    /* this will handle program traps by derring it to support level or killing it*/
    passUpOrDie(GENERALEXCEPT);
}

void TLBExceptHandler(){
    passUpOrDie(PGFAULTEXCEPT);
}

void passUpOrDie(int exceptNum){
    if (currentProc->p_supportStruct == NULL)
    {
        SYS2(currentProc);
        /*use LDST*/
    }
}