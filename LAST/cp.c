#include "ucode.c"

int main (int argc, char *argv[])
{
	int fd1, fd2, n, usingStdOut = 0;
	char c;

	// check for valid parameters
	if (argc < 3)
	{
		printf ("CP ERROR: invalid parameters\n");
		return 0;
	}

	fd1 = open (argv[1], O_RDONLY);
	fd2 = open (argv[2], O_WRONLY|O_CREAT);

	// check if the files were opened correctly
	if (fd1 < 0)
	{
		printf ("CP ERROR: cannot open file 1'%s'\n", argv[1]);
		return 0;
	}
	if (fd2 < 0)
	{
		printf ("CP ERROR: cannot open file 2'%s'\n", argv[2]);
		return 0;
	}

	// read the first char
	n = read (fd1, &c, 1);

	while (n)
	{
		// write the char to the other file
		write (fd2, &c, 1);
	
		// read the next char
		n = read (fd1, &c, 1);
	}

	close (fd1);
	close (fd2);
	return 1;
}
