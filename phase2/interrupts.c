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
    cpu_t leftoverQTime;
    /*end of local variables*/

    STCK(stopTime);
    leftoverQTime = getTIMER();
    debug(200);

    if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00000200) != 0)
    {
        /*local timer interrupt*/
        debug(201);
        localTimerInterrupt(stopTime);
    }

    if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00000400) != 0)
    {
        /*timer interrupt*/
        debug(202);
        pseudoClockInterrupt();
    }

    if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00000800) != 0)
    {
        /*disk interrupt*/
        debug(203);
        deviceInterrupt(DISKINT);
    }

    if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00001000) != 0)
    {
        /*flash interrupt*/
        debug(204);
        deviceInterrupt(FLASHINT);
    }

    if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00004000) != 0)
    {
        /*print interrupt*/
        debug(205);
        deviceInterrupt(PRNTINT);
    }

    if (((((state_t *)BIOSDATAPAGE)->s_cause) & 0x00008000) != 0)
    {
        /*terminal interrupt*/
        debug(206);
        deviceInterrupt(TERMINT);
    }
    debug(207);
    if (currentProc != NULL)
    {
        debug(208);

        currentProc->p_time = currentProc->p_time + (stopTime - startTime);
        debug(209);
        copyState((state_t *)BIOSDATAPAGE, currentProc->p_s);

        debug(210);
        setSpecificQuantum(currentProc, leftoverQTime);
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
void localTimerInterrupt(cpu_t stopTime)
{

    if (currentProc == NULL)
    {
        PANIC();
        return;
    }

    currentProc->p_time = timeCalc(stopTime);
    copyState((state_t *)BIOSDATAPAGE, &(currentProc->p_s));

    insertProcQ(&readyQ, currentProc);
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
    debug(2021);

    pcb_t *removedPcbs; /*local variable*/

    LDIT(STANPSEUDOCLOCK);

    removedPcbs = removeBlocked(&(devSema4[DEVPERINT + DEVCNT]));

    while (removedPcbs != NULL)
    {
        debug(2022);
        insertProcQ(&readyQ, removedPcbs);
        softBlockCnt--;
        removedPcbs = removeBlocked(&(devSema4[DEVPERINT + DEVCNT]));
    }

    devSema4[DEVPERINT + DEVCNT] = 0;

    if (currentProc == NULL)
    {
        debug(2023);
        scheduleNext();
    }
    debug(2024);
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
        debug(2061);
        deviceNumber = 0;
    }

    else if ((bitMap & 0x00000002) != 0)
    {
        debug(2062);
        deviceNumber = 1;
    }

    else if ((bitMap & 0x00000004) != 0)
    {
        debug(2063);
        deviceNumber = 2;
    }

    else if ((bitMap & 0x00000008) != 0)
    {
        debug(2064);
        deviceNumber = 3;
    }

    else if ((bitMap & 0x00000010) != 0)
    {
        debug(2065);
        deviceNumber = 4;
    }

    else if ((bitMap & 0x00000020) != 0)
    {
        debug(2066);
        deviceNumber = 5;
    }

    else if ((bitMap & 0x00000040) != 0)
    {
        debug(2067);
        deviceNumber = 6;
    }

    else if ((bitMap & 0x00000080) != 0)
    {
        debug(2068);
        deviceNumber = 7;
    }

    /*in this line, we look at the device in relation to DISKINT*/
    /*so, assume the first device in DISKINT is 0, and we work from there*/
    /*if we not in DISKINT, then we go to the end device,*/
    /* and add deviceNumber to get to our device*/

    deviceSema4Num = ((lineNum - DISKINT) * DEVPERINT) + deviceNumber;
    debug(deviceSema4Num);

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
    debug(deviceSema4Num);
    
    /*we are done waiting for IO, so pop the pcb off*/
    if (devSema4[deviceSema4Num] <= 0)
    {
        pseudoSys4 = removeBlocked(&(devSema4[deviceSema4Num]));
        debug(7777);
        debug(deviceSema4Num);

        if (pseudoSys4 != NULL)
        {
            /*if there is a process, then unblock and set the status*/
            debug(77775);
            pseudoSys4->p_s.s_v0 = devStatus;
            insertProcQ(&readyQ, pseudoSys4);
            softBlockCnt--;
        }
        
    }

    else{ 
        /*if there is no process*/
        debug(77776);
        saveState[deviceSema4Num] = devStatus; /*save the state because there's no where else*/
    }

    if (currentProc == NULL)
    {
        debug(7778);
        scheduleNext();
    }
    debug(7779);
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