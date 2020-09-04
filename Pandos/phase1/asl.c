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
HIDDEN void freeSemd (semd_PTR);
HIDDEN int inactiveSemd (semd_PTR s, int semAdd);
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
    if(temp == *semAdd){//wefind
        insertProcQ(temp->s_procQ, p);
        return; //idk
    }
    //if we don't find, remove semdFree
    if(semdFree_h==NULL){//if the free list is empty, there is an error
        return NULL;
    }
    //remove semd from Free list if available
    semd_PTR newSemd = semdFree_h;
    semdFree_h = newSemd->s_next;
    newSemd ->s_next =NULL;
    semd_t *actListPrev = findDesc(newSemd);//figure out how tf do this
    //once i get the address
    actListPrev->s_next = newSemd;
    return;//idk
}


//This is a mutator method searches activeSemd List for matching semdAdd provided. 
//Two cases: not found -> error case
//Found -> removeProcQ on the process queue that you found in activeSemd list this value is returned
//This found also has two cases: the processQueue is not empty -> done
//processQueue is empty -> Takes out of active list and inserts into the free list
pcb_PTR removeBlocked (int *semAdd){
     -> semdTrail;
    semd = semdTrail -> s_next;
    if(inactiveSemd(semd, semAdd)){ //to do: write inactive
        return NULL;
    }
    semd_t *temp = removeProcQ(&semAdd -> s_procQ);
    temp -> s_semAdd = semAdd;//this is wrong
    removeProcQ(&semAdd -> s_proQ);//why is this wrong?
    return (FALSE);

}

//This  is a mutator method is the same as removeBlocked, but we call outProcQ instead of removeProcQ
pcb_PTR outBlocked (pcb_PTR p){
         -> semdTrail;
    semd = semdTrail -> s_next;
    if(inactiveSemd(semd, semAdd)){ //to do: write inactive
        return NULL;
    }
    semd_t *temp = outProcQ(&semAdd -> s_procQ,p);
    temp -> s_semAdd = semAdd;//this is wrong
    outProcQ(&semAdd -> s_proQ, p);//why is this wrong?
    return (p);

}

//This is a an accessor method is the same as removeBlocked and outBlocked, but instead it calls headProcQ
//Returns that to the caller
pcb_PTR headBlocked (int *semAdd){
    semd_t *tempsemAdd = findDesc(semAdd);//dummy pointer that stores address from find
    semAdd = tempsemAdd -> s_next;//set address to the next one because that is our node of interest
    if(inactiveSemd(semAdd, semAdd)){
        return NULL;
    }
    return headProcQ (semAdd->s.s_procQ);//this is wrong
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