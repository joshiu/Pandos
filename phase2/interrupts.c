#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/exceptions.h"
#include "../h/initial.h"
#include "../h/scheduler.h"

/**
 * This file occurs when either a previously initiated I/O request completes 
 * or when either a Processor Local Timer (PLT) or the Interval Timer 
 * makes a 0x0000.0000 ⇒ 0xFFFF.FFFF transition.
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/************ FILE SPECIFIC METHODS *********************/
HIDDEN void localTimerInterrupt(cpu_t time);
HIDDEN void pseudoClockInterrupt();
HIDDEN void deviceInterrupt(int deviceType);
HIDDEN int terminalInterrupt(int *deviceSema4Num);

/* end of file specific methods */

/**
 * This method is used to determine the appropriate action
 * when the timer generates an interrupt. Look at who interrupted 
 * me, look at Cause.ExcCode.
 *  if line 1, it's a local timer interrupt
 *  if line 2, it's pseudo clock interrupt
 *  if 3-7, then device interrupt 
 * */
void interruptHandler()
{

    /*local variables*/
    cpu_t stopTime;
    cpu_t quantumTimer;
    /*end of local variables*/

    STCK(stopTime);
    quantumTimer = getTIMER();

    if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00000200) != 0)
    {
        /*local timer interrupt*/
        localTimerInterrupt(stopTime);
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00000400) != 0)
    {
        /*timer interrupt*/
        pseudoClockInterrupt();
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00000800) != 0)
    {
        /*disk interrupt*/
        deviceInterrupt(DISKINT);
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00001000) != 0)
    {
        /*flash interrupt*/
        deviceInterrupt(FLASHINT);
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00004000) != 0)
    {
        /*print interrupt*/
        deviceInterrupt(PRNTINT);
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00008000) != 0)
    {
        /*terminal interrupt*/
        deviceInterrupt(TERMINT);
    }

    if (currentProc != NULL)
    {

        currentProc->p_time += (stopTime - startTime);
        copyState((state_t *)BIOSDATAPAGE, &(currentProc->p_s));

        setSpecificQuantum(quantumTimer);
    }

    else
    {

        /*if there is no current, then we have a problem!*/
        HALT();
    }

}

/**
 * The current process's time is up so this method checks if there
 * is a currrent process then it 
 * stops the clock and puts that process back on the readyQ.
**/
void localTimerInterrupt(cpu_t timeStop)
{

    if (currentProc == NULL)
    {
        PANIC();
    }

    currentProc->p_time += (timeStop-startTime);
    copyState((state_t *)BIOSDATAPAGE, &(currentProc->p_s));

    insertProcQ(&readyQ, currentProc);
    currentProc = NULL;

    scheduleNext();
}

/** 
 * This method is called when the pseudo clock
 * has finished and it sys4 everything that we sys7 on the clock.
 * It then resets clock to 0 and calls the scheduler and loads
 * the next process.
 * */
void pseudoClockInterrupt()
{

    pcb_t *removedPcbs; /*local variable*/

    LDIT(STANPSEUDOCLOCK);

    removedPcbs = removeBlocked(&(devSema4[DEVPERINT + DEVCNT]));

    while (removedPcbs != NULL)
    {
        insertProcQ(&readyQ, removedPcbs);
        softBlockCnt-=1;
        removedPcbs = removeBlocked(&(devSema4[DEVPERINT + DEVCNT]));
    }

    devSema4[DEVPERINT + DEVCNT] = 0;

    if (currentProc == NULL)
    {
        scheduleNext();
    }

}

/** 
 * This method looks at the which device is called (Bus register)
 * It then finds the bit number for the device. (with priority: 1 has highest
 * 7 has the lowest) It then saves the status code and acknowledges the interrupt
 * (Writes acknowledgement in the device register)
 * 
 * look at everything in relation to DISKINT 
 * (we make DISKINT our "0th" line and continue from there)
 * */
void deviceInterrupt(int lineNum)
{

    /*Local Variables*/
    int deviceNumber;
    int deviceSema4Num;
    unsigned int devStatus;
    unsigned int bitMap;
    volatile devregarea_t *deviceRegister;
    pcb_t *pseudoSys4;
    /* End of Local Variables*/

    deviceRegister = (devregarea_t *)RAMBASEADDR;
    bitMap = deviceRegister->interrupt_dev[(lineNum - DISKINT)];

    /*if the bitMap has nothing in it, something is wrong*/
    if (&(bitMap) == NULL)
    {
        PANIC();
    }

    if ((bitMap & 0x00000001) != 0)
    {
        deviceNumber = 0;
    }

    else if ((bitMap & 0x00000002) != 0)
    {
        deviceNumber = 1;
    }

    else if ((bitMap & 0x00000004) != 0)
    {
        deviceNumber = 2;
    }

    else if ((bitMap & 0x00000008) != 0)
    {
        deviceNumber = 3;
    }

    else if ((bitMap & 0x00000010) != 0)
    {
        deviceNumber = 4;
    }

    else if ((bitMap & 0x00000020) != 0)
    {
        deviceNumber = 5;
    }

    else if ((bitMap & 0x00000040) != 0)
    {
        deviceNumber = 6;
    }

    else if ((bitMap & 0x00000080) != 0)
    {
        deviceNumber = 7;
    }

    deviceSema4Num = ((lineNum - DISKINT) * DEVPERINT) + deviceNumber;

    if (lineNum == TERMINT)
    {
        /*set the status to either receive or transmit*/
        devStatus = terminalInterrupt(&deviceSema4Num);
    }

    else
    {

        devStatus = (deviceRegister->devreg[deviceSema4Num]).d_status; /*copy status*/
        (deviceRegister->devreg[deviceSema4Num]).d_command = ACK;      /*ACK interrupt*/
    }

    devSema4[deviceSema4Num] += 1;
    
    /*we are done waiting for IO, so pop the pcb off*/
    if (devSema4[deviceSema4Num] <= 0)
    {
        pseudoSys4 = removeBlocked(&(devSema4[deviceSema4Num]));/*ask Mikey about this line*/

        if (pseudoSys4 != NULL)
        {
            /*if there is a process, then unblock and set the status*/
            pseudoSys4->p_s.s_v0 = devStatus;
            insertProcQ(&readyQ, pseudoSys4);
            softBlockCnt-=1;
        }
        
    }

    else{ 
        /*if there is nothing to unblock*/
        saveState[deviceSema4Num] = devStatus; /*save the state because there's no where else*/
    }

    /*if there is no currentProc*/
    if (currentProc == NULL)
    {
        scheduleNext();
    }

}

/**
 * Method comment here
 * */
int terminalInterrupt(int *devSema4Num)
{
    /*return device status after distinguishing between transmit and receive case*/

    /*local variables*/
    unsigned int statusRecord;
    volatile devregarea_t *devRegisters;
    /*end of local variables*/

    devRegisters = (devregarea_t *)RAMBASEADDR;

    statusRecord = devRegisters->devreg[(*devSema4Num)].t_transm_status;

    if ((statusRecord & 0x0F) != READY)
    {
        devRegisters->devreg[(*devSema4Num)].t_transm_command = ACK;
    }
    else
    {

        statusRecord = devRegisters->devreg[(*devSema4Num)].t_recv_status;
        devRegisters->devreg[(*devSema4Num)].t_recv_command = ACK;

        (*devSema4Num) += DEVPERINT;
    }

    return (statusRecord);
}