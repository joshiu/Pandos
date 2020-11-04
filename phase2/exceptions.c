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

    /*local variables*/
    int sysNum;
    
    /*end of local variables*/

    sysNum = ((state_t *)BIOSDATAPAGE)->s_a0;

    /*are we in kernel mode?*/
    if (sysNum >= 1 && sysNum <= 8 && (((state_t *)BIOSDATAPAGE)->s_status & 0x00000008) == 1)
    { /*doesnt like the = 1 here because bitwise comparison
    always = false for some reason?? */
        debug(101);
        /*if the program is not in kernel, then make the cause a not privileged instruction*/
        ((state_t *)BIOSDATAPAGE)->s_cause = (((state_t *)BIOSDATAPAGE)->s_cause & 0xFFFFF00) | (10 << 2);
        programTrap(); /*doesnt like the program trap declaration here*/
    }

    debug(102);
    copyState(((state_t *)BIOSDATAPAGE), &(currentProc->p_s)); 

    /*we are in kernel mode*/
    currentProc->p_s.s_pc += 4;

    /*checks to see the approiate sys call it should go to*/
    if (sysNum == 1)
    {
        debug(1011);
        int returnInt;
        returnInt = sys_1(); /*doesnt like this declaration this is same for all the other sys*/
        currentProc->p_s.s_v0 = returnInt;

        loadState(currentProc);
    }

    if (sysNum == 2)
    {
        debug(1012);
        sys_2(currentProc);

        scheduleNext(); /* don't return control after a terminate*/
    }

    if (sysNum == 3)
    {
        debug(1013);
        sys_3();
    }

    if (sysNum == 4)
    {
        debug(1014);
        debug(((state_t *)BIOSDATAPAGE)->s_a1);
        sys_4();
    }

    if (sysNum == 5)
    {
        debug(1015);
        sys_5();
    }

    if (sysNum == 6)
    {
        debug(1016);
        sys_6();
    }

    if (sysNum == 7)
    {
        debug(1017);
        sys_7();
    }

    if (sysNum == 8)
    {
        debug(1018);
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
    debug(10111);

    /*if new PCB is null, it failed*/
    if (newPcb == NULL)
    {
        debug(10112);
        return FAILED; /*put -1 in v0 when we can't make a process*/
    }

    processCnt+=1;
    allData = (state_t *)currentProc->p_s.s_a1;
    
    copyState(allData, &(newPcb->p_s)); /*copying states from parent to child*/
    supportData = (support_t *)currentProc->p_s.s_a2;
    debug(10113);

    /*if the support data is no null or not o then put newPCB on the support data*/
    if (!((supportData == NULL) || (supportData == 0)))
    {
        debug(101135);
        newPcb->p_supportStruct = supportData;
    }
 

    debug(10114);
    debug(readyQ);
    debug((headProcQ(readyQ)));

    insertProcQ(&readyQ, newPcb);
    insertChild(currentProc, newPcb); 
    debug(10115);

    debug(processCnt);

    return OK; /*put 0 in v0 when we make a process*/

}

/**
 * When requested this service causes the executing process 
 * to cease to exist.
 * */
