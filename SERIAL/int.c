/************** syscall routing table ***********/
int kcinth() 
{
  u16 x, y, z, w, r; 
  u16 seg, off;

  seg = running->uss; off = running->usp;

  x = get_word(seg, off+13*2);
  y = get_word(seg, off+14*2);
  z = get_word(seg, off+15*2);
  w = get_word(seg, off+16*2);
  
   switch(x){
       case 0 : r = running->pid;    break;
       case 1 : r = kpd();           break;
       //case 2 : r = chname(y);       break;
       //case 3 : r = kmode();         break;
       case 4 : r = tswitch();       break;
       case 5 : r = kwait();         break;
       case 6 : r = kexit();         break;
       case 7 : r = fork();          break;
       case 8 : r = kexec(y);        break;


       // FOCUS on ksin() nd ksout()
       case 9 : r = ksout(y);        break;
       case 10: r = ksin(y);         break;

       case 99: r = kexit();         break;

       default: printf("invalid syscall # : %d\n", x);

   }
   put_word(r, seg, off+2*8);
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
