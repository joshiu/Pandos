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
/* Global Variables */
HIDDEN pcb_PTR pcbFree_h;
/*End of Global Variables*/
/*--------------------------------------------------------------------------------------------------------*/
/*-----------------------------------Below: methods for the queue----------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

/*This method inserts pointed to by p onto the freePCB list. This is done uing the insertProcQ method.*/
void freePcb(pcb_t*p){
    insertProcQ(&pcbFree_h,p);
}

/*Gives needed Pcb, then removes Pcb from the list (taken)
If the list is empty then NULL is returned*/
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

/*Initializes Pcb list: it creates a new empty list and contains all elements in the static array MAXPROC in the pcb*/
void initPcbs(){
    int i;
    pcbFree_h = mkEmptyProcQ();
    static pcb_t foo[MAXPROC];
    for( i=0; i<MAXPROC; i++){
        insertProcQ(&pcbFree_h,&foo[i]);
    }
}

/*Makes an empty list*/ 
pcb_t* mkEmptyProcQ(){
    return (NULL);
}

/*This method returns true if the tp is empty, otherwise: returns false.*/
int emptyProcQ(pcb_t*tp){
    return(tp == NULL);
}

/*This method inserts an element at the front of the queue*/
void insertProcQ(pcb_t**tp, pcb_t*p){
    pcb_t *head;
    if(emptyProcQ(*tp)){ /*if queue is empty...*/
        (*tp)-> p_next = p; /*the head points to what p points to*/
        (*tp) ->p_prev =p;
        (*tp) = p;/*the tail is whatever p point to*/
    }
    (*tp) -> p_next = head; 
    p->p_next = head;
    head -> p_prev = p;
    (*tp) -> p_next = p;
    p -> p_prev = (*tp);
    (*tp) = p;
}

/*This method removes the element at the head of the queue*/
pcb_t*removeProcQ(pcb_t**tp){
    pcb_t *pReturn = (*tp)->p_next; /*dummy pointer to the head*/
    if(emptyProcQ(*tp)){/*if there is nothing*/
        return(NULL);
    }
    if((*tp)->p_next == (*tp)){/*only 1 item in queue*/
        (*tp) -> p_next = NULL;
        (*tp) -> p_prev = NULL;
        (*tp) = NULL;
        return (pReturn);
    }
    /*if we have more than one thing*/
    pcb_t *newHead = pReturn ->p_next;
    pReturn->p_prev = NULL; /*get rid of pointer from head to tail*/
    (*tp) -> p_next = newHead; /*tail points to new head*/
    newHead ->p_prev = (*tp);/*new head points back to tail*/
    pReturn ->p_next =NULL;/*get rid of pointer from old head to new head*/
    return(pReturn);/*return old head*/
}

/*Points to an element in the queue and that element gets removed*/
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

/*Gives head from Queue, or returns null if empty*/
pcb_t*headProcQ(pcb_t*tp){
    if(emptyProcQ(tp)){
        return(NULL);
    }
    return(tp->p_next);
}


/*--------------------------------------------------------------------------------------------------------*/
/*---------------------------------Below: methods for process trees--------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/



/*This method returns true if the pcb that is pointed to by p has no children, otherwise returns false.*/
int emptyChild(pcb_t*p){
    return (p->p_child == NULL);
}

/*This method makes the pcb pointed to by p a child of the parent*/
void insertChild(pcb_t*prnt, pcb_t*p){
    if(emptyChild(prnt)){ /*if no other children, point at new child*/
        prnt ->p_child = p;
        p -> p_prnt = prnt;
    } /*if there are children: parent points to new child and new child points to sibling + parent.*/
    pcb_t *currentChild = prnt->p_child;
    p->p_sib_next = currentChild;
    currentChild ->p_sib_prev = p;
    prnt ->p_child = p;
    p->p_prnt = prnt;
}

/*This method removes the first child returns NULL if no children, otherwise returns pointer to this removed child*/
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

/*This method makes a child an orphan, and will become a subtree if it has children*/
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