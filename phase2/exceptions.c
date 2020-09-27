#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"


/**
 * public void SYSCALL(a0){
 * if (a0!=NULL &&  kernel_mode ==true){
 * branch -> look at a0
 * if a0 == 1 then SYS1
 * a0 == 2 then sys2
 * a0 == 3 : sys3
 * a0 == 4 : sys4
 * a0 == 5 : sys5
 * a0 == 6 : sys6
 * a0 == 7 " sys7
 * a0 == 8 : sys8
 * (check 9/18 notes for more)
 * }
 * }
 * */

/**
 * public int SYS1(a0, state_t *statep, support_t *supportp, 0){
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
 * public void SYS2(a0, 0, 0, 0){
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
 * public void SYS3 (a0, int *semaddr, 0, 0){
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
 * public void SYS4(a0, int *semaddr,0,0){
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
 * }
 * */


/**
 *  something Handler() (ask Mia or Pandos? or Paul)
 * */

/**
 * memoryHandler()
 * */