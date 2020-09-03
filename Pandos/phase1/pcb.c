#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
#include "../h/asl.h"
HIDDEN pcb_PTR pcbFree_h;

//This method inserts pointed to by p onto the freePCB list
void freePcb(pcb_t*p){
    insertProcQ(pcbFree_h,p);
}

//Gives needed Pcb, then removes Pcb from the list (taken)
pcb_t* allocPcb(){
    if(pcbFree_h == NULL){//if the pcbFree list is empty, return NULL
        return NULL;
    }
    else{
        pcb_t *p =NULL;
        p->p_next =NULL;
        p->p_prev =NULL;
        p->p_semAdd =NULL;
    }
  return; //find out what to return?
}

//intilize Pcb list
void initPcbs(){
    pcbFree_h = mkEmptyProcQ();
    static pcb_t foo[MAXPROC];
    for(int i = 0; i<MAXPROC; i++){
        insertProcQ(&pcbFree_h,&foo[i]);
    }
}

//Makes an empty list 
pcb_t* mkEmptyProcQ(){
    return (NULL);
}

//This method returns true if the tp is empty, otherwise: returns false
int emptyProcQ(pcb_t*tp){
    return(tp == NULL);
}

//This method inserts an element at the front of the queue
void insertProcQ(pcb_t**tp, pcb_t*p){
    pcb_t *head;
    if(emptyProcQ(tp)){
        (*tp)-> p_next = p; //the head points to what p points to
        (*tp) ->p_prev =p;
        (*tp) = p;//the tail is whatever p point to
    }
    (*tp) -> p_next = head;
    p->p_next = head;
    head -> p_prev = p;
    (*tp) -> p_next = p;
    p -> p_prev = (*tp);
    (*tp) = p;
}

//This method removes the element at the front of the queue
pcb_t*removeProcQ(pcb_t**tp){
    if(emptyProcQ(tp)){//if there is nothing
        return(NULL);
    }
    if((*tp)->p_next == (*tp)){
        (*tp) -> p_next = NULL;
        (*tp) -> p_prev = NULL;
        (*tp) = NULL;
    }
    //if we have more than one thing
    pcb_t *head = (*tp)->p_next;
    pcb_t *newHead = head ->p_next;
    head->p_prev = NULL; //get rid of pointer to tail
    (*tp) -> p_next = newHead; //tail points to new head
    newHead ->p_prev = (*tp);//new head points back to tail
    head ->p_next =NULL;//get rid of pointer to new head
    //what does this return?
}

//Points to something and that gets removed
pcb_t*outProcQ(pcb_t**tp, pcb_t*p){
    pcb_t *head = (*tp)->p_next;//dummy pointer for head
    if(emptyProcQ(*tp)){
        return NULL;
    }
    if(head = p){
        removeProcQ(*tp);
    }
    for(int i =0; i<MAXPROC; i++){
        if(head != p ){
            head ->p_next = head;
            continue;
        }
        pcb_t *forwardTo = head ->p_next;
        pcb_t *backwardTo = head ->p_prev;
        backwardTo ->p_next = forwardTo;
        forwardTo ->p_prev = backwardTo;
        head->p_next=NULL;
        head ->p_prev =NULL;
        return; //idk
    }
    return(NULL); //p is not on the list, so NULL
}

//Gives head from Queue, or returns null if empty
pcb_t*headProcQ(pcb_t*tp){
    if(emptyProcQ(tp)){
        return(NULL);
    }
    return(tp->p_next);
}

//This method returns true if the pcb that is pointed to by p has no children, otherwise returns false.
int emptyChild(pcb_t*p){
    return (p->p_child == NULL);
}

//This method makes the pcb pointed to by p a child of the parent
void insertChild(pcb_t*prnt, pcb_t*p){
    if(emptyChild(prnt)){ //if no other children, point at new child
        prnt ->p_child = p;
        p -> p_prnt = prnt;
    } //if their are children: parent points to new child and new child points to sibling + parent. 
    pcb_t *currentChild = prnt->p_child;
    p->p_sib_next = currentChild;
    currentChild ->p_sib_prev = p;
    prnt ->p_child = p;
    p->p_prnt = prnt;
}

//This method removes the first child returns NULL if no children, otherwise returns pointer to this removed child
pcb_t* removeChild(pcb_t*p){//pointer points to parent?'
    pcb_t *removeFirst = p->p_child;
    if(emptyChild(p)){
        return (NULL);
    }
    if((p->p_child)->p_sib_next ==NULL){//there is one child
        removeFirst ->p_prnt= NULL;
        p->p_child =NULL;
    }
    //more than one child
    p ->p_child = p; //have p point to the child
    pcb_t *parent = p->p_prnt;
    pcb_t *firstSib = p->p_sib_next;
    firstSib ->p_sib_prev =NULL;//stops next sib from pointing to p
    parent ->p_child = firstSib; //set parents first child as sib
    p->p_sib_next =NULL; //make p have no siblings
    p->p_prnt =NULL; //only issue here is that p is now pointing to the orphanized child and not the parent...
    return; //idk
}

//This method makes a child an orphan, and will become a subtree if it has children
pcb_t*outChild(pcb_t*p){//pointer points to child
    if(p->p_prnt ==NULL){
        return NULL;
    }
    pcb_t *parent = p->p_prnt;
    if(p->p_prnt != parent->p_child){//if I point to my parent and my parent doesn't point back
        if(p->p_sib_next ==NULL){
            p->p_prnt=NULL;
            pcb_t *prevSib = p->p_sib_prev;
            prevSib ->p_sib_next = NULL;
            p->p_sib_prev = NULL;
            return; //idk
        }
        p->p_prnt = NULL;
        pcb_t *prevSib = p->p_sib_prev;
        pcb_t *nextSib = p->p_sib_next;
        prevSib ->p_sib_next = nextSib;
        nextSib ->p_sib_prev = prevSib;
        p->p_sib_next =NULL;
        p->p_sib_prev =NULL;
        return; //idk
    }
    return(removeChild(parent)); //child is first child, so use this method
}