#include "ucode.c"

int main (int argc, char *argv[])
{
	char tty[64], c;
	int fd, status;

	// check if a file was passed in
	if (argc == 1)
	{
		// use stdin
		fd = dup (0);
		close (0);
		gettty (tty);
		open (tty, O_RDONLY);
	}
	else
	{
		// otherwise open a file
		fd = open (argv[1], O_RDONLY);
	}

	// check if the file was valid
	if (fd < 0)
	{
		printf ("MORE ERROR: cannot open file '%s'\n", argv[1]);
		return 0;
	}

	// print the first page
	status = printPage (fd);

	// check if there was only one page worth
	if (status == -1)
	{
		return 1;
	}

	while (1)
	{
		// get user input
		c = getc ();

		switch (c)
		{
			// return, space, or q
			case '\n':
				status = printLine (fd);
				break;
			case ' ':
				status = printPage (fd);
				break;
			case 'q':
				putc('\n');
				return 1;
				break;
		}

		if (status == -1)
		{
			return 1;
		}
	}

	return 1;
}

int printLine (int fd)
{
	int i, n;
	char c;

	// loop through the width of the screen
	// 80 pixels
	for (i = 0; i < 80; i++)
	{
		// read one char at a time
		n = read (fd, &c, 1);

		// check if we're at the end
		if (!n)
		{
			return -1;
		}

		putc (c);

		// exit the loop on a new line or return
		if (c == '\n' || c == '\r') break;
	}

	putc ('\n');

	return 1;
}

int printPage (int fd)
{
	int i, j, n;
	char c;
	
	// loop through the height of the screen
	// 20 pixels
	for (i = 0; i < 20; i++)
	{
		// loop through the width of the screen
		// 80 pixels
		for (j = 0; j < 80; j++)
		{
			// read one char at a time
			n = read (fd, &c, 1);

			// check if we're at the end
			if (!n)
			{
				return -1;
			}

			putc (c);

			// exit the loop on a new line or return
			if (c == '\n' || c == '\r') break;
		}
	}

	putc('\n');

	return 1;
}
