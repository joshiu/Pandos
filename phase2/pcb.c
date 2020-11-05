#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
#include "../h/asl.h"

/**
 * This file makes and maintains a PCB queue which is circular and 
 * doubly linked, as well as a process tree that is circular and 
 * doubly linked between the parent and youngest child, while the 
 * rest of the children are linearly singularly linked to the parent
 * and the children are linearly doubly linked to each other.
 * 
 * The free list  contains all freePCBs with a maximum of 20 pcbs. 
 * Pcbs can be allocated from the free list and "freed" back.
 * In the process tree, children can be added and removed from the tree. 
 * Note, p_sib_prev points to the "older" child and p_sib_next points to 
 * the "younger" children. 
 * 
 * This code is written by Umang Joshi and Amy Kelley, with help from Mikey G.
 **/

/* Global Variables */
HIDDEN pcb_PTR pcbFree_h; /*contains all unused free pcbs*/
/*End of Global Variables*/

/*--------------------------------------------------------------------------------------------------------*/
/*-----------------------------------Below: methods for the queue----------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

/**
 * This section makes a PCB queue which is doubly linked and circular.
 * There is a free list that contains all freePCBs, and these pcbs are 
 * allocated from the list and added to a queue. A maximum of 20 pcbs can
 * be allocated.  
 **/

/**
 * This method inserts the element pointed to by p onto the pcbFree list.
 * This is done using the insertProcQ method.
**/
void freePcb(pcb_t *p)
{
    insertProcQ(&pcbFree_h, p);
}

/**
 * Remove an element from the pcbFree list, provide initial values for ALL
 * of the pcbs fields and then return a pointer to the removed element.
**/
pcb_t *allocPcb()
{
    if (pcbFree_h == NULL)
    { /*if the pcbFree list is empty, return NULL*/
        return NULL;
    }

    /*initialize all our pointers*/
    pcb_t *p = removeProcQ(&pcbFree_h); /*set the pointer to point to the removed pcb*/
    p->p_next = NULL;
    p->p_prev = NULL;

    p->p_semAdd = NULL;

    p->p_prnt = NULL;
    p->p_child = NULL;
    p->p_sib_next = NULL;
    p->p_sib_prev = NULL;

    p->p_time = 0;
    p->p_supportStruct = NULL;

    return (p); /*return the pointer*/
}

/**
 * Initialize the pcbFree list to be a static array that 
 * contains all 20 pcbs in the. This method will be called 
 * only once during data structure initialization.
**/
void initPcbs()
{
    int i;

    pcbFree_h = mkEmptyProcQ();

    static pcb_t populate[MAXPROC];

    for (i = 0; i < MAXPROC; i++)
    {

        insertProcQ(&pcbFree_h, &populate[i]);
    }
}

/**
 * This method is used to create a null queue and initializes
 * the tail pointer to NULL for the process queue.
 * Returns a pointer to the tail of an empty process queue.
**/
pcb_t *mkEmptyProcQ()
{

    return (NULL);
}

/**
 * This method returns true if the given queue is empty,
 * otherwise it returns false.
**/
int emptyProcQ(pcb_t *tp)
{

    return (tp == NULL);
}

/**
 * Insert the pcb pointed to by p into the process queue whose 
 * tail pointer is tp. Note the queue is doubly linked to allow
 * for a simulanteous updating of the tail and head. 
 * Note: the last entry is the tail, and tp->p_prev is the head
 * and tp->p_next is the previous tail. 
**/
void insertProcQ(pcb_t **tp, pcb_t *p)
{

    if (emptyProcQ(*tp))
    { 
        /*if queue is empty...*/

        p->p_next = p; /*the head points to what p points to*/
        p->p_prev = p;

        (*tp) = p; /*the tail is whatever p point to*/
        return;
    }

    /*if the queue has one or more element(s) */
    pcb_t *head = headProcQ(*tp); /*Dummy pointer to the head of the queue.*/

    p->p_prev = head;
    head->p_next = p;

    (*tp)->p_prev = p;
    p->p_next = (*tp);

    (*tp) = p; /*this is the issue?*/
    return;
}

/**
 * This method removes the head from the process queue whose 
 * tail-pointer is tp. Return NULL if the process queue is empty; 
 * otherwise return the pointer to the removed element. 
 * Update the process queue’s tail pointer if necessary.
**/
pcb_t *removeProcQ(pcb_t **tp)
{

    if (emptyProcQ(*tp))
    { 
        /*if there is nothing*/

        return NULL;
    }

    pcb_t *head = headProcQ(*tp); /*dummy pointer to the head*/

    if (head == (*tp))
    { 
        /*only 1 item in queue*/

        (*tp)->p_next = NULL;
        (*tp)->p_prev = NULL;
        (*tp) = NULL;

        return (head);
    }

    /*if we have more than one process*/

    pcb_t *newHead = head->p_prev; /*Dummy pointer to the new head*/

    head->p_prev = NULL;
    (*tp)->p_prev = newHead;
    newHead->p_next = (*tp);

    head->p_next = NULL;

    return (head); /*return old head*/
}

