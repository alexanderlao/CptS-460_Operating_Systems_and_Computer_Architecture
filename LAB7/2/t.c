#include "type.h"

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;

int body();
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
               "Saturn", "Uranus", "Neptune" };

/**************************************************
  bio.o, queue.o loader.o are in mtxlib
**************************************************/
//#include "wait.c"
//#include "kernel.c"

// #include "fe.c"
#include "int.c"
#include "timer.c"

int color;

int init()
{
    PROC *p;
    int i;

    printf("init ....");

    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
        p->inkmode = 1;   
        p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

int scheduler()
{
    if (running->status == RUNNING){
       running->status = READY;
       enqueue(&readyQueue, running);
    }
    running = dequeue(&readyQueue);
    running->status = RUNNING;
    running -> time = 5;
    color = 0x09 + running -> pid;
}

int int80h(), tinth();

int set_vec(vector, addr) u16 vector, addr;
{
    u16 location,cs;
    location = vector << 2;
    put_word(addr, 0, location);
    put_word(0x1000,0,location+2);
}

int goUmode();

PROC *ufork(char *filename)
{
	int i;
	u16 segment;

	PROC *p = get_proc(&freeList);	

	if (p == 0)
	{
	 	printf("no more PROC, kfork() failed\n");
	 	return 0;
	}

	p->status = READY;
	p->priority = 1;          // priority = 1 for all proc except P0
	p->ppid = running->pid;   // parent = running
	p->parent = running;
	p->inkmode = 0;

	for (i=1; i<10; i++)
	  p->kstack[SSIZE-i]= 0 ;

	p->kstack[SSIZE-1] = (int)goUmode;	// call tswitch from uMode when the proc runs
	p->ksp = &(p->kstack[SSIZE-9]);		// point ksp to the top of the stack

	// setting up umode
	segment = ((p -> pid) + 1) * 0x1000;

	if (filename)
	{
		load (filename, segment);	// load filename to child segment as its umode image
		
		// set up the child's stack to return to umode
		for (i = 0; i < 13; i++)
		{
			put_word (0, segment, segment - (i * 2));
		}

		put_word (0x0200, segment, segment - 2);	// uflag
	}
	
	put_word (segment, segment, segment - 4);	// uCS
	put_word (segment, segment, segment - 22);	// uES
	put_word (segment, segment, segment - 24);	// uDS
	
	p -> usp = segment - 24;
	p -> uss = segment;

	enqueue(&readyQueue, p);  // enter p into readyQueue by priority
	printf ("Proc %d forked a child %d at segment=%x\n", p -> ppid, p -> pid, p -> uss);
	nproc++;
	return p;                 // return child PROC pointer
}
            
main()
{
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    set_vec(80,int80h);

    ufork("/bin/u1");     // P0 kfork() P1
    ufork("/bin/u1");     // P0 kfork() P2
    ufork("/bin/u1");     // P0 kfork() P3
    ufork("/bin/u1");     // P0 kfork() P4
    lock();
    set_vec(8,tinth);
    timer_init();

    while(1){
      printf("P0 running\n");
      if (nproc==2 && proc[1].status != READY)
	  printf("no runable process, system halts\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();   // P0 switch to run P1
   }
}
