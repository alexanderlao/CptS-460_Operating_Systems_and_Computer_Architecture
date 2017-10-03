#include "ucode.c"

int main (int argc, char *argv[])
{
	int fd, lineNumber = 1, atEOF;
	char pattern[64], line[128];

	// check for valid parameters
	if (argc < 2)
	{
		printf ("GREP ERROR: invalid parameters\n");
		return 0;
	}

	atEOF = 0;
	strcpy (pattern, argv[1]);

	// check if a file was passed in
	if (argc == 2)
	{
		// grep from stdin
		fd = 0;
	}
	else
	{
		// open the file to grep
		fd = open (argv[2], O_RDONLY);

		if (!fd)
		{
			printf ("GREP ERROR: cannot open file '%s'\n", argv[1]);
			return 0;
		}
	}

	while (!atEOF)
	{
		// reset the line buffer and read a line into the buffer
		resetLine (line);
		getLine (fd, line, &atEOF);

		// check if the read in line contains the pattern
		if (containsPattern (line, pattern))
		{
			// print the line if it does
			printLine (lineNumber, line);
		}

		lineNumber++;
	}

	close(fd);
	return 1;
}

// clears the line
int resetLine (char *line)
{
	int i;

	for (i = 0; i < 128; i++)
	{
		line[i] = 0;
	}

	return 1;
}

// retrieves a line from the file
int getLine (int fd, char *line, int *atEOF)
{
	int i = 0, n;

	while (i < 128)
	{
		// read one char at a time
		n = read(fd, &line[i], 1);

		// check if we didn't read anything
		if (!n)
		{
			*atEOF = 1;
			return 1;
		}

		// check if we found the end of the line
		if (line[i] == '\n' || line[i] == '\r')
		{
			return 1;
		}

		i++;
	}
}

// checks if the line contains the pattern
int containsPattern (char *line, char *pattern)
{
	return strstr (line, pattern);
}

// prints the line based on the line number
int printLine (int lineNumber, char *line)
{
	printf("%d: %s\n", lineNumber, line);
	return 1;
}