/**
 *Remove the pcb pointed to by p from the process queue whose 
 *tail pointer is tp. Update the process queue’s tail pointer 
 *if necessary. If the desired entry is not in the indicated 
 *queue (an error condition), return NULL; otherwise, find, 
 *remove, and return p. 
**/
pcb_t *outProcQ(pcb_t **tp, pcb_t *p)
{
    int i;

    pcb_t *dumTail = (*tp); /*dummy pointer to tail*/

    if (emptyProcQ(*tp))
    { 
        /*if the queue is empty return NULL*/

        return NULL;
    }

    if (headProcQ(*tp) == p)
    {
        /*if the head is the tail then call removeProcQ */

        return removeProcQ(tp);
    }

    for (i = 0; i < MAXPROC; i++)
    {
        if (dumTail != p)
        {
            dumTail = dumTail->p_prev; /*iterative call that shifts dumTail*/

            continue;
        }

        pcb_t *forwardTo = dumTail->p_next;  /*Dummy pointer to the next entry*/
        pcb_t *backwardTo = dumTail->p_prev; /*Dummy pointer to the prev entry*/

        backwardTo->p_next = forwardTo;
        forwardTo->p_prev = backwardTo;
        
        dumTail->p_next = NULL;
        dumTail->p_prev = NULL;
        
        return (dumTail); /*return pointer to removed*/
    }

    return (NULL); /*p is not on the list, so NULL*/

}

/**
 *Return a pointer to the head of the process queue whose tail
 *is tp. Do not remove this pcb from the process queue, just 
 *return a pointer. Return NULL if the process queue is empty.
**/
pcb_t *headProcQ(pcb_t *tp)
{
    if (emptyProcQ(tp))
    {
        return (NULL);
    }
    return (tp->p_prev);
}

/*--------------------------------------------------------------------------------------------------------*/
/*---------------------------------Below: methods for process trees--------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

/**
 * This section is for the process tree of the program. We create a tree
 * where the parents and the newest child are doubly linked and circular.
 *  
 * The rest of the childern point to the parent (singly linked linear).
 * 
 * The children point to one another in a doubly linked linear list. 
 * 
 * Note: p_sib_prev points to the "older" sibling. While p_sib_next points 
 * to the "younger" sibling. So, the youngest child has no p_sib_next and 
 * the oldest has no p_sib_prev. 
 * */


/**
 *This method returns TRUE if the pcb pointed to by p has no children. Return
 *FALSE otherwise.
**/
int emptyChild(pcb_t *p)
{
    return (p->p_child == NULL);
}

/**
 * This method makes the pcb p a child of the pcb prnt, which 
 * adds to the process tree and gives the prnt its most recent child.
 * If there are no children, one is added; if there are children, one 
 * is added and the rest are shifted. 
**/
void insertChild(pcb_t *prnt, pcb_t *p)
{
    if (emptyChild(prnt))
    { 
        /*if no other children, point at new child*/
        prnt->p_child = p;
        p->p_prnt = prnt;
        p->p_sib_next = NULL;
        p->p_sib_prev = NULL;
        return;
    }
    if (!emptyChild(prnt))
    {
        /*there are children*/

        pcb_t *currentChild = prnt->p_child; /*dummy pointer to current child*/

        p->p_sib_prev = currentChild; /*new one points to sibling*/
        p->p_sib_next = NULL; /*no younger sibling for new one*/
        currentChild->p_sib_next = p;/*older points to newer*/

        prnt->p_child = p;/*parent points back*/
        p->p_prnt = prnt;/*point to the parent*/
        return;
    }
}

/**
 * This method makes the first/youngest child of the parent 
 * pcb p no longer a child of p. Return NULL if there were no 
 * children of p. Otherwise, return a pointer to the removed child pcb.
**/
pcb_t *removeChild(pcb_t *p)
{
    if (emptyChild(p))
    { 
        /* if there are no children*/
        return NULL;
    }

    if (p->p_child->p_sib_prev == NULL)
    {
        /*if there is one child*/

        pcb_t *temp = p->p_child; /*dummy pointer to the first child*/

        p->p_child = NULL; /*remove the child*/

        temp->p_prnt = NULL;

        return (temp);
    }

    /*more than one child*/

    pcb_t *removeFirst = p->p_child;/*dummy pointer to child we want to remove*/
    pcb_t *firstSib = removeFirst->p_sib_prev; /*dummy pointer to the older sibling*/

    firstSib->p_sib_next = NULL; /*older has no younger sib*/
    p->p_child = firstSib; /*older sib is now youngest child*/

    removeFirst->p_sib_next = NULL;/*remove the ex-first child*/
    removeFirst->p_sib_prev = NULL;
    removeFirst->p_prnt = NULL;

    return (removeFirst); /*return removed child*/
}

/**
 * This method makes the pcb p no longer the child of its parent.
 * Ensure p has a parent, then return p. If p has no parent or p 
 * is NULL, return NULL
**/
pcb_t *outChild(pcb_t *p)
{
    if (p == NULL)
    {
        return NULL;
    }

    if (p->p_prnt == NULL)
    { 
        /*if you don't have a parent then you are already an orphan*/
        return NULL;
    }

    pcb_t *parent = p->p_prnt; /*dummy pointer to the parent*/

    if (p == parent->p_child)
    { 
        /*if you are the first and only child*/
        return (removeChild(parent));
    }

    if (p != parent->p_child)
    { 
        /*not the first child*/

        if (p->p_sib_prev == NULL)
        { 
            /*you are the oldest, so no prev sibling*/
            p->p_prnt = NULL;

            pcb_t *nextSib = p->p_sib_next; /*dummy pointer to next sibling*/

            nextSib->p_sib_prev = NULL;
            p->p_sib_next = NULL;

            return (p); /*return child*/
        }

        /*if you are a middle child, so have sib_next and sib_prev*/
        p->p_prnt = NULL;

        pcb_t *prevSib = p->p_sib_prev; /*dummy pointer to previous sibling*/
        pcb_t *nextSib = p->p_sib_next; /*dummy pointer to next sibling*/

        prevSib->p_sib_next = nextSib;
        nextSib->p_sib_prev = prevSib;

        p->p_sib_next = NULL;
        p->p_sib_prev = NULL;
        
        return (p); /*return orphaned child*/
    }
 
    return NULL; /*case if p doesn't belong*/

}