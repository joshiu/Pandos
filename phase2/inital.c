#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"

/**
 * Main comment here
 * */


/** contains the main
 * extern void uTLB_RefillHandler();
 * initPcb
 * initASL
 * readyQ = mkemptyQ()
 * set current Proc
 * scheduler() <-- Last line of the code
 *
 * all the global variables are to be defined and initialized here
 * initialize all 40 semaphores to 0 (use 1 process and put it on readyQ)
 * */

/**
 * Define and Init Global Variables
 * processcnt = 0;
 * softBlockcnt = 0;
 * readyQue = mkeEmptyProc();
 * currentProc = NULL;
 * int devicesSema4[?] = 0;
 * initPCB();
 * ASL(); instantiate Pcb and make the state of the Pcb into another state (ready) and put on readyQ
 * then insert and run scheduler()
 * */

/**
 * genExceptionHandler(take in file state/cause register);
 * create a branch: if interupt go to interupt, if syscall go to syscall(interrupt.c), if program execption go to exceptions.c,
 * memory mangement(interrupt.c) then memoryHandler
 * */

/*********************************Psycho Code*************************************************/

/**
 * Define and Init Global Variables
 * processcnt = 0;
 * softBlockcnt = 0;
 * readyQue = mkeEmptyProc();
 * currentProc = NULL;
 * int devicesSema4s[49] = 0; 2 sema4s per device :, 5 devices per terminal : array size is 49
 * initPCB();
 * ASL(); instantiate Pcb and make the state of the Pcb into another state (ready) and put on readyQ
 * then insert and run scheduler()
 * */

/**
 * something to populate passup at 0x0FFF.F900
 * xxx->tlb refll handler = (memaddr) uTLB RefillHandler;
 *
 * set stack pointer for nucleus TLB refill event to the top of the nucleus event: Ox2000.1000
 * set xxx-> expection_handler = (memaddr) fooBar; (change foo name)
 * set stack pointer for the expection handler to top of Nucleus 0x2000.1000
 * initPCB
 * initSemd
 * initilzate all the nucleus maintained variables
 * Process Count (0), Soft-block Count (0), Ready Queue (mkEmptyProcQ()), and Current Process (NULL).
 * set system wide interval timer at 100 millsecs
 * place pcb on ready queue and processcount ++
 * allocPCB
 * init alloct state on PCB (interrupts enable, process local timer enabled, cornal mode on)
 * SP = RAMTOP
 * PCADDRESS = TEST
 * set all process tree fields to NULL
 * set p_time = 0;
 * set p_semAdd = NULL;
 * set p_supportstruct = NULL;
 * call scheduler();
 * after this never return to main again
 * */