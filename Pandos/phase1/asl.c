#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"


/**
 * This file contains the ASL or active semphone list. Which has public and private methods that are excessors and mutators that
 * manipulates this data structure.
 * 
 * Written by Umang Joshi and Amy Kelley
 * /

/* semaphore descriptor type */
typedef struct semd_t {
    struct semd_t *s_next;/* next element on the ASL */
    int*s_semAdd;/* pointer to the semaphore*/
    pcb_t *s_procQ; /* tail pointer to a*/
                    /* process queue*/
} semd_t;

//local functions
HIDDEN semd_PTR actSemd (semd_PTR, int *semAdd);
HIDDEN void freeSemd (semd_PTR);
HIDDEN int inactiveSemd (semdPTR, int semAdd);
HIDDEN semd_PTR findDesc (int *semAdd);
//end of local functions

//globals
HIDDEN semdFree_h;
HIDDEN semd_h;
//end of globals

//This method searches the active semdList to see if theres the semAdd in it
//Two cases: found -> calls insertProcQ 
//Or not found -> allocate new node and put it into list then preform found
int insertBlocked (int *semAdd, pcb_PTR p){

}


//This is a mutator method searches activeSemd List for matching semdAdd provided. 
//Two cases: not found -> error case
//Found -> removeProcQ on the process queue that you found in activeSemd list this value is returned
//This found also has two cases: the processQueue is not empty -> done
//processQueue is empty -> Takes out of active list and inserts into the free list
pcb_PTR removeBlocked (int *semAdd){
    //get descriptior -> semdtrail
    semd = semdTrail -> s_next
    if(inactiveSemd){ //to do: write inactive
        return NULL
    }
    temp = removeProcQ(&semAdd -> sprocQ)
    temp -> p_Add
    remove(&semAdd -> s_proQ, p)
    return (FALSE);
    //insert, same as remove
    //out same as remove (return p)
}

//This  is a mutator method is the same as removeBlocked, but we call outProcQ instead of removeProcQ
pcb_PTR outBlocked (pcb_PTR p){

}

//This is a an accessor method is the same as removeBlocked and outBlocked, but instead it calls headProcQ
//Returns that to the caller
pcb_PTR headBlocked (int *semAdd){
    semAdd = findDesc semAdd
    semAdd = semAdd -> s_next
    if(inactiveSemd(semAdd, semAdd)){
        return NULL
    }
    return headProcQ (semAdd -> s.s_ProcQ)
}

//This method declares static array of 20 nodes (+ 2 dummy nodes) and then goes through the array and puts each node
//on a free list.
void initASL (){
    static semd [MAXPROC+2]
    semdFree_h = & semdTable[0]
    //go thru and initialize ptr
    semdTable[i-1].s_next = & semTable[i]
    set[MAXPROC-1].s.next = NULL

    //dummy nodes (?)
    semd_h ->s.semAdd = 0
    mkEmptyProcQ(s_ProcQ)
    //to do: go to end and set null then reun make empty and set to null
}

//to do: where to add findDesc? -> pesudo code in Umang's notes