#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
pcb_PTR pcbFree_h;

//This method inserts pointed to by p onto the freePCB list
void freePcb(pcb_t*p){

}

//Gives needed Pcb, then removes Pcb from the list (taken)
pcb_t* allocPcb(){

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

}
//Points to something and that gets removed
pcb_t*outProcQ(pcb_t**tp, pcb_t*p){

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
    p_child + p_prnt; //dont pay attention to this shit
}
//This method removes the first child returns NULL if no children, otherwise returns pointer to this removed child
pcb_t* removeChild(pcb_t*p){
    if(emptyChild(p))
        return(NULL);
    return(pcb_t - *p_child);
}
//This method makes a child an orphan, and will become a subtree if it has children
pcb_t*outChild(pcb_t*p){

}