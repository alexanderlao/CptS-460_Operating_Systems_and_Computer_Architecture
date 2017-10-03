#include "MyPrint.c"

#define SSIZE 1024		// 2KB stack size per PROC

typedef struct proc		// process structure
{
	struct proc *next;	// next PROC pointer
	int *ksp;		// saved sp when PROC is not running
	int kstack[SSIZE];	// process kernel mode stack
} PROC;				// PROC is a type

int procSize = sizeof (PROC);
PROC proc0, *running;

int scheduler ()
{
	running = &proc0;
}

main ()
{
	myprintf ("This is my first OS kernel!\n");
	running = &proc0;
	myprintf ("call tswitch()\n");
	tswitch();
	myprintf("back to main()\n");
}
