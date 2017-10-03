#include "MyPrint.c"

#define NPROC 9			// number of PROCs
#define SSIZE 1024		// 2KB stack size per PROC

/// PROC status
#define FREE 	0
#define READY	1
#define STOP	2
#define DEAD	3

typedef struct proc		// process structure
{
	struct proc *next;	// next PROC pointer
	int *ksp;		// saved sp when PROC is not running
	int pid;		// add pid as proc's ID
	int ppid;		// parent pid
	int status;		// status = FREE|READY|STOPPED|DEAD
	int priority;		// scheduling priority
	int kstack[SSIZE];	// process kernel mode stack
} PROC;				// PROC is a type

PROC proc[NPROC], *running, *freeList, *readyQueue;	// define NPROC proc structures
int procSize = sizeof (PROC);

#include "queue.c"
#include "kernel.c"

int body ()
{
	char c;
	myprintf ("proc %d starts from body ()\n", running -> pid);
	
	while (1)
	{
		printList ("freeList  ", freeList);
		printList ("readyQueue", readyQueue);	// show the readyQueue
		printf ("proc %d running: parent=%d\n", running -> pid, running -> ppid);
		printf ("enter a char [s|f]: ");
		c = getc(); printf("%c\n", c);

		switch (c)
		{
			case 'f' : do_kfork();	 break;
			case 's' : do_tswitch(); break;		
		}
	}
}

PROC *kfork ()		// creat a child process, begin from body ()
{
	int i;
	PROC *p = get_proc(&freeList);

	if (!p)
	{
		printf ("no more PROC, kfork () failed\n");
		return 0;
	}

	p -> status = READY;
	p -> priority = 1;		// priority = 1 for all proc except P0
	p -> ppid = running -> pid;	// parent = running

	// initialize new proc's kstack[]
	for (i = 1; i < 10; i++)
	{
		p -> kstack[SSIZE - i] = 0;	// all 0s
	}
	
	p -> kstack[SSIZE -1] = (int)body;	// resume point = address of body ()
	p -> ksp = &p -> kstack[SSIZE - 9];	// proc saved sp
	enqueue (&readyQueue, p);		// enter p into readyQueue by priority
	return p;				// return child PROC pointer
}

int init ()
{
	PROC *p;
	int i;
	
	printf ("init ...\n");
	
	for (i = 0; i < NPROC; i++)	// initialize all PROCs
	{
		p = &proc[i];
		p -> pid = i;		// pid = 0, 1, 2, ..., NPROC - 1
		p -> status = FREE;
		p -> priority = 0;
		p -> next = &proc[i+1];	// point to the next PROC
	}

	proc[NPROC - 1].next = 0;
	freeList = &proc[0];			// all PROCs are in freeList
	readyQueue = 0;

	// create P0 as running
	p = get_proc(&freeList);		// allocate a PROC from freeList
	p -> ppid = 0;				// P0's parent is itself
	p -> status = READY;
	running = p;				// P0 is now running
}

int scheduler ()
{
	if (running -> status == READY)		// if running is still READY
	{
		enqueue (&readyQueue, running);	// enter it into the readyQueue
	}

	running = dequeue (&readyQueue);	// new running
}

main ()
{
	printf ("MTX starts in main ()\n");
	init ();				// initialize and create P0 as running
	kfork ();				// P0 creates child P1
	while (1)				// P0 switches if readyQueue not empty
	{
		if (readyQueue)
		{
			tswitch ();
		}
	}
}
