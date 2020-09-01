#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"

void freePcb(pcb_t*p){

}
pcb_t *allocPcb(){

}
void initPcbs(){
    // pcbfree_h = NULL;
    static pcb_t foo[MAXPROC];
    for(int i = 0; i<MAXPROC; i++){
        freePcb(&foo[i]);
    }
}
pcb_t *mkEmptyProcQ(){
    return NULL;
}
int emptyProcQ(pcb_t*tp){
    return(tp == NULL);
}
void insertProcQ(pcb_t**tp, pcb_t*p){

}
pcb_t*removeProcQ(pcb_t**tp){

}
pcb_t*outProcQ(pcb_t**tp, pcb_t*p){

}
pcb_t*headProcQ(pcb_t*tp){
    if(emptyProcQ(tp))
    return(NULL);

    return(tp->p_next);
}
int emptyChild(pcb_t*p){

}
void insertChild(pcb_t*prnt, pcb_t*p){

}
pcb_t*removeChild(pcb_t*p){

}
pcb_t*outChild(pcb_t*p){

}