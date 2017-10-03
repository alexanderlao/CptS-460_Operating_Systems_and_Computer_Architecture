/****************** kernel.c file *************************/
int body();

PROC *kfork(char *filename)
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

	for (i=1; i<10; i++)
	  p->kstack[SSIZE-i]= 0 ;

	p->kstack[SSIZE-1] = (int)body;		// call tswitch from body when the proc runs
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
		put_word (segment, segment, segment - 4);	// uCS
		put_word (segment, segment, segment - 22);	// uES
		put_word (segment, segment, segment - 24);	// uDS
		p -> uss = segment;
		p -> usp = segment - 24;
	}

	enqueue(&readyQueue, p);  // enter p into readyQueue by priority
	printf ("Proc %d forked a child %d at segment=%x\n", p -> ppid, p -> pid, p -> uss);
	nproc++;
	return p -> pid;                 // return child PROC pointer
}

int get_word (u16 segment, u16 offset)
{
	u16 word;
	setds (segment);
	word = *((u16*)offset);
	setds (0x1000);
	return word;
}

int put_word (u16 word, u16 segment, u16 offset)
{
	setds (segment);
	*((u16*)offset) = word;
	setds (0x1000);
}

int do_tswitch()
{
  printf("proc %d tswitch()\n", running->pid);
  tswitch();
  printf("proc %d resumes\n", running->pid);
}

int do_kfork()
{
  PROC *p;
  printf("proc %d kfork a child\n", running->pid);
  p = kfork ("/bin/u1");
  if (p==0)
    printf("kfork failed\n");
  else
    printf("child pid = %d\n", p->pid);
}

int do_exit()
{
  int exitValue;
  if (running->pid == 1 && nproc > 2){
      printf("other procs still exist, P1 can't die yet !%c\n",007);
      return -1;
  }
  printf("enter an exitValue (0-9) : ");
  exitValue = (getc()&0x7F) - '0'; 
  printf("%d\n", exitValue);
  kexit(exitValue);
}

int do_wait()
{
  int child, status;
  child = kwait(&status);
  if (child<0){
    printf("proc %d wait error : no child\n", running->pid);
    return -1;
  }
  printf("proc %d found a ZOMBIE child %d exitValue=%d\n", 
	   running->pid, child, status);
  return child;
}

int do_umode()
{
  goUmode ();
}

int body()
{
  char c;
  printf("proc %d resumes to body()\n", running->pid);
  while(1){
    printf("-----------------------------------------\n");
    printList("freelist  ", freeList);
    printQueue("readyQueue", readyQueue);
    printSleepList("sleepList ", sleepList);
    printf("-----------------------------------------\n");

    printf("proc %d[%d] running: parent=%d\n",
	   running->pid, running->priority, running->ppid);

    printf("enter a char [s|f|q|w|u] : ");
    c = getc(); printf("%c\n", c);
    switch(c){
       case 's' : do_tswitch();   break;
       case 'f' : do_kfork();     break;
       case 'q' : do_exit();      break;  
       case 'w' : do_wait();      break;
       case 'u' : do_umode();	  break;
    }
  }
}
