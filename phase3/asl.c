#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"

/**
 * This file contains the ASL which is a semaphore that contains 
 * an address (semAdd) and a process queue. It also contains two lists.
 * Both are NULL terminated single, linear list. The first list is the 
 * Active Semaphore List or ASL and it's a list sorted by semAdd from 0 
 * to MAXINT. The second list is a semaphore descriptors (semdFree list)
 * that holds the unused semaphore descriptors. A max of 20 semaphores can be used.
 * 
 * Written by Umang Joshi and Amy Kelley with help from Mikey G. 
 **/

/*semaphore descriptor type */
typedef struct semd_t
{
    struct semd_t *s_next; /* next element on the ASL */
    int *s_semAdd;         /* pointer to the semaphore*/
    pcb_t *s_procQ;        /* tail pointer to a*/
                           /* process queue*/
} semd_t;

/*local functions*/

HIDDEN semd_PTR searchASL(int *semAdd); /*searches through ASL and finds the sema4*/
HIDDEN semd_PTR allocASL(int *semAdd);  /*allocs sema4*/
HIDDEN void backToFreeList(semd_PTR prevSema4);

/*end of local functions*/

/*globals*/

HIDDEN semd_PTR semdFree_h; /*free list that is unsorted*/
HIDDEN semd_PTR semd_h;     /*sorted sema4 list*/

/*end of globals*/

/**
 * This method initializes the semdFree list to contain all the elements of the array
 * static semd t semdTable[MAXPROC] This method will be only called once during data 
 * structure initialization.
**/
void initASL()
{
    int i;

    static semd_t semdTable[MAXPROC + 2]; /* make a table with MAXPROC+2 entries*/

    semdFree_h = &semdTable[0]; /*set free list pointer to beginning of list*/

    for (i = 1; i < MAXPROC; i++)
    {
        /*initialize semdFree List*/
        semdTable[i - 1].s_next = &semdTable[i];
    }

    semdTable[MAXPROC - 1].s_next = NULL; /*last one on free list has no next*/

    semd_h = &semdTable[MAXPROC]; /*set 2nd last sema4 to head of sorted ASL*/

    semd_h->s_next = &semdTable[MAXPROC + 1]; /*next one is last ASL entry*/

    semd_h->s_next->s_next = NULL; /*no other next on sorted ASL*/

    semd_h->s_semAdd = 0;              /*head of ASL list has semdAdd 0*/
    semd_h->s_next->s_semAdd = MAXINT; /*tail of ASL list has semdAdd near infinity*/

    semd_h->s_procQ = mkEmptyProcQ();
    semd_h->s_next->s_procQ = mkEmptyProcQ();

    return;
}

/**
 * This method searches the active semdList for a semAdd.
 * Two cases: found -> calls insertProcQ 
 * Or not found -> allocate new node and put it into the active list 
 * then preform found case.
**/
int insertBlocked(int *semAdd, pcb_PTR p)
{
    semd_PTR ASLPrev = searchASL(semAdd); /*dummy pointer to insert location in ASL*/

    if (ASLPrev->s_next->s_semAdd == semAdd)
    {
        /*if sema4 already in ASL*/
        p->p_semAdd = semAdd;
        insertProcQ(&(ASLPrev->s_next->s_procQ), p); /*insert p into Q in ASL*/

        return FALSE;
    }

    /*if we don't find, allocate*/

    if (semdFree_h == NULL)
    {
        /*if the free list is empty, there is an error*/
        return TRUE;
    }

    /*remove semd from Free list and add to ASL, then and insert pcb into new semd */

    semd_t *newSemd = allocASL(semAdd); /*allocate new sema4*/
    semd_t *ASLNext = ASLPrev->s_next;  /*find next in line*/

    ASLPrev->s_next = newSemd;
    p->p_semAdd = semAdd;
    newSemd->s_next = ASLNext;

    insertProcQ(&(newSemd->s_procQ), p);

    return FALSE; /*return false if semdFree not empty*/
}

