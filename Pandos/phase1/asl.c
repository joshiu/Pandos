#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"
#include "../h/pcb.h"


//local functions
HIDDEN semd_PTR actSemd (semd_PTR, int *semAdd);
HIDDEN void freeSemd (semd_PTR);
HIDDEN int inactiveSemd (semdPTR, int semAdd);
HIDDEN semdPTR findDesc (int *semAdd);
//end of local funnctions

//globals

//end of globals

int insertBlocked (int *semAdd, pcb_PTR p){

}

pcb_PTR removeBlocked (int *semAdd){
    //get descriptior -> semdtrail
    semd = semdTrail -> s_next
    if(inactiveSemd){ //to do: write inactive
        return NULL
    }
    temp = removeProcQ(&semAdd -> sprocQ)
    temp -> p_Add
    remove(&semAdd -> s_proQ, p)
    return (FALSE);
    //insert, same as remove
    //out same as remove (return p)
}

pcb_PTR outBlocked (pcb_PTR p){

}

pcb_PTR headBlocked (int *semAdd){
    semAdd = findDesc semAdd
    semAdd = semAdd -> s_next
    if(inactiveSemd(semAdd, semAdd)){
        return NULL
    }
    return headProcQ (semAdd -> s.s_ProcQ)
}

void initASL (){
    static semd [MAXPROC+2]
    semdFree_h = & semdTable[0]
    //go thru and initialize ptr
    semdTable[i-1].s_next = & semTable[i]
    set[MAXPROC-1].s.next = NULL

    //dummy nodes (?)
    semd_h ->s.semAdd = 0
    mkEmptyProcQ(s_ProcQ)
    //to do: go to end and set null then reun make empty and set to null
}

//to do: where to add findDesc? -> pesudo code in Umang's notes
//also add comments >:c