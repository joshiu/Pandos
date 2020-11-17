#ifndef CONSTS
#define CONSTS

/**************************************************************************** 
 *
 * This header file contains utility constants & macro definitions.
 * 
 ****************************************************************************/
/*Status Register bit constants*/
#define ALLOFF            0x00000000
#define IEPREVON          0x00000004 /*previous interrupt bit on*/
#define IMASKON           0x0000FF00 /*interrupt masking turned on*/
#define TIMEREBITON       0x08000000 /*timer enable bit on*/
#define IECURRENTON       0x00000001 /*current interrupt bit on*/
#define USERPREVON        0x00000008 /*user bit previous is on*/

/*Cause Register bit constants*/
#define GETCAUSE          0x0000007C /*keep on cause bits*/
#define CLEARCAUSE        0xFFFFF00 /*clear the cause bits*/
#define SHIFTCAUSE        2
#define NOTPRIVINSTRUCT   10
#define PLTINTERRUPT      0x00000200
#define PSEUDOCLOCKINT    0x00000400
#define DISKINTERRUPT     0x00000800
#define FLASHINTERRUPT    0x00001000
#define PRINTINTERRUPT    0x00004000
#define TERMINALINTERRUPT 0x00008000
#define TRANSMITBITS      0x0F

/*Device Constants*/
#define DEVICE0           0x00000001
#define DEVICE1           0x00000002
#define DEVICE2           0x00000004
#define DEVICE3           0x00000008
#define DEVICE4           0x00000010
#define DEVICE5           0x00000020
#define DEVICE6           0x00000040
#define DEVICE7           0x00000080

/*Exception Constants*/
#define MAKEPROCESS       1
#define KILLPROCESS       2
#define PASSERN           3
#define VERHOGEN          4
#define WAITIO            5
#define GETCLOCK          6  
#define CLOCKSEMA4        7
#define SUPPORTDATA       8
#define TERMINATE         9
#define GETTOD            10
#define WRITETOPRINTER    11
#define WRITETOTERMINAL   12
#define READFROMTERMINAL  13

/* Hardware & software constants */
#define PAGESIZE		  4096			/* page size in bytes	*/
#define WORDLEN			  4				  /* word size in bytes	*/
#define pcb_PTR           pcb_t*
#define MAXPROC           20
#define semd_PTR          semd_t*
#define MAXINT            0xFFFFFFFF
#define STKPTR            0x20001000
#define POOLSIZE          32
#define MAXPAGE           32

/* timer, timescale, TOD-LO and other bus regs */
#define RAMBASEADDR		0x10000000
#define RAMBASESIZE		0x10000004
#define TODLOADDR		  0x1000001C
#define INTERVALTMR		0x10000020	
#define TIMESCALEADDR	0x10000024
#define STANQUANTUM     5000
#define STANPSEUDOCLOCK 100000

/* utility constants */
#define	TRUE			    1
#define	FALSE			    0
#define HIDDEN			  static
#define EOS				    '\0'

#define NULL 			    ((void *)0xFFFFFFFF)

/* device interrupts */
#define DISKINT			  3
#define FLASHINT 		  4
#define NETWINT 		  5
#define PRNTINT 		  6
#define TERMINT			  7
#define DEVINTNUM		  5		  /* interrupt lines used by devices */
#define DEVPERINT		  8		  /* devices per interrupt line */
#define DEVREGLEN		  4		  /* device register field length in bytes, and regs per dev */	
#define DEVREGSIZE	      16 	  /* device register size in bytes */
#define DEVCNT   (DEVINTNUM * DEVPERINT) /*total number of devices */

/* device register field number for non-terminal devices */
#define STATUS			  0
#define COMMAND			  1
#define DATA0			  2
#define DATA1			  3

/* device register field number for terminal devices */
#define RECVSTATUS  	0
#define RECVCOMMAND 	1
#define TRANSTATUS  	2
#define TRANCOMMAND 	3

/* device common STATUS codes */
#define UNINSTALLED		0
#define READY			    1
#define BUSY			    3

/* device common COMMAND codes */
#define RESET			    0
#define ACK				    1
#define ON                  1
#define OFF                 0
#define FAILED              -1
#define OK                  0

/* Memory related constants */
#define KSEG0           0x00000000
#define KSEG1           0x20000000
#define KSEG2           0x40000000
#define KUSEG           0x80000000
#define RAMSTART        0x20000000
#define BIOSDATAPAGE    0x0FFFF000
#define	PASSUPVECTOR	  0x0FFFF900

/* Exceptions related constants */
#define	PGFAULTEXCEPT	  0
#define GENERALEXCEPT	  1
#define GOTOSYSCALL       8
#define GOTOINTERRUPTS    0
#define PGFAULTCAUSEMAX   3
#define PGFAULTCAUSEMIN   0

/* operations */
#define	MIN(A,B)		((A) < (B) ? A : B)
#define MAX(A,B)		((A) < (B) ? B : A)
#define	ALIGNED(A)		(((unsigned)A & 0x3) == 0)

/* Macro to load the Interval Timer */
#define LDIT(T)	((* ((cpu_t *) INTERVALTMR)) = (T) * (* ((cpu_t *) TIMESCALEADDR))) 

/* Macro to read the TOD clock */
#define STCK(T) ((T) = ((* ((cpu_t *) TODLOADDR)) / (* ((cpu_t *) TIMESCALEADDR))))

/**
 * This macro assumes that both RAMBASEADDR and RAMBASESIZE are
 * both defined with the appropriate addresses (from the beginning of the Bus Register area.
 * */
#define RAMTOP(T) ((T) = ((* ((int *) RAMBASEADDR)) + (* ((int *) RAMBASESIZE))))

#endif
