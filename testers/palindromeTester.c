/* Checks a user inputted palindrome */

#include "h/localLibumps.h"
#include "h/tconst.h"
#include "h/print.h"


void main() {
	int status, i;
	char buf[40];
    int test;

    test = TRUE;
	
	print(WRITETERMINAL, "Palindrome Checker Started\n");
	print(WRITETERMINAL, "Enter a palindrome to check: ");
		
	status = SYSCALL(READTERMINAL, (int)&buf[0], 0, 0);
	buf[status] = EOS;
	
	
	for( i = 0; i < (int)(status/2); i++ )
	{
        if(buf[i] != buf[status-1-i]){
            test = FALSE;
        }
	}

    if(test){
        print(WRITETERMINAL, &buf[0]);
        print(WRITETERMINAL, "\n\nThis is a palindrome!\n");
        print(WRITETERMINAL, "\n\n End of Palindrome Tester\n");	
        /* Terminate normally */	
        SYSCALL(TERMINATE, 0, 0, 0);
    }

    print(WRITETERMINAL, &buf[0]);
    print(WRITETERMINAL, "\n\n Not a palindrome\n")
	print(WRITETERMINAL, "\n\n End of Palindrome Tester\n");	
	/* Terminate normally */	
	SYSCALL(TERMINATE, 0, 0, 0);
}

