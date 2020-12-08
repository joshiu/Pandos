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

    if (status == 1)
    {
        /*if nothing is entered*/

        print(WRITETERMINAL, "\nUmmmm, you entered nothing? Is nothing even a palindrome?\n");
        print(WRITETERMINAL, " End of Palindrome Tester");

        /* Terminate */
        SYSCALL(TERMINATE, 0, 0, 0);
    }

    if ((status - 1) > 1)
    {
        /*if there are more than 1 chars*/

        if ((status - 1) % 2 == 0)
        { 
            /*if the length is even*/

            for (i = 0; i < ((status - 1) / 2); i++)
            {
                if (palindrome[i] != palindrome[status - 2 - i])
                {
                    /*if the ends are not equal, not a palindrome*/
                    test = FALSE;
                }
            }
        }

        for (i = 0; i < (int)((status - 1) / 2) + 1; i++)
        {
            /*if the length is odd*/

            if (palindrome[i] != palindrome[status - 2 - i])
            {
                /*if the ends are not equal, not a palindrome*/
                test = FALSE;
            }
        }
    }

    if (test == TRUE)
    {
        print(WRITETERMINAL, &palindrome[0]);
        print(WRITETERMINAL, "\nThis is a palindrome!\n");
        print(WRITETERMINAL, " End of Palindrome Tester");

        /* Terminate */
        SYSCALL(TERMINATE, 0, 0, 0);
    }

    print(WRITETERMINAL, &palindrome[0]);
    print(WRITETERMINAL, "\n Not a palindrome\n");
    print(WRITETERMINAL, " End of Palindrome Tester");
    /* Terminate  */
    SYSCALL(TERMINATE, 0, 0, 0);
    
}
