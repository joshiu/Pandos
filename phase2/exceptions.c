#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/initial.h"
#include "../h/scheduler.h"
#include "../h/initial.h"

/**
 * This file occurs when a syscall, TLBexception, or programTrap is executed. 
 * All appropriate values are placed in registers a0-a3 before executing 
 * the SYSCALL instruction and will execute based on the value in a0.
 * Program traps and TLBexcepts are independent of the value in a0
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/*********************FILE SPECIFIC METHODS******************************/

extern void passUpOrDie(int exceptNum);
extern void programTrap();
extern void copyState(state_t *source, state_t *copy);

HIDDEN int sys_1();
HIDDEN void sys_2(pcb_t *runningProc);
HIDDEN void sys_3();
HIDDEN void sys_4();
HIDDEN void sys_5();
HIDDEN void sys_6();
HIDDEN void sys_7();
HIDDEN support_t * sys_8();
HIDDEN void blockAndTime(int *address);
HIDDEN cpu_t calcTime();

/*end of file specific methods*/

/************************************BEGIN METHOD DECLARATION************************************/

/**
 * This System Call (syscall) method occurs when the SYSCALL assembly 
 * intruction is executed. This places the appropriate values in 
 * the general purpose registers (a0-a3) and these values will
 * call the appropriate SYSCALL (1-8)
 * */
void syscall()
{

    int sysNum; /*local variable*/

    sysNum = ((state_t *)BIOSDATAPAGE)->s_a0;

    /*check if we are in kernel mode*/
    if (sysNum >= 1 && sysNum <= 8 && (((state_t *)BIOSDATAPAGE)->s_status & USERPREVON) == 1)
    {
        /*if the program is not in kernel, then make cause a not privileged instruction*/

        ((state_t *)BIOSDATAPAGE)->s_cause =
            (((state_t *)BIOSDATAPAGE)->s_cause & CLEARCAUSE) | (NOTPRIVINSTRUCT << SHIFTCAUSE);

        programTrap();
    }

    /*we are in kernel mode*/

    /*store the states in currentProc*/
    copyState(((state_t *)BIOSDATAPAGE), &(currentProc->p_s));

    currentProc->p_s.s_pc += 4; /*update stack pointer to prevent looping*/

    /*checks to see the approiate sys call it should go to*/
    if (sysNum == MAKEPROCESS)
    {
        int returnInt;
        returnInt = sys_1();
        currentProc->p_s.s_v0 = returnInt;

        loadState(currentProc);
    }

    if (sysNum == KILLPROCESS)
    {
        sys_2(currentProc);

        scheduleNext(); /* don't return control after a terminate*/
    }

    if (sysNum == PASSERN)
    {
        sys_3();
    }

    if (sysNum == VERHOGEN)
    {
        sys_4();
        loadState(currentProc);
    }

    if (sysNum == WAITAWHILE)
    {
        sys_5();
        loadState(currentProc);
    }

    if (sysNum == GETCLOCK)
    {
        sys_6();
        loadState(currentProc);
    }

    if (sysNum == CLOCKSEMA4)
    {
        sys_7();
        loadState(currentProc);
    }

    if (sysNum == SUPPORTDATA)
    {
        support_t *info = sys_8();
        currentProc->p_s.s_v0 = info;
        loadState(currentProc);
    }

    /*if none of the above, then passup*/
    passUpOrDie(GENERALEXCEPT);
}

/**
 * When requested this service creates a new process
 * that is a child of the currentprocess. Inserts the
 * new process into the readyQ and the parent's process tree. 
 * */
