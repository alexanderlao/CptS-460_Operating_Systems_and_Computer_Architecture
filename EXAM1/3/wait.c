/************* wait.c file ****************/

int ksleep(int event)
{
	running -> event = event;		// record the event in PROC.event
	running -> status = SLEEP;		// change status to sleep
	tswitch ();				// give up CPU
}

int kwakeup(int event)
{
	int i;
	PROC *p;
	
	// loop through every PROC but P0
	for (i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		
		if ((p -> status == SLEEP) && (p -> event == event))
		{
			p -> event = 0;			// cancel PROC's event
			p -> status = READY;		// make it ready to run again
			enqueue (&readyQueue, p);	// put it in the readyQueue
		}
	}
}

int kexit(int exitValue)
{
	int i, wakeUpP1 = 0;
	PROC *p;
	
	// nproc = number of active procs
	if ((running -> pid == 1) && (nproc > 2))
	{
		printf ("other procs still exist, P1 can't die yet!\n");
		return -1;
	}

	// send children (dead or alive) to P1
	for (i = 1; i < NPROC; i++)
	{
		p = &proc[i];
		
		// look for active children of the parent
		if (p -> status != FREE && p -> ppid == running -> pid)
		{
			p -> ppid = 1;
			p -> parent = &proc[1];
			wakeUpP1++;
		}
	}

	// record the exit value and become a zombie
	running -> exitCode = exitValue;
	running -> status = ZOMBIE;

	// wakeup parent and P1 if necessary
	kwakeup (running -> parent);		// parent sleeps on its PROC address
	
	if (wakeUpP1)
	{
		kwakeup (&proc[1]);
	}
	
	tswitch ();				// give up CPU
}

int kwait(int *status)
{
	PROC *p;
	int i, hasChild = 0;
	
	// search PROCs for a child
	while (1)
	{
		// loop through each PROC except P0
		for (i = 0; i < NPROC; i++)
		{
			p = &proc[i];

			// check for non-free processes and children
			if (p -> status != FREE && (p -> ppid == running -> pid))
			{
				hasChild = 1;			// has child flag

				// check for zombie child
				if (p -> status == ZOMBIE)
				{
					*status = p -> exitCode;	// get its exit code
					p -> status = FREE;		// FREE the child
					put_proc (&freeList, p);	// put it in the freeList
					nproc--;			// one less process
					return (p -> pid);		// return its pid
				}
			}
		}
	
		if (!hasChild) return -1;	// if there's no child, return error
		ksleep (running);		// if there's still children alive: sleep on PROC address
	}
}

