// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", "fork", "exec", 0};

#define LEN 64

int show_menu()
{
   printf("***************** Menu *******************************\n");
   printf("*  ps  chname  kfork  switch  wait  exit  fork  exec *\n");
   printf("******************************************************\n");
}

int find_cmd(char *name)
{
  // return command index
  int i;

  for (i = 0; i < 9; i++)
  {
	if (strcmp (cmd[i], name) == 0)
	{
		return i;
	}
  }
}

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   return syscall(1, 0, 0);
}

int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
}    

int kswitch()
{
    return syscall(4,0,0);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
} 

int geti()
{
   // return an input integer
   char s[16];
   int val;

   gets(s);		// read in the user input
   val = atoi(s);	// convert the string to an int

   return val;
}

int exit()
{
   int exitValue;
   printf("enter an exitValue: ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  return syscall(6,exitValue,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}

int getc()
{
    return syscall(90, 0, 0) & 0x7F;
}

int putc(char c)
{
    return syscall(91,c,0,0);
}

int fork ()
{
    return syscall (7,0,0,0);
}

int exec (char *s)
{
    return syscall (8,s,0,0);
}

int ufork()
{
    int child = fork();
    (child)? printf ("parent ") : printf("child ");
    printf ("%d return from fork, child_pid = %d\n", getpid(), child);
}

int uexec()
{
    int r;
    char filename[64];
    printf ("enter exec command : ");
    gets(filename);
    r = exec (filename);
    printf ("exec failed\n");
}