int sys_1()
{

    /*local variables*/
    pcb_t *newPcb;
    state_t *allData;
    support_t *supportData;
    /*end of local variables*/

    newPcb = allocPcb();

    /*if new PCB is null, it failed*/
    if (newPcb == NULL)
    {
        return FAILED; /*put -1 in v0 when we can't make a process*/
    }

    processCnt += 1;
    allData = (state_t *)currentProc->p_s.s_a1;

    copyState(allData, &(newPcb->p_s)); /*copying states from parent to child*/
    supportData = (support_t *)currentProc->p_s.s_a2;

    /*if the support data is no null or not o then put newPCB on the support data*/
    if (!((supportData == NULL) || (supportData == 0)))
    {
        newPcb->p_supportStruct = supportData;
    }

    insertProcQ(&readyQ, newPcb);
    insertChild(currentProc, newPcb);
    return OK; /*put 0 in v0 when we make a process*/
}

/**
 * When requested this service causes the executing process 
 * and its children to be wiped from existence.
 * */
void sys_2(pcb_t *runningProc)
{
    /*local variables*/
    pcb_t *blockedChild;
    int *semNum;
    /*end of local variables*/

    /*As long as there are kids, remove them, till we get the the last one */
    while (!(emptyChild(runningProc)))
    {
        sys_2(removeChild(runningProc));
    }

    /*if the running is the currentProc, remove it*/
    if (runningProc == currentProc)
    {
        outChild(runningProc); /*detach any relations and leave*/
    }

    /*if it is on the Q*/
    else if (runningProc->p_semAdd == NULL)
    {
        outProcQ(&readyQ, runningProc);
    }

    /* if it is blocked*/
    else
    {
        blockedChild = outBlocked(runningProc);

        if (blockedChild != NULL)
        {
            semNum = blockedChild->p_semAdd;

            /*if the semNum is somewhere between the first deSema4 and clock sema4*/
            if (semNum >= &devSema4[0] && semNum <= &devSema4[DEVCNT + DEVPERINT])
            {
                /*process was blocked and we removed it*/
                softBlockCnt -= 1;
            }

            else
            {
                /*if process wasn't blocked*/
                *semNum += 1;
            }
        }
    }

    /*once the are pulled off ready Q/unblocked, free them*/
    freePcb(runningProc);
    processCnt -= 1;

}

/**
 * When requested this service tells the Nucleus
 * to perform a P operation on the semaphore.
 * */
void sys_3()
{

    /*local variables*/
    int *semAddr;
    /*end of local variables*/

    semAddr = (int *)currentProc->p_s.s_a1;

    /*update CPU for current proc*/
    *semAddr -= 1;

    /*if semAddress is less than 0 then do P operation*/
    if (*semAddr < 0)
    {
        blockAndTime(semAddr);
    }
    else
    {

        loadState(currentProc);
    }
}

/**
 * When requested this service tells the Nucleus
 * to perform a V operation on the semaphore.
 * */
void sys_4()
{

    /*local variables*/
    int *semAddr;
    pcb_t *removedPcb;
    /*end of local variables*/

    semAddr = (int *)currentProc->p_s.s_a1;
    *semAddr += 1;

    /* if semaddress is less than or equal to 0 do the V operation*/
    if (*semAddr <= 0)
    {
        removedPcb = removeBlocked(semAddr);

        /*make sure what we insert exists*/
        if (removedPcb != NULL)
        {
            insertProcQ(&readyQ, removedPcb);
        }
    }
}

/**
 * When requested, this serivce always transitions the 
 * Current Process from the “running” state to a “blocked”state, waiting 
 * for an interrupt to remove it. 
 * */
void sys_5()
{

    /*local variables*/
    int lineNum;
    int deviceNum;
    /*end of local variables*/

    /* find the line num and device num*/
    lineNum = currentProc->p_s.s_a1;
    deviceNum = currentProc->p_s.s_a2;

    deviceNum += ((lineNum - DISKINT) * DEVPERINT); /*find which device we're in*/

    /*if the interrupt is on line 7 and we are reading, then correct deviceNum*/
    if ((deviceNum == TERMINT) && (currentProc->p_s.s_a3 == TRUE))
    {
        deviceNum += DEVPERINT;
    }

    devSema4[deviceNum] -= 1;

    /*block process and move on, since we have not blocked anything*/
    if (devSema4[deviceNum] < 0)
    {
        softBlockCnt += 1;

        blockAndTime(&devSema4[deviceNum]);
    }

    /*interrupt happened and ACK-ed, so load savedState and return*/
    currentProc->p_s.s_v0 = saveState[deviceNum];
}

