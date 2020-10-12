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
        sys2(currentProc);
        /*use LDST*/
    }
    /*write the biosdatapage to the currentProc -> p_supportStruct->sup_exceptState[exceptNum]*/
    /*do a LDCXT() on 3 things:
  currentProc->p_supportStruct->sup_exceptState[exceptNum].c_stackPtr;
  currentProc->p_supportStruct->sup_exceptState[exceptNum].c_status;
  currentProc->p_supportStruct->sup_exceptState[exceptNum].c_pc;
  */
}

void SYSCALL(){ /*find out how to call a0*/
    int sysNum;
    state_t *procState;
    cpu_t currentTOD;

    procState = (state_t *)BIOSDATAPAGE;
    sysNum = procState->s_a0;

    /*are we in kernel mode?*/
    if (sysNum >= 1 && sysNum <= 8 && (procState->s_status & 0x00000008) == 1){
        procState->s_cause = (procState->s_cause & 0xFFFFF00) | (10<<2); /* what are we doing here?*/
        programTrap();
    }

    /*we are in kernel mode*/
    pcb_t *currentProc;
    
    currentProc -> p_s.s_pc = currentProc->p_s.s_pc +4;
    
    if (sysNum == 1)
    {
        int returnInt = SYS1(a0, statep, supportp, i);
        pcbUsingSYSCALL->s_v1 = returnInt;
    }
    if (sysNum == 2)
    {
        SYS2(a0, a1, a2, a3);
        return; /* don't return control after a terminate*/
    }
    if (a0 == 3)
    {
        SYS3(a0, a1->semAdd, a2, a3);
        return; /*we don't return control after a block*/
    }
    if (a0 == 4)
    {
        SYS4(a0, a1->semAdd, a2, a3);
    }
    if (a0 == 5)
    {
        int IOStatus = SYS5(a0, a1->semAdd, a2, a3);
        return; /*we don't return control after a block*/
    }
    if (a0 == 6)
    {
        cpu_t timeReturn = SYS6(a0, a1, a2, a3);
        pbcUsingSYSCALL->p_time = timeReturn;
    }
    if (a0 == 7)
    {
        SYS7(a0, a1->semAdd, a2, a3);
        return; /*we don't return control after a block*/
    }
    if (a0 == 8)
    {
        support_t *info = SYS8(a0, a1, a2, a3);
    }
}
// /* this is the situation where either kernel mode is not TRUE or a0 is not 1-8*/
//     Cause.ExcCode = RI;
//     programTrapExcept(GeneralExcept's index value);
//     return;
// }

/**
 * int SYS1(a0, state_t *statep, support_t *supportp, int i){
 * pcb_t *newPcb = allocPcb();
 * processcnt ++;
 * if(newPcb == NULL){
 * return (-1); (put this in v0)
 * }
 * newPcb -> p_s = a1;
 * newPcb -> p_supportStruct = a2 (NULL if there is nothing);
 * insertProcQ(&readyQ, newPcb);
 * insertChild(&currentProc, newPcb);
 * p_time =0
 * return 0; (put this in v0)
 * }
 * */

/**
 * void SYS2(a0, int i, int j, int k){
 * pcb_PTR lastChild = currentProc;
 * 
 * if you have no kids
 * 
 * if(emptyChild(lastChild)){
 * lastChild ->p_prnt -> p_child =NULL;
 * lastChild -> p_prnt = NULL;
 * scheduler();
 * return;
 * }
 * 
 * if there are children
 * 
 * find the last descendent with children
 * 
 * while(lastChild->p_child->p_child != NULL){
 *  lastChild = lastChild ->p_child;
 * }
 * 
 * remove all the children
 * 
 * while(!emptyChild(lastChild)){
 *  removeChild(lastChild);
 * }
 * 
 * repeat until no children left
 * SYSCALL2(a0,0,0,0);
 * }
 * */

/**
 * void SYS3 (a0, int *semaddr, int i, int j){
 * currentProc->p_s = saved proc state;
 * update CPU time for current proc
 * semaddr --;
 * if (semaddr <0){
 * insertBlocked(&semaddr, currentProc);
 * scheduler();
 * return;
 * }
 * LDST(currentProc);
 * return;
 * }
 * */

/**
 * void SYS4(a0, int *semaddr,int i ,int j){
 * semaddr++;
 * if(semaddr <= 0){
 * pcb_t *temp = removeBlocked(&semaddr);
 * insertProcQ(&readyQ, temp);
 * return;
 * }
 * LDST(currentProc);
 * }
 * */

/**
 * int SYS5(a0, int int1No, int dnum, int waitForTermRead){
 * insertBlocked(& currentProc->p_semAdd, currentProc);
 * scheduler();
 * what do we return?
 * this is incorrect, talk to Mikey Monday
 * }
 * */

/**
 * cpu_t SYS6(a0, int i, int j, int k){
 * cpu_t sumOfTime = currentProc->p_time + amnt of time used in current quantum; 
 * return sumOfTime
 * }
 * */

/**
 * void SYS7(a0, int i, int j, int k){
 * need to preform P opertation on psuedoclock semaphore; (SYS3)
 * insertBlocked(& currentProc->p_semAdd, currentProc);
 * scheduler();
 * }
 * */

/**
 * support_t SYS8(a0, int i, int j, int k){
 * if(currentProc -> p_supportStruct == NULL){
 * return NULL;
 * }
 * return currentProc -> p_supportStruct;
 * }
 * 
 * */

/**
 * performs standard pass up or die using the general exception index value
 * void programTrapExcept(int index){
 *      passUpOrDie(index);
 * }
 * */

/* idk where to put this, so it's going here!*/

/*
 * Else:
 * two tasks: copy and save the exception state into a location accessible 
 * pass control to a routine specified by the Support Level
 * 
 * Support level: two locations for saved exception states, two addresses for handlers.
 * One state t/PC address pair for both TLB exceptions & one for all other exceptions
 * 
 * To pass up the handling of an exception:
 * Copy the saved exception state from the BIOS 
 * Give to the correct sup exceptState field of the Current Process.
 * 
 * Important: The Current Process’s pcb should point to a non-null support t.
 * 
 * Perform a LDCXT using the fields from the correct sup exceptContext field of the Current Process
 * }
 * */