void sys_2(pcb_t *runningProc)
{ /*it saids this is a conflicting type hmmm*/
    debug(10121);

    /* how to kll kids*/
    /*local variables*/
    pcb_t *blockedChild;
    int *semNum; /*saids its not used but it is?*/
    /*end of local variables*/

    /*As long as there are kids, remove them, till we get the the last one */
    while (!(emptyChild(runningProc)))
    {
        debug(10122);
        sys_2(removeChild(runningProc));
    }

    /*if the running is the currentProc, remove it*/
    if (runningProc == currentProc)
    { 
        debug(10125);
        outChild(runningProc);/*detach any relations and leave*/
    }

    /*if it is on the Q*/
    else if (runningProc->p_semAdd == NULL)
    {
        debug(10123);
        outProcQ(&readyQ, runningProc);
    }

    /* if it is blocked*/
    else
    {
        blockedChild = outBlocked(runningProc);

        if (blockedChild != NULL)
        {
            debug(12124);
            semNum = blockedChild->p_semAdd;

            if(semNum>= &devSema4[0] && semNum <= &devSema4[DEVCNT+DEVPERINT]){
                softBlockCnt-=1;
            }

            else{
                *semNum+=1;
            }
        }
    }

    debug(10126);

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

    debug(10131);
    debug(currentProc->p_semAdd);

    semAddr = (int *)currentProc->p_s.s_a1; 
    debug(*semAddr);

    /*update CPU for current proc*/
    *semAddr -= 1; /*value computed is not used*/
    debug(*semAddr);
    debug(10132);

    /*if semAddress is less than 0 then do P operation (i think)*/
    if (*semAddr < 0)
    {
        debug(10133);
        STCK(endTime);
        currentProc->p_time += (endTime-startTime);
        
        debug(currentProc->p_semAdd);
        insertBlocked(semAddr, currentProc);
        
        debug(currentProc->p_semAdd);
        currentProc = NULL;
        
        debug(10134);
        scheduleNext();
    }
    else{

        debug(10135);
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

    debug(10141);

    debug(((state_t *)BIOSDATAPAGE)->s_a1);

    debug(currentProc->p_s.s_a1);

    semAddr = (int *)currentProc->p_s.s_a1;
    debug(*semAddr);
    
    *semAddr += 1;
    debug(*semAddr);

    /* if semaddress is less than or equal to 0 do the V operation*/
    if (*semAddr <= 0)
    {
        debug(10142);
        removedPcb = removeBlocked(semAddr);
        debug(removedPcb->p_semAdd);

        /*make sure what we insert exists*/
        if(removedPcb != NULL){
            insertProcQ(&readyQ, removedPcb);
            debug(10143);
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
    debug(10151);
    debug(devSema4[deviceNum]);

    /*if the interrupt is on line 7 and we are reading, then correct deviceNum*/
    if ((deviceNum == TERMINT) && (currentProc->p_s.s_a3 == TRUE))
    {
        debug(10152);
        deviceNum += DEVPERINT;
        debug(10153);
    }

    devSema4[deviceNum]-=1;
    debug(devSema4[deviceNum]);
    debug(10154);


    /*block process and move on, since we have not blocked anything*/
    if (devSema4[deviceNum] < 0)
    {
        debug(10155);
        softBlockCnt+=1;

        STCK(endTime);
        currentProc->p_time += (endTime-startTime);

        insertBlocked(&(devSema4[deviceNum]), currentProc);
        currentProc = NULL;

        debug(10156);

        scheduleNext(); /*we don't return control after a block*/
    }

    /*so interrupt happened and ACK-ed, so load savedState and return*/

        debug(10157);
        currentProc->p_s.s_v0 = saveState[deviceNum];
        debug(10158);

        loadState(currentProc);

} 

/**
 * When requested, this service requests the Nucleus records 
 * (in the pcb: p time) the amount of processor time used by each process.
 * */
void sys_6()
{

    cpu_t currentTime; /*local variable*/
    cpu_t endTime;

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
    debug(10171);
    cpu_t endTime; /*local variable*/

    debug(devSema4[DEVCNT+DEVPERINT]);

    devSema4[DEVCNT + DEVPERINT] -= 1;

    debug(devSema4[DEVCNT+DEVPERINT]);

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
        return ((support_t *)NULL); /*it doesnt like this because of this wack words:
       returning ‘support_t *’ {aka ‘struct support_t *’} 
       from a function with return type ‘int’ makes integer from pointer without a cast */
    }

    return (currentProc->p_supportStruct); /*same reason for this one*/
}

/**
 * This method will handle program traps by derring it 
 * to the support level or killing it. (calling passUpOrDie)
 * */
void programTrap()
{ /*conflicting types*/

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
{ /*conflicting types*/

    /*if current process on support struct is NULL, sys 2 on currentproc (i think)*/
    if (currentProc->p_supportStruct == NULL)
    {
        sys_2(currentProc);
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
