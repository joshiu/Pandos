/*
 * This file contains the freePCB list, and methods that helps it maintain and manipulate the queue. 
 * It also contains the process trees that also organize the pcbs. The methods for this section are used to support 
 * and manipulate the process trees.
 * 
 * 
 * This code is written by Umang Joshi and Amy Kelley.
 */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
#include "../h/asl.h"

/**
 * Method for debugging for seeing a value or seeing where we are.
**/
void debugA(int a){
    int i;
    i = 0;
}

/* Global Variables */
HIDDEN pcb_PTR pcbFree_h;
/*End of Global Variables*/

/*--------------------------------------------------------------------------------------------------------*/
/*-----------------------------------Below: methods for the queue----------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

/**
 * This method inserts "the element pointed to by p onto the pcbFree list."
 * This is done uing the insertProcQ method.
**/
void freePcb(pcb_t*p){
    insertProcQ(&pcbFree_h,p);
}

/**
 * "Remove an element from the pcbFree list, provide initial values for ALL
 * of the pcbs fields (i.e. NULL and/or 0) and then return a pointer
 * to the removed element. pcbs get reused, so it is important that
 * no previous value persist in a pcb when it gets reallocated"
 * Gives needed Pcb, then removes Pcb from the list
 * If the list is empty then NULL is returned.
**/
pcb_t* allocPcb(){
    if(pcbFree_h == NULL){/*if the pcbFree list is empty, return NULL*/
        return NULL;
    }
    /*initialize all our pointers*/
    pcb_t *p =NULL;
    p->p_next =NULL;
    p->p_prev =NULL;
    p->p_semAdd =NULL;
    p->p_prnt =NULL;
    p->p_child = NULL;
    p->p_sib_next = NULL;
    p->p_sib_prev =NULL;
    p=removeProcQ(&pcbFree_h);/*set the pointer to point to the removed pcb*/
    return (p);/*return the pointer*/
}

/**
 *"Initialize the pcbFree list to contain all the elements of the
 *static array of MAXPROC pcbs. This method will be called only
 *once during data structure initialization"
 *It creates a new empty list and contains all elements in the static array MAXPROC in the pcb.
**/
void initPcbs(){
    int i;
    pcbFree_h = mkEmptyProcQ();
    static pcb_t foo[MAXPROC];
    for( i=0; i<MAXPROC; i++){
        debugA(i); /* for debugging initPcb -> see whats the value of i*/
        insertProcQ(&pcbFree_h,&foo[i]);
    }
}

/**
 *"This method is used to initialize a variable to be tail pointer to a
 *process queue. Return a pointer to the tail of an empty process queue; i.e. NULL"
 *Makes an empty list
**/
pcb_t* mkEmptyProcQ(){
    return (NULL);
}

/**
 * This method returns true if the tp is empty, otherwise: returns false.
**/
int emptyProcQ(pcb_t*tp){
    return(tp == NULL);
}

/**
 * "Insert the pcb pointed to by p into the process queue whose tail pointer is pointed to by tp. 
 * Note the double indirection through tp to allow for the possible updating of the tail pointer as well"
 * This method inserts an elements at the tail.
**/
void insertProcQ(pcb_t**tp, pcb_t*p){
    pcb_t *head;/*dummy node to keep track of head*/
    if(emptyProcQ(*tp)){ /*if queue is empty...*/
        p-> p_next = p; /*the head points to what p points to*/
        p->p_prev =p;
        (*tp) = p;/*the tail is whatever p point to*/
    }
    (*tp) -> p_next = head; 
    p->p_next = head;
    head -> p_prev = p;
    (*tp) -> p_next = p;
    p -> p_prev = (*tp);
    (*tp) = p;
}

/**
 * This method removes the head "from the process queue whose tail-pointer is pointed to by tp. 
 * Return NULL if the process queue was initially empty; 
 * otherwise return the pointer to the removed element. Update the process queue’s tail pointer if necessary"
**/
pcb_t*removeProcQ(pcb_t**tp){
    pcb_t *head = (*tp)->p_next; /*dummy pointer to the head*/
    if(emptyProcQ(*tp)){/*if there is nothing*/
        return(NULL);
    }
    if(head == (*tp)){/*only 1 item in queue*/
        (*tp) -> p_next = NULL;
        (*tp) -> p_prev = NULL;
        (*tp) = NULL;
        return (head);
    }
    /*if we have more than one thing*/
    pcb_t *newHead = head ->p_next;
    head->p_prev = NULL; /*get rid of pointer from head to tail*/
    (*tp) -> p_next = newHead; /*tail points to new head*/
    newHead ->p_prev = (*tp);/*new head points back to tail*/
    head ->p_next =NULL;/*get rid of pointer from old head to new head*/
    return(head);/*return old head*/
}

