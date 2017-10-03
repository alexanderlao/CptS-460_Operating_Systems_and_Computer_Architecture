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

	p->kstack[SSIZE-1] = (int)body;	// call tswitch from uMode when the proc runs
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

int copyImage (u16 pseg, u16 cseg, u16 size)
{
	u16 i;
	for (i = 0; i < size; i++)
	{
		put_word(get_word(pseg, 2*i), cseg, 2*i);
	}
}

int fork ()
{
	int pid;
	u16 segment;
	
	PROC *p = kfork(0);	// kfork() a child, do not load image file
	if (p == 0) return -1;	// kfork failed

	segment = ((p -> pid) + 1) * 0x1000;		// child segment
	copyImage (running -> uss, segment, 32*1024);	// copy the whole 32k words
	
	p -> uss = segment;		// child's own segment
	p -> usp = running -> usp;	// same as parent's usp

	// change uDS, uES, uCS, AX to child's in child's ustack
	put_word (segment, segment, p -> usp);		// uDS = segment
	put_word (segment, segment, ((p -> usp) + 2));	// uES = segment
	put_word (0, segment, ((p -> usp) + (2 * 8)));	// uax = 0
	put_word (segment, segment, ((p -> usp) + (2 * 10)));	// uCS = segment
	
	return p -> pid;
}

int kexec (char *y)	// y points to the filename in Umode space
{
	int i, length = 0;
	char filename[64], *cp = filename;
	u16 segment = running -> uss;		// same segment
	
	// get the filename from user space with a length limit of 64
	while ((*cp++ = get_byte (running -> uss, y++)) && length++ < 64);

	printf ("about to load file = %s\n", filename);
	
	if (!load(filename, segment)) return -1;	// load filename to segment
	
	// reinitialize process ustack for it to return -1 to user mode
	for (i = 1; i < 12; i++)
		put_word (0, segment, -2*i);
	running -> usp = -24;

	put_word (segment, segment, -2*12);	// saved uDS = segment
	put_word (segment, segment, -2*11);	// saved uES = segment
	put_word (segment, segment, -2*2);	// uCS = segment; uPC = 0
	put_word (0x0200, segment, -2*1);	// Umode flag = 0x0200
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
