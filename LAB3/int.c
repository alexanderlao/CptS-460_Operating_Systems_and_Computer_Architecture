
/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
   int a,b,c,d, r;

	//==> WRITE CODE TO GET get syscall parameters a,b,c,d from ustack
	a = get_word (running -> uss, ((running -> usp) + 26));
	b = get_word (running -> uss, ((running -> usp) + 28));
	c = get_word (running -> uss, ((running -> usp) + 30));
	d = get_word (running -> uss, ((running -> usp) + 32));

   switch(a){
       case 0 : r = kgetpid();        break;
       case 1 : r = kpd();            break;
       case 2 : r = kchname(b);       break;
       case 3 : r = kkfork();         break;
       case 4 : r = ktswitch();       break;
       case 5 : r = kkwait(b);        break;
       case 6 : r = kkexit(b);        break;

       case 90: r = getc();	      break;
       case 91: r = putc(b);	      break;
       case 99: kkexit(b);            break;
       default: printf("invalid syscall # : %d\n", a); 
   }

	//==> WRITE CODE to let r be the return value to Umode
	put_word (r, running -> uss, ((running -> usp) + 16));	// ax register
	return r;
}

//============= WRITE C CODE FOR syscall functions ======================

int kgetpid()
{
    return running -> pid;
}

int kpd()
{
	//WRITE C code to print PROC information
	int i;
	PROC *p;

	// loop through each proc
	for (i = 0; i  < NPROC; i++)
	{
		p = &proc[i];
		printf ("%s ", p -> name);
		
		switch (p -> status)
		{
			case FREE:
				printf ("FREE ");
				break;
			case READY:
				printf ("READY ");
				break;
			case RUNNING:
				printf ("RUNNING ");
				break;
			case STOPPED:
				printf ("STOPPED ");
				break;
			case SLEEP:
				printf ("SLEEP ");
				break;
			case ZOMBIE:
				printf ("ZOMBIE ");
				break;
		}
		
		printf ("%d %d\n", p -> pid, p -> ppid);
	}

	return 0;
}

int kchname(char *name)
{
	//WRITE C CODE to change running's name string;
	char c;
	int i = 0;
	
	// assuming there are only 32 bytes in a name
	while (i < 32)
	{
		// get and write each byte one by one
		c = get_byte (running -> uss, name + i);
		running -> name[i] = c;
		if (c == '\0') break;
		i++;
	}
}

int kkfork()
{
	//use you kfork() in kernel;
	//return child pid or -1 to Umode!!!
	return kfork ("/bin/u1");
}

int ktswitch()
{
    return tswitch();
}

int kkwait(int *status)
{
	//use YOUR kwait() in LAB3;
	//return values to Umode!!!
	int pid, s;
	pid = kwait (&s);
	put_word (s, running -> uss, status);
	return pid;
}

int kkexit(int value)
{
	//use your kexit() in LAB3
	//do NOT let P1 die
	kexit (value);
}
