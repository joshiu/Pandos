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


/**
 * This method is used to determine the appropriate action
 * when the timer generates an interrupt. Look at who interrupted 
 * me, look at Cause.ExcCode.
 *  if line 1, it's a local timer interrupt
 *  if line 2, it's pseudo clock interrupt
 *  if 3-7, then device interrupt 
 * */
void interruptHandler(){
    cpu_t stopTime;
    cpu_t leftoverQTime;


    STCK(stopTime);
    leftoverQTime = getTIMER();

    if(((((state_t *)BIOSDATAPAGE )->s_cause) & 0x00000200) !=0){
        /*local timer interrupt*/
        localTimerInterrupt(stopTime);
    }

    if(((((state_t *)BIOSDATAPAGE )->s_cause) & 0x00000400) !=0){
        /*timer interrupt*/
        pseudoClockInterrupt();
    }

    if(((((state_t *)BIOSDATAPAGE )->s_cause) & 0x00000800) !=0){
        /*disk interrupt*/
        deviceInterrupt(DISKINT);
    }

    if(((((state_t *)BIOSDATAPAGE )->s_cause) & 0x00001000) !=0){/*flash interrupt*/
        deviceInterrupt(FLASHINT);
    }

    if(((((state_t *)BIOSDATAPAGE )->s_cause) & 0x00004000) !=0){/*print interrupt*/
        deviceInterrupt(PRNTINT);
    }

    if(((((state_t *)BIOSDATAPAGE )->s_cause) & 0x00008000) !=0){
        /*terminal interrupt*/
        deviceInterrupt(TERMINT);
    }

    if(currentProc != NULL){

        currentProc ->p_time = currentProc->p_time + (stopTime - startTime);

        copyState((state_t *) BIOSDATAPAGE, currentProc->p_s);

        setSpecificQuantum (currentProc, leftoverQTime);
    }else{
        /*if there is no current, then we have a problem!*/

        HALT();
    }

}

/**
 * The current process's time is up so this method checks if there
 * is a currrent process then it 
 * stops the clock and puts that process back on the readyQ.
**/
void localInterruptTimer(cpu_t stopTime){
    if(currentProc == NULL){
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
void pseudoClockInterrupt(){
    pcb_t *removedPcbs;

    LDIT(1000);
    
    removedPcbs = removeBlocked(&(devSema4[DEVPERINT+DEVCNT]));

    while (removedPcbs !=NULL){
        insertProcQ(&readyQ, removedPcbs);
        softBlockCnt --;
        removedPcbs = removeBlocked(&(devSema4[DEVPERINT+DEVCNT]));
    }

    devSema4[DEVPERINT+DEVCNT] = 0;

    if(currentProc ==NULL){
        scheduleNext();
    }

    loadState(currentProc);
    
}

/** 
 * This method looks at the which device is called (Bus register)
 * It then finds the bit number for the device. (with priority: 1 has highest
 * 7 has the lowest) It then saves the status code and acknowledges the interrupt
 * (Writes acknowledgement in the device register)
 * */
void deviceInterrupt(int deviceType){
    int deviceNumber;
    int deviceSema4Num;
    unsigned int devStatus;
    unsigned int bitMap;
    volatile devregarea_t *deviceRegister; /
    
    deviceRegister = (devregarea_t *)RAMBASEADDR;
    bitMap = deviceRegister->interrupt_dev[(deviceType - DISKINT)]; /*ask about this monday*/
    
}


/**
 * void deviceInterrupt(){
 * which device (look at Bus register)
 * find bit number with priority for specific device 
 *  (bit 1 has highest priority while bit 7 has low)
 * save the status code
 * acknowledge the interrupt (so write acknowledge in device register)
 * pcb_t newPcb = SYS4 on the device sema4
 * newPcb ->v0 = status code
 * insertProcQ(readyQ, newPcb);
 * newPcb status = ready;
 * LDST(saved exception state);
 * */

/**
 * QUANTUM IS OVER!
 * 
 * void localTimerInt(){
 * PLT Interrupt ⇒ you have run out of time
 * acknowledge the interrupt by refilling timer
 * Copy state into currentProc -> p_s 
 * insertProcQ(readyQ, currentProc);
 * Change currentProc state to ready state
 * scheduler();
 * */

/**
 * PseudoClock is done!
 * 
 * void pseudoClockInt(){
 * We sys4 everything that SYS7 on the pseudoClock
 * reset pseudoClock to 0
 * LDST(currentProc) (could also be a wait, so make sure to include this is scheduler)
 * */
