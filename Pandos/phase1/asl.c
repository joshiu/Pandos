#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"


/**
 * This file contains the ASL or active semphone list. Which has public and private methods that are excessors and mutators that
 * manipulates this data structure.
 * 
 * Written by Umang Joshi and Amy Kelley
 **/

/*semaphore descriptor type */
typedef struct semd_t {
    struct semd_t   *s_next;/* next element on the ASL */
    int             *s_semAdd;/* pointer to the semaphore*/
    pcb_t           *s_procQ; /* tail pointer to a*/
                            /* process queue*/
} semd_t;

/*local functions*/
HIDDEN semd_PTR findDesc (int *semAdd);
/*end of local functions*/

/*globals*/
HIDDEN semd_PTR semdFree_h;
HIDDEN semd_PTR semd_h;
/*end of globals*/

/**
 * This method declares static array of 20 nodes (+ 2 dummy nodes) and then goes through the array and puts each node
 * on a free list not complete. "Initialize the semdFree list to contain all the elements of the array
 * static semd t semdTable[MAXPROC] This method will be only called once during data structure initialization."
**/
void initASL (){
    int i;
    static semd_t semdTable[MAXPROC+2]; /* make a dummy table with +2 entries here */
    semdFree_h = &semdTable[0];
    for(i=1; i<MAXPROC; i++){
        semdTable[i-1].s_next = & semdTable[i];
    }
    semdTable[MAXPROC-1].s_next = NULL;
    semd_h = &semdTable[MAXPROC];
    semd_h->s_next = &semdTable[MAXPROC+1];
    semd_h->s_next->s_next = NULL;
    semd_h->s_semAdd =0;/*head of list points to 0*/
    semd_h -> s_next ->s_semAdd = MAXINT;/*tail of list points to near infinity*/
    semd_h -> s_procQ = mkEmptyProcQ();/*give head an empty ProcQ*/
    semd_h->s_next->s_procQ = mkEmptyProcQ();/*give tail empty ProcQ*/
    return;
}

/**
 * This method searches the active semdList to see if theres the semAdd in it
 * Two cases: found -> calls insertProcQ 
 * Or not found -> allocate new node and put it into the active list then preform found case
**/
int insertBlocked (int *semAdd, pcb_PTR p){
    semd_PTR temp = findDesc(semAdd);/*dummy pointer that points to address from find*/
    if(temp->s_next->s_semAdd == semAdd){/*if the sem addresses match*/
        insertProcQ(&(temp->s_next-> s_procQ), p);/*first variable wrong here*/
        return FALSE; 
    }
    /*if we don't find, remove semdFree*/
    if(semdFree_h==NULL){/*if the free list is empty, there is an error*/
        return NULL;
    }
    /*remove semd from Free list and add to ASL, then and insert pcb into new semd */
    semd_PTR newSemd = semdFree_h;/*get new semd from list*/
    semdFree_h = newSemd->s_next;
    newSemd ->s_next =NULL;
    semd_t *actListPrev = findDesc(newSemd->s_semAdd);
    semd_t *actListNext = actListPrev->s_next;
    actListPrev->s_next = newSemd;
    newSemd ->s_next = actListNext;/*point new to next*/
    insertProcQ(&(newSemd->s_procQ), p);
    if(semdFree_h==NULL){/*if the free list is empty,after semd removal*/
        return TRUE;
    }
    return(FALSE);/*return false if semdFree not empty*/
}


/**
 * This is a mutator method searches activeSemd List for matching semdAdd provided. 
 * Two cases: not found -> error case
 * Found -> removeProcQ on the process queue that you found in activeSemd list this value is returned
 * This found also has two cases: the processQueue is not empty -> done
 * processQueue is empty -> Takes node out of active list and inserts into the free list (deallocate)
**/
pcb_PTR removeBlocked (int *semAdd){
    semd_PTR tempSemAdd = findDesc(semAdd);/*dummy pointer that points to address from find*/
    if(tempSemAdd->s_next->s_semAdd == semAdd){
        pcb_PTR returnP = removeProcQ(&(tempSemAdd->s_next-> s_procQ));/*first variable wrong here*/
        if(emptyProcQ(tempSemAdd->s_next -> s_procQ)){/*if process queue empty, put semd back on free list*/
            semd_PTR tempRemoval = tempSemAdd ->s_next;
            tempSemAdd ->s_next = tempRemoval->s_next;
            tempRemoval->s_next = semdFree_h;
            semdFree_h = tempRemoval;
        }
        /*if process queue not empty/we're done adjusting the ASL, then return*/ 
        return(returnP);
    }
    /*if temp and semAdd don't match, then semAdd not in ASL, so error*/
    return NULL;
}

/**
 * This is a mutator method is the same as removeBlocked, but we call outProcQ instead of removeProcQ
 * "Remove the pcb pointed to by p from the process queue associated
 * with p’s semaphore (p → p semAdd) on the ASL"
**/
pcb_PTR outBlocked (pcb_PTR p){
    semd_PTR tempSemAdd = findDesc(p->p_semAdd);/*dummy pointer that points address from find*/
    if(tempSemAdd->s_next->s_semAdd == p->p_semAdd){
        pcb_PTR returnP = outProcQ(&(tempSemAdd->s_next-> s_procQ),p);/*first variable wrong here*/
        if(emptyProcQ(tempSemAdd->s_next -> s_procQ)){/*if queue empty, return semd to free list*/
            semd_PTR tempRemoval = tempSemAdd ->s_next;
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
 * This is an accessor method is the same as removeBlocked and outBlocked, 
 * but instead it calls headProcQ. Returns that to the caller.
 * "Return a pointer to the pcb that is at the head of the process queue
 * associated with the semaphore semAdd. Return NULL if semAdd is
 * not found on the ASL or if the process queue associated with semAdd
 * is empty"
**/
pcb_PTR headBlocked (int *semAdd){
    semd_t *tempSemAdd = findDesc(semAdd);/*dummy pointer that points to address from find*/
    if(tempSemAdd->s_next->s_semAdd ==semAdd){
        if(emptyProcQ(tempSemAdd->s_next->s_procQ)){
            /*semd_PTR tempRemoval = tempSemAdd ->s_next;
            tempSemAdd ->s_next = tempRemoval->s_next;
            tempRemoval->s_next = semdFree_h;
            semdFree_h = tempRemoval;*/ /*Method to put semaphore with no queue back on list, don't know if we need*/
            return NULL;
        }
        return headProcQ(tempSemAdd->s_next->s_procQ);
    }
    return NULL;
}

/**
 * Cycles through the ASL and finds the closest value to given semAdd.
**/
semd_PTR findDesc(int *semAdd){
    semd_t *temp = semd_h; /*dummy node pointing to the head*/
    while(temp ->s_next-> s_semAdd < semAdd)
        temp=temp->s_next;
    return temp; 
}