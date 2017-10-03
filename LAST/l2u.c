#include "ucode.c"

int main (int argc, char *argv[])
{
	int fd1, fd2, n, usingStdOut = 0;
	char c;

	// use stdin and stdout if no paramters
	if (argc == 1)
	{
		fd1 = 0;
		fd2 = 1;
		usingStdOut = 1;
	}
	// use file and stdout if one parameter
	else if (argc == 2)
	{
		fd1 = open (argv[1], O_RDONLY);
		fd2 = 1;
		usingStdOut = 1;
	}
	// otherwise use the two files
	else
	{
		fd1 = open (argv[1], O_RDONLY);
		fd2 = open (argv[2], O_WRONLY|O_CREAT);
	}

	// check if the files were opened correctly
	if (fd1 < 0)
	{
		printf ("L2U ERROR: cannot open file 1 '%s'\n", argv[1]);
		return 0;
	}
	if (fd2 < 0)
	{
		printf ("L2U ERROR: cannot open file 2 '%s'\n", argv[2]);
		return 0;
	}

	// read the first character
	n = read (fd1, &c, 1);

	while (n)
	{
		// convert by subtracting ascii values
		if (c > 96 && c < 123)
		{
			c -= 32;
		}

		if (usingStdOut)
		{
			// output if we're using stdout
			putc (c);
		}
		else
		{
			// otherwise write to the file
			write (fd2, &c, 1);
		}

		// read the next char
		n = read (fd1, &c, 1);
	}

	close(fd1);
	close(fd2);
	return 1;
}