/**
 *"Remove the pcb pointed to by p from the process queue whose tailpointer is pointed to by tp. 
 *Update the process queue’s tail pointer if
 *necessary. If the desired entry is not in the indicated queue (an error
 *condition), return NULL; otherwise, return p. "
 *This method points to any element in the queue and that element gets removed
**/
pcb_t*outProcQ(pcb_t**tp, pcb_t*p){
    int i;
    pcb_t *removeQ = (*tp)->p_next;/*dummy pointer for head*/
    if(emptyProcQ(*tp)){ /*if the queue is empty return NULL*/
        return NULL;
    }
    if(removeQ == p){ /*if the head is the pointer then call removeProcQ on tp*/
        removeProcQ(tp);
    }
    for(i=0; i<MAXPROC; i++){
        if(removeQ != p ){
            removeQ ->p_next = removeQ;
            continue;
        }
        pcb_t *forwardTo = removeQ ->p_next;/*dummy pointer to next item*/
        pcb_t *backwardTo = removeQ ->p_prev;/*dummy pointer to previous item*/
        backwardTo ->p_next = forwardTo;/*have previous point to next*/
        forwardTo ->p_prev = backwardTo;/*have next point to previous*/
        removeQ->p_next=NULL; /*remove pointer to next item*/
        removeQ ->p_prev =NULL;/*remove pointer to previous item*/
        return(removeQ); /*return pointer to removed*/
    }
    return(NULL); /*p is not on the list, so NULL*/
}

/**
 *"Return a pointer to the first pcb from the process queue whose tail
 *is pointed to by tp. Do not remove this pcbfrom the process queue.
 *Return NULL if the process queue is empty"
 *Gives head from Queue, or returns null if empty
**/
pcb_t*headProcQ(pcb_t*tp){
    if(emptyProcQ(tp)){
        return(NULL);
    }
    return(tp->p_next);
}


/*--------------------------------------------------------------------------------------------------------*/
/*---------------------------------Below: methods for process trees--------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/



/**
 *This method returns "TRUE if the pcb pointed to by p has no children. Return
 *FALSE otherwise"
**/
int emptyChild(pcb_t*p){
    return (p->p_child == NULL);
}

/**
 * *This method "Make the pcb pointed to by p a child of the pcb pointed to by prnt"
**/
void insertChild(pcb_t*prnt, pcb_t*p){
    if(emptyChild(prnt)){ /*if no other children, point at new child*/
        prnt ->p_child = p;
        p -> p_prnt = prnt;
    } /*if there are children: parent points to new child and new child points to sibling + parent.*/
    pcb_t *currentChild = prnt->p_child;/*dummy pointer to current child*/
    p->p_sib_next = currentChild;
    currentChild ->p_sib_prev = p;
    prnt ->p_child = p;
    p->p_prnt = prnt;
}

/**
 * This method makes "the first child of the pcb pointed to by p no longer a child of
 * p. Return NULL if initially there were no children of p. Otherwise,
 * return a pointer to this removed first child pcb."
**/
pcb_t* removeChild(pcb_t*p){/*pointer points to parent*/
    pcb_t *removeFirst = p->p_child;/*dummy pointer that points to child we want to remove*/
    if(emptyChild(p)){/*call emptyChild to see if there are children*/
        return (NULL);
    }
    if((p->p_child)->p_sib_next ==NULL){/*if there is one child*/
        removeFirst ->p_prnt= NULL;
        p->p_child =NULL;
        return(removeFirst);
    }
    /*more than one child*/
    pcb_t *firstSib = removeFirst->p_sib_next;/*dummy pointer to the next sibling*/
    firstSib ->p_sib_prev =NULL;/*stops next sib from pointing to p*/
    p ->p_child = firstSib; /*set parent's first child as sib*/
    removeFirst->p_sib_next =NULL; /*make former first child have no siblings*/
    removeFirst->p_prnt =NULL; /*make former first child have no parents*/
    return(removeFirst); /*return removed child*/
}

/**
 * This method "Make the pcb pointed to by p no longer the child of its parent." (an orphan) 
 * "If the pcb pointed to by p has no parent, return NULL; otherwise, return
 * p. Note that the element pointed to by p need not be the first child of
 * its parent"
**/
pcb_t*outChild(pcb_t*p){/*pointer points to child*/
    if(p->p_prnt ==NULL){ /*if you don't have a parent then you are already an orphan*/
        return NULL;
    }
    pcb_t *parent = p->p_prnt;/*dummy pointer to the parent*/

    if(p->p_prnt != parent->p_child){/*if not the first child*/
        if(p->p_sib_next ==NULL){/*you are an end child, so no next sibling*/
            p->p_prnt=NULL;
            pcb_t *prevSib = p->p_sib_prev;/*dummy pointer to previous sibling*/
            prevSib ->p_sib_next = NULL;
            p->p_sib_prev = NULL;
            return(p); /*return orphaned child*/
        }/*if you are a middle child, so have sib_next and sib_prev*/
        p->p_prnt = NULL;
        pcb_t *prevSib = p->p_sib_prev;/*dummy pointer to previous sibling*/
        pcb_t *nextSib = p->p_sib_next;/*dummy pointer to next sibling*/
        prevSib ->p_sib_next = nextSib;/*point previous to next sibling*/
        nextSib ->p_sib_prev = prevSib;/*point next sibling to previous*/
        p->p_sib_next =NULL;/*remove p's sibling relations*/
        p->p_sib_prev =NULL;
        return(p); /*return orphaned child*/
    }
    return(removeChild(parent)); /*child is first child, so use removeChild*/
}