/**
 * Search ASL for sema4 with given semdAdd and remove 
 * pcb using removeProcQ() from queue of that sema4 if present.
 * Free sema4 if the queue is empty, otherwise go on. 
 * If sema4 not found -> error case
**/
pcb_PTR removeBlocked(int *semAdd)
{
    semd_PTR tempSemAdd = searchASL(semAdd); /*dummy pointer to sema4 on ASL*/

    if (tempSemAdd->s_next->s_semAdd == semAdd)
    {
        /*Found sema4 with given semAdd*/

        pcb_PTR returnP = removeProcQ(&(tempSemAdd->s_next->s_procQ)); /*remove pcb from queue*/

        if (emptyProcQ(tempSemAdd->s_next->s_procQ))
        {
            /*if queue empty, put sema4 back on free list*/
            backToFreeList(tempSemAdd);
        }

        /*if process queue not empty/we're done adjusting the ASL, then return*/
        return (returnP);
    }

    /*if temp and semAdd don't match, then semAdd not in ASL, so error*/
    return NULL;
}

/**
 * This is a mutator method that removes the pcb p from the process 
 * queue of p’s semaphore (p → p semAdd) on the ASL. 
**/
pcb_PTR outBlocked(pcb_PTR p)
{
    semd_PTR tempSemAdd = searchASL(p->p_semAdd); /*dummy pointer to p's sema4 on ASL*/

    if (tempSemAdd->s_next->s_semAdd == p->p_semAdd)
    {
        /*if semd that contains p is found*/
        pcb_PTR returnP = outProcQ(&(tempSemAdd->s_next->s_procQ), p); /*removed pcb*/

        if (emptyProcQ(tempSemAdd->s_next->s_procQ))
        {
            /*if queue is empty, return sema4 back to free list*/
            backToFreeList(tempSemAdd);
        }

        /*once done with semaphore, return p*/
        return returnP;
    }

    /*if semd not on list, error*/
    return NULL;
}

/**
 * This is an accessor method returns a pointer to the pcb at the 
 * head of the process queue associated with the semaphore with given 
 * semAdd. Return NULL if semAdd is not on the ASL or if the process 
 * queue is empty.
**/
pcb_PTR headBlocked(int *semAdd)
{
    semd_t *tempSemAdd = searchASL(semAdd); /*dummy pointer to sema4 on ASL with semAdd*/
    if (tempSemAdd->s_next->s_semAdd == semAdd)
    {
        /*If sema4 with semAdd is found*/

        if (emptyProcQ(tempSemAdd->s_next->s_procQ))
        {
            /*If sema4 queue is empty*/
            return NULL;
        }

        return headProcQ(tempSemAdd->s_next->s_procQ); /*If queue not empty return head*/
    }
    return NULL; /*If semd not found*/
}


/************************************LOCAL FUNCTIONS***************************************/

/**
 * Method that cycles through the ASL and finds the closest  
 * value less than given semAdd. Returns pointer to previous sema4. 
**/
semd_PTR searchASL(int *semAdd)
{
    semd_t *temp = semd_h; /*dummy node pointing to the head*/

    while (temp->s_next->s_semAdd < semAdd)
    {
        /*Finds prev semd in ASL*/
        temp = temp->s_next;
    }

    return temp; /*returns prev*/
}

/**
 * Allocates a semd from the semdFree list, assigns it 
 * a given semAdd, and creates an empty queue. Returns 
 * pointer to newly created sema4.
 **/
semd_PTR allocASL(int *semAdd)
{
    semd_PTR newSemd = semdFree_h; /*get new semd from list*/
    semdFree_h = newSemd->s_next;
    newSemd->s_next = NULL;
    newSemd->s_semAdd = semAdd;
    newSemd->s_procQ = mkEmptyProcQ();
    return newSemd;
}

/**
 * This method takes in the sema4 pointer to the previous 
 * sema4 on the ASL and removes the next sema4 on the ASL.
 * Note, we check in outBlocked and removeBlocked that the 
 * sema4 we want to remove has an empty queue. 
 * */
void backToFreeList(semd_PTR prevSema4)
{
    semd_PTR tempRemoval = prevSema4->s_next; /*Dummy pointer to next sema4 on ASL*/

    prevSema4->s_next = tempRemoval->s_next;
    tempRemoval->s_next = semdFree_h;

    tempRemoval->s_semAdd = NULL;
    semdFree_h = tempRemoval;
}