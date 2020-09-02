#include "../h/const.h"
#include "../h/types.h"
#include "../h/asl.h"


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

}

pcb_PTR outBlocked (pcb_PTR p){

}

pcb_PTR headBlocked (int *semAdd){

}

void initASL (){

}