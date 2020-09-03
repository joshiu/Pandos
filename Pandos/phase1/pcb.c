#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
HIDDEN pcb_PTR pcbFree_h;

//This method inserts pointed to by p onto the freePCB list
void freePcb(pcb_t*p){
    insertProcQ(pcbFree_h,p);
}

//Gives needed Pcb, then removes Pcb from the list (taken)
pcb_t* allocPcb(){
    if(emptyProcQ(tp) == TRUE){//if the pcbFree list is empty, return NULL
        return NULL;
    }//otherwise remove element from pcb list, then return a pointer to the removed element
    //remember that pcbs are reused, so make sure no value is in the pcb when its relocated, so -> set proprieties to NULL
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
    (tp->p_prev) -> (tp->p_next);//second last item points to head
    (tp->p_next) -> (tp->p_prev);//head points to second last
    tp = tp->p_prev;//make the second last the new tail
    //get rid of the tail (add a return as well)
}

//Points to something and that gets removed
pcb_t*outProcQ(pcb_t**tp, pcb_t*p){
    if(tp == p){
        removeProcQ(*tp);
    }
    (p->p_prev) -> (p->p_next); //have the previous one point to the next one
    (p->p_next) -> (p->p_previous); //have the next point to the previous
    //get rid of the thing (add a return)
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
    if(emptyChild(prnt) == TRUE){ //if no other children, point at new child
        prnt -> p;
        p -> prnt;
    } //if their are children: parent points to new child and new child points to sibling + parent. 
    prnt -> p_child; //how do you get this to be siblings?? halp
    p -> p_child;
    p_child -> p;
    prnt -> p_child;
    p -> prnt;
}

//This method removes the first child returns NULL if no children, otherwise returns pointer to this removed child
pcb_t* removeChild(pcb_t*p){//where does this point to parent or child?
    if(emptyChild(p)){
        return (NULL);
    }
    if(){//there is one child
    //delete the child and parent pointers
    }
    //more than 1 child
    p->p_child;
    //delete the previous pointer that pointed from parent to child
    p->p_sib;
    //delete pointer that points back
    //save the new child location (so tail pointer)
    p->p_prnt;
    //have the parent point to the new pointer

}

//This method makes a child an orphan, and will become a subtree if it has children
pcb_t*outChild(pcb_t*p){


}