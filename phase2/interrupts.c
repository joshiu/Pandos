#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"
#include "/usr/local/include/umps3/umps/libumps.h"
#include "../h/exceptions.h"
#include "../h/initial.h"
#include "../h/scheduler.h"

/**
 * This file occurs when the exception cause number is 0. 
 * The reasons this file is called is either a previously 
 * initiated I/O request completes, or the quantum finishes
 * or the Interval Timer makes a 0x0000.0000 ⇒ 0xFFFF.FFFF transition. 
 * 
 * Written by Umang Joshi and Amy Kelley
 * */

/************ FILE SPECIFIC METHODS *********************/
HIDDEN void localTimerInterrupt(cpu_t time);
HIDDEN void pseudoClockInterrupt();
HIDDEN void deviceInterrupt(int deviceType);

/* end of file specific methods */

/**
 * This method is used to determine the appropriate action
 * when the an interrupt is generated. Look at the Cause.ExcCode.
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

    if (((((state_t *)BIOSDATAPAGE)->s_cause) & PLTINTERRUPT) != 0)
    {
        /*local timer interrupt*/
        localTimerInterrupt(stopTime);
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & PSEUDOCLOCKINT) != 0)
    {
        /*timer interrupt*/
        pseudoClockInterrupt();
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & DISKINTERRUPT) != 0)
    {
        /*disk interrupt*/
        deviceInterrupt(DISKINT);
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & FLASHINTERRUPT) != 0)
    {
        /*flash interrupt*/
        deviceInterrupt(FLASHINT);
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & PRINTINTERRUPT) != 0)
    {
        /*print interrupt*/
        deviceInterrupt(PRNTINT);
    }

    else if (((((state_t *)BIOSDATAPAGE)->s_cause) & TERMINALINTERRUPT) != 0)
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
 * The current process's quantum is up so the method
 * checks to see if there is a process, then adjusts
 * the time and puts the process back on the ready queue. 
 * Then schedules next. 
**/
void localTimerInterrupt(cpu_t timeStop)
{
    /*if we don't have a process, something wrong*/
    if (currentProc == NULL)
    {
        PANIC();
    }

    /*write time into p_time*/
    currentProc->p_time += (timeStop - startTime);
    copyState((state_t *)BIOSDATAPAGE, &(currentProc->p_s));

    insertProcQ(&readyQ, currentProc);
    currentProc = NULL;

    /*put back on Q and schedule next*/
    scheduleNext();
}

/** 
 * This method is called when the pseudo clock
 * reaches 0 and it frees everything that we blocked
 * on the pseudo clock sema4 and puts it back on the ready
 * queue. It then resets clock to 100000 and calls the 
 * scheduler and loads the current process, if there is one.
 * */
void pseudoClockInterrupt()
{
    /*start local variable*/
    pcb_t *removedPcbs; 
    /*end local variable*/

    LDIT(STANPSEUDOCLOCK);/*reset psuedoclock*/

    removedPcbs = removeBlocked(&(devSema4[DEVPERINT + DEVCNT]));

    /*as long as something needs to be removed, remove it*/
    while (removedPcbs != NULL)
    {
        insertProcQ(&readyQ, removedPcbs);
        softBlockCnt -= 1;
        removedPcbs = removeBlocked(&(devSema4[DEVPERINT + DEVCNT]));
    }

    devSema4[DEVPERINT + DEVCNT] = 0;

    /*if no proc, scedule next*/
    if (currentProc == NULL)
    {
        scheduleNext();
    }

    /*return back to interruptHandler*/
}

/** 
 * This method looks at the Bus register and the bitmap
 * for the device that called the interrupt. Then it finds 
 * the bit number for the device, keepng priority (1 has highest
 * 7 has the lowest). It then saves the status code and 
 * writes the acknowledgement in the device register. 
 * 
 * Note: look at everything in relation to DISKINT 
 * (we make DISKINT our "0th" line and continue from there)
 * */
void deviceInterrupt(int lineNum)
{

    /*Local Variables*/
    int deviceNumber;
    unsigned int devStatus;
    unsigned int bitMap;
    volatile devregarea_t *deviceRegister;
    pcb_t *pseudoSys4;
    volatile devregarea_t *devRegisters;
    int foundDev;
    /* End of Local Variables*/

    foundDev = FALSE; /*tells us if we found the deviceNum*/

    deviceRegister = (devregarea_t *)RAMBASEADDR;
    bitMap = deviceRegister->interrupt_dev[(lineNum - DISKINT)];

    /*if the bitMap has nothing in it, something is wrong*/
    if ((&(bitMap) == NULL) & !(foundDev))
    {
        PANIC();
    }

    /*see which device is on*/
    if (((bitMap & DEVICE0) != 0) & !(foundDev) )
    {
        deviceNumber = 0;
        foundDev = TRUE; /*we found the device!*/
    }

    if (((bitMap & DEVICE1) != 0) & !(foundDev))
    {
        deviceNumber = 1;
        foundDev = TRUE;
    }

    if (((bitMap & DEVICE0) != 0) & !(foundDev))
    {
        deviceNumber = 2;
        foundDev = TRUE;
    }

    if (((bitMap & DEVICE0) != 0) & !(foundDev))
    {
        deviceNumber = 3;
        foundDev = TRUE;
    }

    if (((bitMap & DEVICE0) != 0) & !(foundDev))
    {
        deviceNumber = 4;
        foundDev = TRUE;
    }

    if (((bitMap & DEVICE0) != 0) & !(foundDev))
    {
        deviceNumber = 5;
        foundDev = TRUE;
    }

    if (((bitMap & DEVICE0) != 0) & !(foundDev))
    {
        deviceNumber = 6;
        foundDev = TRUE;
    }

    if (((bitMap & DEVICE0) != 0) & !(foundDev))
    {
        deviceNumber = 7;
        foundDev = TRUE;
    }

    /*adjust the device number*/

    deviceNumber += ((lineNum - DISKINT) * DEVPERINT);

    if (lineNum == TERMINT)
    {
        /*check to see if transmit or receive*/ 

        devRegisters = (devregarea_t *)RAMBASEADDR;

        devStatus = devRegisters->devreg[(deviceNumber)].t_transm_status;

        if ((devStatus & TRANSMITBITS) != TRUE)
        {
            devRegisters->devreg[(deviceNumber)].t_transm_command = ACK;
        }

        else
        {
            devStatus = devRegisters->devreg[(deviceNumber)].t_recv_status;
            devRegisters->devreg[(deviceNumber)].t_recv_command = ACK;

            deviceNumber += DEVPERINT;
        }
    }

    else
    {

        devStatus = (deviceRegister->devreg[deviceNumber]).d_status; /*copy status*/
        (deviceRegister->devreg[deviceNumber]).d_command = ACK;      /*ACK interrupt*/
    }

    devSema4[deviceNumber] += 1;

    /*we are done waiting for IO, so unblock pcb */
    if (devSema4[deviceNumber] <= 0)
    {
        pseudoSys4 = removeBlocked(&(devSema4[deviceNumber]));

        if (pseudoSys4 != NULL)
        {
            /*if there is a process, then unblock and set the status*/
            pseudoSys4->p_s.s_v0 = devStatus;
            insertProcQ(&readyQ, pseudoSys4);

            softBlockCnt -= 1;
        }
    }

    else
    {
        /*nothing to unblock*/
        /*save the state because there's no where else*/
        saveState[deviceNumber] = devStatus;
    }

    /*if there is no currentProc*/
    if (currentProc == NULL)
    {
        scheduleNext();
    }
}
