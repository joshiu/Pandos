#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
pcb_PTR pcbFree_h;

//This method inserts pointed to by p onto the freePCB list
void freePcb(pcb_t*p){

}

//Gives needed Pcb, then removes Pcb from the list (taken)
pcb_t* allocPcb(){
    if(){//if the pcbFree list is empty, return NULL
        return NULL;
    }//otherwise remove element from pcb list, then return a pointer to the removed element
    //remember that pcbs are reused, so make sure no value is in the pcb when its relocated, so if relocated -> remove all in list?
}
//intilize Pcb list
void initPcbs(){
    pcbFree_h = NULL;
    static pcb_t foo[MAXPROC];
    for(int i = 0; i<MAXPROC; i++){
        freePcb(&(foo[i]));
    }
}
//Makes list empty :) like my soul
pcb_t* mkEmptyProcQ(){
    return (NULL);
}
//This method returns true if the tp is empty, otherwise: returns false
int emptyProcQ(pcb_t*tp){
    return(tp == NULL);
}
//This method inserts an element at the front of the queue
void insertProcQ(pcb_t**tp, pcb_t*p){
    if(emptyProcQ(tp) == TRUE){
        tp= p;
        //find a way to make it point back to itself
    }
    p-> (tp->p_next); //p needs to point to the head (tp-> p_next)
    (tp->p_next) -> p; //tp-> p_next needs to point back to p
    tp->p; //have the old tail point to new tail
    p->tp; //have the new tail point back
    tp = p; //assign tp to the new tail
}
//This method removes the element at the front of the queue
pcb_t*removeProcQ(pcb_t**tp){
    if(emptyProcQ(tp)==TRUE){//if there is nothing
        return(NULL);
    }
    if(tp->p_next == tp){ //if we point to ourseleves
    mkEmptyProcQ();
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
    if(emptyProcQ(tp))
    return(NULL);

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