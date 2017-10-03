//*************************************************************************
//                      Logic of init.c 
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1.. 
//************************************************************************

int pid, status, tty0, ttyS0, ttyS1;
int stdin, stdout, stderr;

#include "ucode.c"  //<========== AS POSTED on class website

main(int argc, char *argv[])
{
	// 1. open /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages
	stdin = open ("dev/tty0", O_RDONLY);
	stdout = open ("dev/tty0", O_WRONLY);
	stderr = open ("dev/tty0", O_RDWR);

	// 2. Now we can use printf, which calls putc(), which writes to stdout
	printf ("INIT : fork a login task on console\n");

	tty0 = fork ();
	printf ("tty0: %d\n", tty0);

	if (tty0)
	{
		ttyS0 = fork ();

		if (ttyS0)
		{
			ttyS1 = fork ();

			if (ttyS1)
			{
				parent ();
				return 1;
			}

			login (2);
			return 1;
		}

		login (1);
		return 1;
	}

	login (0);
	return 1;
}       

int login (int p)
{
	switch (p)
	{
		case 0:
			exec ("login /dev/tty0");
			break;
		case 1:
			exec ("login /dev/ttyS0");
			break;
		case 2:
			exec ("login /dev/ttyS1");
			break;
		default:
			printf ("INIT: login error\n");
			break;
	}

	exec("login /dev/tty0");
	return 1;
}
      
int parent()
{
	while(1)
	{
		printf ("INIT : waiting .....\n");
		pid = wait (&status);

		if (pid == tty0)
		{
			tty0 = fork ();

			if (tty0 == 0)
			{
				login (0);
			}
		}
		else if (pid == ttyS0) 
		{
			ttyS0 = fork ();

			if (ttyS0 == 0)
			{
				login (1);
			}
		}
		else if (pid == ttyS1)
		{
			ttyS1 = fork ();

			if (ttyS1 == 0)
			{
				login (2);
			}
		}
		else 
		{
			printf ("INIT: buried an orphan child %d\n", pid);
		}
	}

	return 1;
}
