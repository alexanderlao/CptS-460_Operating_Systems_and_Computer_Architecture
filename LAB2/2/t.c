#include "MyPrint.c"

#define NPROC 9			// number of PROCs
#define SSIZE 1024		// 2KB stack size per PROC

typedef struct proc		// process structure
{
	struct proc *next;	// next PROC pointer
	int *ksp;		// saved sp when PROC is not running
	int pid;		// add pid as proc's ID
	int kstack[SSIZE];	// process kernel mode stack
} PROC;				// PROC is a type

PROC proc[NPROC], *running;	// define NPROC proc structures

int procSize = sizeof (PROC);

int body ()
{
	char c;
	int pid = running -> pid;
	myprintf ("proc %d resumes to body ()\n", pid);
	
	while (1)
	{
		printf ("proc %d running, enter a key: \n", pid);
		c = getc ();
		tswitch ();
	}
}

int init ()
{
	PROC *p;
	int i, j;
	
	for (i = 0; i < NPROC; i++)	// initialize all PROCs
	{
		p = &proc[i];
		p -> pid = i;		// pid = 0, 1, 2, ..., NPROC - 1
		p -> next = &proc[i+1];	// point to the next PROC
		
		if (i)			// for all PROCs except P0
		{
			p -> kstack[SSIZE - 1] = (int)body;	// entry address of body ()
			for (j = 2; j < 10; j++)		// all saved registers = 0
			{
				p -> kstack[SSIZE - j] = 0;
			}
			
			p -> ksp = &(p -> kstack[SSIZE - 9]);	// save sp in PROC.ksp
		}
	}

	proc[NPROC - 1].next = &proc[0];	// all PROCs form a circular list
	running = &proc[0];			// P0 is running;
	myprintf ("init complete\n");
}

int scheduler ()
{
	running = running -> next;
}

main ()
{
	init ();
	while (1)
	{
		myprintf ("P0 running\n");
		tswitch ();
	}
}
