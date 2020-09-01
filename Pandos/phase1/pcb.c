#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"

//This method inserts pointed to by p onto the freePCB list
void freePcb(pcb_t*p){

}

//Gives needed Pcb, then removes Pcb from the list (taken)
pcb_t *allocPcb(){

}
//
void initPcbs(){
    // pcbfree_h = NULL;
    static pcb_t foo[MAXPROC];
    for(int i = 0; i<MAXPROC; i++){
        freePcb(&foo[i]);
    }
}
//
pcb_t *mkEmptyProcQ(){
    return NULL;
}
//This method returns true if the tp is empty, otherwise: returns false
int emptyProcQ(pcb_t*tp){
    return(tp == NULL);
}
//This method inserts an element at the front of the queue
void insertProcQ(pcb_t**tp, pcb_t*p){


}
//This method removes the element at the front of the queue
pcb_t*removeProcQ(pcb_t**tp){

}
//
pcb_t*outProcQ(pcb_t**tp, pcb_t*p){

}
//
pcb_t*headProcQ(pcb_t*tp){
    if(emptyProcQ(tp))
    return(NULL);

    return(tp->p_next);
}
//This method returns true if the pcb that is pointed to by p has no children, otherwise returns false.
int emptyChild(pcb_t*p){
    return (p_child == NULL);
}
//This method makes the pcb pointed to by p a child of the parent
void insertChild(pcb_t*prnt, pcb_t*p){
    *p_child + *p_prnt; //dont pay attention to this shit
}
//This method removes the first child returns NULL if no children, otherwise returns pointer to this removed child
pcb_t*removeChild(pcb_t*p){
    if(emptyChild(pcb_t*p){
    return(NULL);
    }else{
    return(pcb_t - *p_child);
    }
}
//This method makes a child an orphan, and will become a subtree if it has children
pcb_t*outChild(pcb_t*p){

}