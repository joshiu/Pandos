/*	Test of a CPU intensive recusive job using factorials */

#include "h/localLibumps.h"
#include "h/tconst.h"
#include "h/print.h"

int factorial(int num){
    
    if(num ==1){
        return 1;
    }

    return(num * factorial(num-1));
}

void main(){
    int factorialNum;

    print(WRITETERMINAL, "The Factorial Test has started\n");

    factorialNum = factorial(10);

    if(factorialNum == 3628800){
        print(WRITETERMINAL, "Factorial Successful :D\n");
    }

    else{
        print(WRITETERMINAL, "Factorial Failed X_X\n");
    }

    SYSCALL(TERMINATE, 0, 0, 0);
}