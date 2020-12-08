/* Checks a user inputted palindrome */

#include "h/localLibumps.h"
#include "h/tconst.h"
#include "h/print.h"

void main()
{

    int status, i;
    char palindrome[40];
    int test;

    test = TRUE;

    print(WRITETERMINAL, "Palindrome Checker Started\n");
    print(WRITETERMINAL, "Enter a palindrome to check: ");

    status = SYSCALL(READTERMINAL, (int)&palindrome[0], 0, 0);
    palindrome[status] = EOS;

    
    for (i = 0; i < (int)(status / 2); i++)
    {
        /*if status is an odd number, the middle term is excluded*/

        if (palindrome[i] != palindrome[status - 1 - i])
        {
            /*if the ends are not equal, not a palindrome*/
            test = FALSE;
        }
    }

    if (test)
    {
        print(WRITETERMINAL, &palindrome[0]);
        print(WRITETERMINAL, "\n\nThis is a palindrome!\n");
        print(WRITETERMINAL, "\n\n End of Palindrome Tester\n");

        /* Terminate */
        SYSCALL(TERMINATE, 0, 0, 0);
    }

    print(WRITETERMINAL, &palindrome[0]);
    print(WRITETERMINAL, "\n\n Not a palindrome\n");
    print(WRITETERMINAL, "\n\n End of Palindrome Tester\n");
    /* Terminate  */
    SYSCALL(TERMINATE, 0, 0, 0);
}
