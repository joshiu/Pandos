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
    struct semd_t   *s_next;/* next element on the ASL */
    int             *s_semAdd;/* pointer to the semaphore*/
    pcb_t           *s_procQ; /* tail pointer to a*/
                            /* process queue*/
} semd_t;

//local functions
HIDDEN semd_PTR actSemd (semd_PTR s, int *semAdd);
HIDDEN void freeSemd (semd_PTR);//not writing this because writing it out each time makes more sense to me right now; will condense later
HIDDEN int inactiveSemd (semd_PTR s, int semAdd);//not writing this because writing it out each time makes more sense to me right now; will condense later
HIDDEN semd_PTR findDesc (int *semAdd);
//end of local functions

//globals
HIDDEN semd_PTR semdFree_h;
HIDDEN semd_PTR semd_h;
//end of globals

//This method searches the active semdList to see if theres the semAdd in it
//Two cases: found -> calls insertProcQ 
//Or not found -> allocate new node and put it into list then preform found
int insertBlocked (int *semAdd, pcb_PTR p){
    semd_PTR temp = findDesc(semAdd);
    if(temp->s_next == *semAdd){//wefind
        insertProcQ(temp->s_next-> s_procQ, p);
        return; //idk
    }
    //if we don't find, remove semdFree
    if(semdFree_h==NULL){//if the free list is empty, there is an error
        return NULL;
    }
    //remove semd from Free list if available
    semd_PTR newSemd = semdFree_h;//get new semd from list
    semdFree_h = newSemd->s_next;//adjust head to point to new head
    newSemd ->s_next =NULL;//orphanize
    semd_t *actListPrev = findDesc(newSemd);//is this correct?
    semd_t *actListNext = actListPrev->s_next;//address of next one
    actListPrev->s_next = newSemd;//point prev to new
    newSemd ->s_next = actListNext;//point new to next
    insertProcQ(newSemd->s_procQ, p);//insert pcb into new
    return; //idk     
}


//This is a mutator method searches activeSemd List for matching semdAdd provided. 
//Two cases: not found -> error case
//Found -> removeProcQ on the process queue that you found in activeSemd list this value is returned
//This found also has two cases: the processQueue is not empty -> done
//processQueue is empty -> Takes out of active list and inserts into the free list
pcb_PTR removeBlocked (int *semAdd){
    semd_PTR tempSemAdd = findDesc(semAdd);
    if(tempSemAdd->s_next == *semAdd){
        removeProcQ(tempSemAdd->s_next-> s_procQ);
        if(emptyProcQ(tempSemAdd->s_next -> s_procQ)){
            semd_PTR removal = tempSemAdd ->s_next;// remove next one (current is previous)
            tempSemAdd ->s_next = removal->s_next;//point prev to next
            removal->s_next = semdFree_h;//point removed one to freeList
            semdFree_h = removal;//make removed one the head of the FreeList
            return; //idk
        }
        //not empty then done 
        return;//idk
    }
    //if temp and semAdd don't match, then semAdd not in ASL, so error
    return NULL;
}

//This  is a mutator method is the same as removeBlocked, but we call outProcQ instead of removeProcQ
pcb_PTR outBlocked (pcb_PTR p){
    semd_PTR tempSemAdd = findDesc(p->p_semAdd);
    if(tempSemAdd->s_next == p->p_semAdd){
        outProcQ(tempSemAdd->s_next-> s_procQ,p);
        if(emptyProcQ(tempSemAdd->s_next -> s_procQ)){
            semd_PTR removal = tempSemAdd ->s_next;// remove next one (current is previous)
            tempSemAdd ->s_next = removal->s_next;//point prev to next
            removal->s_next = semdFree_h;//point removed one to freeList
            semdFree_h = removal;//make removed one the head of the FreeList
            return; //idk
        }
        //not empty then done 
        return;//idk
    }
    //if temp and semAdd don't match, then semAdd not in ASL, so error
    return NULL;
}

//This is a an accessor method is the same as removeBlocked and outBlocked, but instead it calls headProcQ
//Returns that to the caller
pcb_PTR headBlocked (int *semAdd){
    semd_t *tempsemAdd = findDesc(semAdd);//dummy pointer that stores address from find
    if(tempsemAdd->s_next ==semAdd){
        if(emptyProcQ(tempsemAdd->s_next->s_procQ)){
            return NULL;
        }
        return tempsemAdd->s_next->s_procQ->p_next;
    }
    return NULL;
}

//This method declares static array of 20 nodes (+ 2 dummy nodes) and then goes through the array and puts each node
//on a free list.
//not complete
void initASL (){
    static semd_t semdTable[MAXPROC+2];
    semdFree_h = &semdTable[0];
    for(int i= 0; i<MAXPROC; i++){
        semdTable[i-1].s_next = & semdTable[i];
    }
    semdTable[MAXPROC-1].s_next = NULL;
    semd_h->s_semAdd =0;
    semd_h -> s_procQ = mkEmptyProcQ();    
}
//idk if totally correct
//cycles through the ASL and finds the given semAdd
semd_PTR findDesc(int *semAdd){
    semd_t *temp = semd_h; //dummy node
    while(temp ->s_next-> s_semAdd < semAdd)
        temp=temp->s_next;//possible memory leak?
    return temp; 
}