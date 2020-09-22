#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"


/**
 * This file contains two lists
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G. 
 **/

/*semaphore descriptor type */
typedef struct semd_t {
    struct semd_t   *s_next;/* next element on the ASL */
    int             *s_semAdd;/* pointer to the semaphore*/
    pcb_t           *s_procQ; /* tail pointer to a*/
                            /* process queue*/
} semd_t;

/*local functions*/
HIDDEN semd_PTR searchASL (int *semAdd);
HIDDEN semd_PTR allocASL(int *semAdd);
/*end of local functions*/

/*globals*/
HIDDEN semd_PTR semdFree_h;
HIDDEN semd_PTR semd_h;
/*end of globals*/

/**
 * This method initializes the semdFree list to contain all the elements of the array
 * static semd t semdTable[MAXPROC] This method will be only called once during data structure initialization.
**/
void initASL (){
    int i;
    static semd_t semdTable[MAXPROC+2]; /* make a table with MAXPROC+2 entries*/
    semdFree_h = &semdTable[0];
    for(i=1; i<MAXPROC; i++){/*initialize semdFree List*/
        semdTable[i-1].s_next = & semdTable[i];
    }
    semdTable[MAXPROC-1].s_next = NULL;
    semd_h = &semdTable[MAXPROC];
    semd_h->s_next = &semdTable[MAXPROC+1];
    semd_h->s_next->s_next = NULL;
    semd_h->s_semAdd =0;/*head of ASL list has semdAdd 0*/
    semd_h -> s_next ->s_semAdd = MAXINT;/*tail of ASL list has semdAdd near infinity*/
    semd_h -> s_procQ = mkEmptyProcQ();
    semd_h->s_next->s_procQ = mkEmptyProcQ();
    return;
}

/**
 * This method searches the active semdList for a semAdd.
 * Two cases: found -> calls insertProcQ 
 * Or not found -> allocate new node and put it into the active list then preform found case.
**/
int insertBlocked (int *semAdd, pcb_PTR p){
    semd_PTR ASLPrev = searchASL(semAdd);/*dummy pointer that points to address from find*/
    if(ASLPrev->s_next->s_semAdd == semAdd){/*if the sem addresses match*/
        p->p_semAdd = semAdd;
        insertProcQ(&(ASLPrev->s_next-> s_procQ), p);
        return FALSE; 
    }
    /*if we don't find, remove semdFree*/
    if(semdFree_h==NULL){/*if the free list is empty, there is an error*/
        return TRUE;
    }
    /*remove semd from Free list and add to ASL, then and insert pcb into new semd */
    semd_t *newSemd = allocASL(semAdd);
    semd_t *ASLNext = ASLPrev->s_next;
    ASLPrev->s_next = newSemd;
    p->p_semAdd = semAdd;
    newSemd ->s_next = ASLNext;
    insertProcQ(&(newSemd->s_procQ), p);
    return FALSE;/*return false if semdFree not empty*/
}


/**
 * This is a mutator method searches activeSemd List for matching semdAdd provided. 
 * Two cases: not found -> error case
 * Found -> removeProcQ on the process queue that you found in activeSemd list this value is returned
 * Afterwards, if the processQueue is not empty -> done.
 * processQueue is empty -> Removes the node out of active list and inserts into the free list.
**/
pcb_PTR removeBlocked (int *semAdd){
    semd_PTR tempSemAdd = searchASL(semAdd);/*dummy pointer that points to address from find*/
    if(tempSemAdd->s_next->s_semAdd == semAdd){/*Found semd with given semAdd*/
        pcb_PTR returnP = removeProcQ(&(tempSemAdd->s_next-> s_procQ));
        if(emptyProcQ(tempSemAdd->s_next -> s_procQ)){/*if process queue empty, put semd back on free list*/
            semd_PTR tempRemoval = tempSemAdd ->s_next;/*Dummy pointer to what we want to remove*/
            tempSemAdd ->s_next = tempRemoval->s_next;
            tempRemoval->s_next = semdFree_h;
            tempRemoval->s_semAdd = NULL;
            semdFree_h = tempRemoval;
        }
        /*if process queue not empty/we're done adjusting the ASL, then return*/ 
        return(returnP);
    }
    /*if temp and semAdd don't match, then semAdd not in ASL, so error*/
    return NULL;
}

/**
 * This is a mutator method removes the pcb pointed to by p from the process queue associated
 * with p’s semaphore (p → p semAdd) on the ASL.
**/
pcb_PTR outBlocked (pcb_PTR p){
    semd_PTR tempSemAdd = searchASL(p->p_semAdd);/*dummy pointer that points address from find*/
    if(tempSemAdd->s_next->s_semAdd == p->p_semAdd){/*if semd that contains p is found*/
        pcb_PTR returnP = outProcQ(&(tempSemAdd->s_next-> s_procQ),p);/*dummy pointer to return*/
        if(emptyProcQ(tempSemAdd->s_next -> s_procQ)){/*if queue empty, return semd to free list*/
            semd_PTR tempRemoval = tempSemAdd ->s_next;/*Dummy pointer that points to semd that we want to put on the free list*/
            tempSemAdd ->s_next = tempRemoval->s_next;
            tempRemoval->s_next = semdFree_h;
            semdFree_h = tempRemoval;
        }
        /*once done with semaphore, return p*/
        return returnP;
    }
    /*if semd not on list, error*/
    return NULL;
}

/**
 * This is an accessor method returns a pointer to the pcb that is at the head of the process queue
 * associated with the semaphore semAdd. Return NULL if semAdd is
 * not found on the ASL or if the process queue associated with semAdd is empty.
**/
pcb_PTR headBlocked (int *semAdd){
    semd_t *tempSemAdd = searchASL(semAdd);/*dummy pointer that points to address from find*/
    if(tempSemAdd->s_next->s_semAdd ==semAdd){/*If semd with semAdd is found*/
        if(emptyProcQ(tempSemAdd->s_next->s_procQ)){/*If semd queue is empty*/
            return NULL;
        }
        return headProcQ(tempSemAdd->s_next->s_procQ);/*If queue not empty return head*/
    }
    return NULL;/*If semd not found*/
}

/**
 * Cycles through the ASL and finds the closest value to given semAdd.
**/
semd_PTR searchASL(int *semAdd){
    semd_t *temp = semd_h; /*dummy node pointing to the head*/
    while(temp ->s_next-> s_semAdd < semAdd){/*Finds prev semd in ASL*/
        temp=temp->s_next;
    }
    return temp;/*returns prev*/
}

/**
 * Allocates a semd from the semdFree list, assigns it the given semAdd, and creates and empty queue.
 **/
semd_PTR allocASL(int *semAdd){
    semd_PTR newSemd = semdFree_h;/*get new semd from list*/
    semdFree_h = newSemd->s_next;
    newSemd ->s_next =NULL;
    newSemd ->s_semAdd = semAdd;
    newSemd ->s_procQ = mkEmptyProcQ();
    return newSemd;
}