/**
 * When requested, this service gives the total amount 
 * of processor time used by the current process.
 * */
void sys_6()
{
     /*calculate the total time and set to v0*/

    currentProc->p_s.s_v0 = calcTime();
}

/**
 * This service  service performs a P operation 
 * on the Nucleus maintained Pseudo-clock semaphore.
 * */
void sys_7()
{
    /*decrement the address*/
    devSema4[DEVCNT + DEVPERINT] -= 1;

    /*increase softblock count and block the process on clock sema4*/
    if (devSema4[DEVCNT + DEVPERINT] < 0)
    {
        softBlockCnt += 1;

        blockAndTime(&devSema4[DEVCNT+DEVPERINT]);

    }

    /*if we don't block, then return to syscall*/
}

/**
 * This service procides a pointer to currenProc's Support Structure. 
 * Hence, this service returns the value of p_supportStruct 
 * from the Current Process’s pcb. If there is no value, it returns NULL.
 * */
support_t* sys_8()
{

    /*if current process' support Struct is not Null, return pointer */
    if (currentProc->p_supportStruct != NULL)
    {
        return (currentProc->p_supportStruct);
    }

    /*return null if nothing there*/
    return (NULL);

}

/**
 * This method will handle program traps by calling passUpOrDie.
 * */
void programTrap()
{

    passUpOrDie(GENERALEXCEPT);
}

/**
 * This method passes the memory management issue to passUpOrDie.
 * */
void TLBExceptHandler()
{

    passUpOrDie(PGFAULTEXCEPT);
}

/**
 * This method will take all SYSCALLs 9 and above
 * and will either "pass up" to phase 3 if the currentProc's p_supportStruct
 * is not NULL. Otherwise it will call SYS2 and terminate it. ("die")
 * */
void passUpOrDie(int exceptNum)
{
    /*start local variables*/
    unsigned int stackPtr;
    unsigned int status;
    unsigned int pc;
    /*end local variables*/

    

    /*if current process has support struct, passup*/
    if (currentProc->p_supportStruct != NULL)
    {
        copyState((state_t *)BIOSDATAPAGE, &(currentProc->p_supportStruct->sup_exceptState[exceptNum]));

        stackPtr = currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_stackPtr;
        status = currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_status;
        pc = currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_pc;

        LDCXT(stackPtr, status, pc);
    }
    
    /*otherwise die*/
    sys_2(currentProc);

    scheduleNext();
    
}

/*********************************** HELPER METHODS *********************************/

/** 
 * Method for copying the states in the source into the copy
 * */
void copyState(state_t *source, state_t *copy)
{

    int i; /*local variable*/

    /*cycle through all the states and copy them from source to copy*/
    for (i = 0; i < STATEREGNUM; i += 1)
    {
        copy->s_reg[i] = source->s_reg[i];
    }

    /*copy the last 4*/

    copy->s_cause = source->s_cause;
    copy->s_entryHI = source->s_entryHI;
    copy->s_status = source->s_status;
    copy->s_pc = source->s_pc;
}

/**
 * Method called to stop time keeping, record the time
 * and block the process to the given address. Then schedule next. 
 * */
void blockAndTime(int *address)
{
    /*time calculations*/
    currentProc->p_time = calcTime();

    /*block process*/
    insertBlocked(address, currentProc);
    currentProc = NULL;

    scheduleNext();
}

/** Method that calculates the total time used
 * by an process and returns the time
 * */
cpu_t calcTime(){
    cpu_t endtime;
    cpu_t totalTime;

    STCK(endtime);

    totalTime = currentProc->p_time + (endtime-startTime);

    return(totalTime);

    
}