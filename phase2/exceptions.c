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
void syscall()
{

    int sysNum; /*local variable*/

    sysNum = ((state_t *)BIOSDATAPAGE)->s_a0;

    /*are we in kernel mode?*/
    if (sysNum >= 1 && sysNum <= 8 && (((state_t *)BIOSDATAPAGE)->s_status & 0x00000008) == 1)
    { 
        /*if the program is not in kernel, then make the cause a not privileged instruction*/
        ((state_t *)BIOSDATAPAGE)->s_cause = (((state_t *)BIOSDATAPAGE)->s_cause & 0xFFFFF00) | (10 << 2);
        programTrap(); 
    }

    copyState(((state_t *)BIOSDATAPAGE), &(currentProc->p_s)); 

    /*we are in kernel mode*/
    currentProc->p_s.s_pc += 4;

    /*checks to see the approiate sys call it should go to*/
    if (sysNum == 1)
    {
        int returnInt;
        returnInt = sys_1();
        currentProc->p_s.s_v0 = returnInt;

        loadState(currentProc);
    }

    if (sysNum == 2)
    {
        sys_2(currentProc);

        scheduleNext(); /* don't return control after a terminate*/
    }

    if (sysNum == 3)
    {
        sys_3();
    }

    if (sysNum == 4)
    {
        sys_4();
    }

    if (sysNum == 5)
    {
        sys_5();
    }

    if (sysNum == 6)
    {
        sys_6();
    }

    if (sysNum == 7)
    {
        sys_7();
    }

    if (sysNum == 8)
    {
        int info = sys_8();
        currentProc->p_s.s_v0 = info;
        loadState(currentProc);
    }

    passUpOrDie(GENERALEXCEPT);
}

/**
 * When request this service creates a new process
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

    processCnt+=1;
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
 * to cease to exist.
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
        outChild(runningProc);/*detach any relations and leave*/
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

            if(semNum>= &devSema4[0] && semNum <= &devSema4[DEVCNT+DEVPERINT]){
                softBlockCnt-=1;
            }

            else{
                *semNum+=1;
            }

        }
    }


    /*once the are pulled off ready Q/unblocked, free them*/
    freePcb(runningProc);
    processCnt-=1;

}

/**
 * When requested this service tells the Nucleus
 * to perform a P operation on the semaphore.
 * */
void sys_3()
{

    /*local variables*/
    int *semAddr;
    cpu_t endTime;
    /*end of local variables*/

    semAddr = (int *)currentProc->p_s.s_a1; 

    /*update CPU for current proc*/
    *semAddr -= 1; 

    /*if semAddress is less than 0 then do P operation (i think)*/
    if (*semAddr < 0)
    {
        STCK(endTime);
        currentProc->p_time += (endTime-startTime);
        
        insertBlocked(semAddr, currentProc);
        currentProc = NULL;
        
        scheduleNext();
    }
    else{

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
        if(removedPcb != NULL){
            insertProcQ(&readyQ, removedPcb);
        }
        
    }

    loadState(currentProc);
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
    cpu_t endTime;
    /*end of local variables*/

    /* find the line num and device num*/
    lineNum = currentProc->p_s.s_a1;
    deviceNum = currentProc->p_s.s_a2;

    deviceNum += ((lineNum - DISKINT) * DEVPERINT); /*find which device we in*/

    /*if the interrupt is on line 7 and we are reading, then correct deviceNum*/
    if ((deviceNum == TERMINT) && (currentProc->p_s.s_a3 == TRUE))
    {
        deviceNum += DEVPERINT;
    }

    devSema4[deviceNum]-=1;

    /*block process and move on, since we have not blocked anything*/
    if (devSema4[deviceNum] < 0)
    {
        softBlockCnt+=1;

        STCK(endTime);
        currentProc->p_time += (endTime-startTime);

        insertBlocked(&(devSema4[deviceNum]), currentProc);
        currentProc = NULL;

        scheduleNext(); /*we don't return control after a block*/
    }
    
    /*so interrupt happened and ACK-ed, so load savedState and return*/
        currentProc->p_s.s_v0 = saveState[deviceNum];

        loadState(currentProc);

} 

/**
 * When requested, this service requests the Nucleus records 
 * (in the pcb: p time) the amount of processor time used by each process.
 * */
void sys_6()
{
    /*local variables*/
    cpu_t currentTime;
    cpu_t endTime;
    /*end of local variables*/

    STCK(endTime);

    currentTime =currentProc->p_time+ (endTime-startTime);

    currentProc->p_s.s_v0 = currentTime;
    loadState(currentProc);
}

/**
 * This service  service performs a P operation 
 * on the Nucleus maintained Pseudo-clock semaphore.
 * */
void sys_7()
{
    cpu_t endTime; /*local variable*/

    devSema4[DEVCNT + DEVPERINT] -= 1;

    /*insert comment here -> still unsure what this does :(*/
    if (devSema4[DEVCNT + DEVPERINT] < 0)
    {
        softBlockCnt+=1;

        STCK(endTime);
        currentProc->p_time += (endTime-startTime);

        insertBlocked(&(devSema4[DEVCNT + DEVPERINT]), currentProc); /*wait on clock semaphore*/
        
        currentProc=NULL;
        scheduleNext();

    }
    
    /*if we don't block, then we load the state and continue*/
    loadState(currentProc);

}

/**
 * This service requests a pointer to the Current Process’s Support Structure. 
 * Hence,this service returns the value of p supportStruct from the Current Process’s pcb.
 * If there is no value, it returns NULL.
 * */
int sys_8()
{

    /*if current process on support Struct is Null retrun support_t (i think)*/
    if (currentProc->p_supportStruct == NULL)
    {
        return ((support_t *)NULL);
    }

    return (currentProc->p_supportStruct);
}

/**
 * This method will handle program traps by derring it 
 * to the support level or killing it. (calling passUpOrDie)
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

    /*if current process on support struct is NULL, sys 2 on currentproc (i think)*/
    if (currentProc->p_supportStruct == NULL)
    {
        sys_2(currentProc);
        scheduleNext();
    }

    copyState((state_t *)BIOSDATAPAGE, &(currentProc->p_supportStruct->sup_exceptState[exceptNum]));

    LDCXT(currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_stackPtr,
          currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_status,
          currentProc->p_supportStruct->sup_exceptContext[exceptNum].c_pc);
}

/*********************************** HELPER METHODS *********************************/

/** 
 * Method for copying the states of one entry into the other
 * */
void copyState(state_t *source, state_t *copy)
{

    int i; /*local variable*/

    /*insert comment here idk what this is doing honestly :(*/
    for (i = 0; i < STATEREGNUM; i+=1)
    {
        copy->s_reg[i] = source->s_reg[i];
    }

    copy->s_cause = source->s_cause;
    copy->s_entryHI = source->s_entryHI;
    copy->s_status = source->s_status;
    copy->s_pc = source->s_pc;
    
}
