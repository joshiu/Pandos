#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"


/**
 * public void SYSCALL(a0){
 * if(kernel_mode !=true){
 * programTrapExec();
 * }
 * if (a0!=NULL ){
 * branch -> look at a0
 * if a0 == 1 then SYS1 return LDST(currentProc);
 * a0 == 2 then sys2 
 * a0 == 3 : sys3 (PC count +4)
 * a0 == 4 : sys4
 * a0 == 5 : sys5(PC count +4)
 * a0 == 6 : sys6
 * a0 == 7 " sys7(PC count +4)
 * a0 == 8 : sys8
 * (check 9/18 notes for more)
 * }
 * }
 * */

/**
 * public int SYS1(a0, state_t *statep, support_t *supportp, int i){
 * pcb_t *newPcb = allocPcb();
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
 * public void SYS2(a0, int i, int j, int k){
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
 * while(lastChild->p_child->p_child != NULL){
 * lastChild = lastChild ->p_child;
 * }
 * 
 * remove all the children
 * 
 * while(!emptyChild(lastChild)){
 * removeChild(lastChild);
 * }
 * 
 * repeat until no children left
 * SYSCALL2(a0,0,0,0);
 * }
 * */

/**
 * SEPTEMBER 14th
 * public void SYS3 (a0, int *semaddr, int i, int j){
 * (PC count +4)
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
 * public void SYS4(a0, int *semaddr,int i ,int j){
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
 * public int SYS5(a0, int int1No, int dnum, int waitForTermRead){
 * SYS3(a0, currentProc->p_semAdd,0,0); /this might be wrong
 * insertBlocked(& currentProc->p_semAdd, currentProc);
 * scheduler(); 
 * 
 * GET BACK TO THIS LATER
 * }
 * */

/**
 * public cpu_t SYS6(a0, int i, int j, int k){
 * cpu_t sumOfTime = currentProc->p_time + amnt of time used in current quantum; 
 * return sumOfTime
 * }
 * */

/**
 * public void SYS7(a0, int i, int j, int k){
 * need to preform P opertation on psuedoclock semaphore; (SYS3)
 * insertBlocked(& currentProc->p_semAdd, currentProc);
 * scheduler();
 * }
 * */

/**
 * public support_t SYS8(a0, int i, int j, int k){
 * if(currentProc -> p_supportStruct == NULL){
 * return NULL;
 * }
 * return currentProc -> p_supportStruct;
 * }
 * 
 * */

/**
 * 
 * 
 * */


/**
 *  something Handler() (ask Mia or Pandos? or Paul)
 * */

/**
 * memoryHandler()
 * */