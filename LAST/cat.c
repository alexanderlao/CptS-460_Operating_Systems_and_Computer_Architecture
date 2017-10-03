#include "ucode.c"

int main (int argc, char *argv[])
{
	char c, line[128], tty[64], nc = '\n';
	int fd, n, i, isRedirected = 0;
	STAT st0, st1, st_tty;

	// check if a file was passed in
	if (argc == 1)
	{
		// reading from stdin
		fd = 0;
	}
	else
	{
		// reading from a file
		fd = open (argv[1], O_RDONLY);
	}

	// check if the file was opened correctly
	if (fd < 0)
	{
		printf ("CAT ERROR: cannot open file '%s'\n", argv[1]);
		return 0;
	}

	// reading a file...
	if (fd)
	{
		// read one char at a time
		n = read (fd, &c, 1);

		while (n)
		{
			putc (c);
			n = read (fd, &c, 1);
		}
	}
	// reading from stdin...
	else
	{
		// check if the output is bring redirected
		isRedirected = checkIfRedirected ();

		if (isRedirected == 1)
		{
			while (1)
			{
				n = read (fd, &c, 1);
				
				// ctrl + d or EOF
				if (c == 4 || !n)
				{
					return 1;
				}

				// writing to console
				write (2, &c, 1);
				
				if (c == '\r')
				{		
					// write the new line character if we encounter a new line	
					write (2, &nc, 1);
				}

				// write to the redirected output
				putc (c);
			}
		}
		else
		{
			while(1)
			{
				n = read (fd, &c, 1);

				if (c == 13)
				{
					c = '\n';
				}

				// ctrl + d or EOF
				if (c == 4 || !n)
				{
					return 1;
				}

				putc (c);
			}
		}
	}

	close(fd);
	return 1;
}

// checks if output is being redirected
int checkIfRedirected ()
{
	int n = 0;
	char buf[128];
	STAT stat;

	fstat (1, &stat);

	// check modes for redirection
	if ((stat.st_mode & 0xF000) == 0x8000 || (stat.st_mode & 0xF000) == 0x4000 || (stat.st_mode & 0xF000) == 0xA000)
	{
		return 1;
	}

	// otherwise no redirection
	return 0;
}
