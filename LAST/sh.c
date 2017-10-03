/* ==========================================================
                   sh Program:
YOUR sh must support
(1). logout/exit :  terminate
(2). simple command exceution, e.g. ls, cat, ....
(3). I/O redirection:  e.g. cat < infile;  cat > outfile, etc.
(4). (MULTIPLE) PIPEs: e.g. cat file | l2u | grep LINE 
=========================================================== */

#include "ucode.c"

#define NCMD 7

char *commands[NCMD] = {"cat", "more", "grep", "cp", "l2u", "ls", "logout"};
char userInput[64], command[64][64], leftCmd[64];
int numOfTokens, running = 1;

int main (int argc, char *argv[])
{
	printf("welcome to my sh\n");

	// loop continuously
	while (running)
	{
		// reset the buffers for the new command
		resetBuffers ();
		printf ("\n");

		// prompt the user for a command
		printf ("mysh #: ");
		gets (userInput);

		// parse the user's input
		parseInput ();

		// do the command
		doCommand ();

		printf("\n");
	}

	return 1;
}

int resetBuffers ()
{
	int i, j;

	// reset the token counter
	numOfTokens = 0;

	// loop through the command array
	for (i = 0; i < 64; i++)
	{
		for (j = 0; j < 64; j++)
		{
			// reset each slot
			command[i][j] = 0;
		}
	}

	// loop through the user input array
	for (i = 0; i < 64; i++)
	{
		// reset it
		userInput[i] = 0;
	}

	return 1;
}

int parseInput ()
{
	int i = 0;
	char copy[64];
	char *token;

	// use a copy of the userInput
	strcpy (copy, userInput);

	// tokenize the copy because strtok
	// will mess up the str
	token = strtok (copy, " ");

	while (token)
	{
		// store the command in the command array
		strcpy (command[i], token);
		token = strtok (0, " ");
		i++;
	}

	// save a copy of how many tokens we parsed
	numOfTokens = i;

	return 1;
}

int getCmdIndex (char *cmd)
{
	int i;

	// loop through the array of hard-coded commands
	for (i = 0; i < NCMD; i++)
	{
		// check for a matching command
		if (strcmp (cmd, commands[i]) == 0)
		{
			// return that index
			return i;	
		}
	}

	// otherwise the command wasn't found
	return -1;
}

int doCommand ()
{
	char *cmd;
	int cmdIndex, pid, i, status = 0;
	int hasPipe = 0, hasRedirection = 0;

	// the command should be the first index
	strcpy (cmd, command[0]);

	// get the command's index
	cmdIndex = getCmdIndex (cmd);

	// check for a valid command
	if (cmdIndex == -1)
	{
		printf("DO_COMMAND ERROR: command %s not found\n", cmd);
		return 0;
	}

	// check for a logout first
	if (cmdIndex == 6)
	{
		printf ("logging out! see ya!\n");
		exit (0);
	}

	// fork a child to do the command
	pid = fork();

	// check for a valid fork
	if (pid < 0)
	{
		printf("DO_COMMAND ERROR: fork failed!\n");
		return 0;
	}

	// parent executes this part
	if (pid)
	{
		printf("PARENT WAITS FOR CHILD %d TO DIE\n", pid);
		pid = wait(&status);
		printf("DEAD CHILD = %d, HOW = %04x\n", pid, status);
	}
	// child executes this part
	else
	{
		// check if the command contains a pipe
		hasPipe = checkForPipe (userInput);
		
		if (hasPipe == 1)
		{
			// found a pipe so handle it
			handlePipe (userInput);
		}
		else
		{
			// check if the command contains redirection
			hasRedirection = checkForRedirection (userInput);

			if (hasRedirection != -1)
			{
				// handle the redirection
				handleRedirection (userInput, hasRedirection);
				exec (leftCmd);
			}
			else 
			{
				// no redirection or pipe
				exec (userInput);
			}
		}
	}

	return 1;
}

int checkForPipe (char* cmd)
{
	int i;

	// loop through each char in the command
	for (i = 0; i < strlen (cmd); i++)
	{
		// check for the pipe character
		if (cmd[i] == '|')
		{
			return 1;
		}
	}

	// no pipe was found
	return 0;
}

int handlePipe (char* cmd)
{
	char head[64], tail[64];
	int containsPipe = 0, multiplePipes = 0, hasRedirection = 0;	
	int i = 0, pd[2], pid;

	// create the pipe and fork a child
	pipe (pd);
	pid = fork ();

	// parent as pipe reader
	if (pid)
	{
		// get the tail of the command
		getTail (cmd, tail);

		close (pd[1]);		// READER MUST close pd[1]
		dup2 (pd[0], 0);	// replace 0 with pd[0]
		close (pd[0]);
		
		// check for a multiple pipes
		multiplePipes = checkForPipe (tail);
	
		if (multiplePipes == 1)
		{
			// handle the other pipe
			handlePipe (tail);
		}
		else
		{
			// check for redirection in the tail
			hasRedirection = checkForRedirection (tail);
			
			if (hasRedirection != -1)
			{
				handleRedirection (tail, hasRedirection);
			}
			
			// no redirection so just exec the command
			exec (tail);
		}
	}
	// child as pipe writer
	else
	{
		// get the head of the command
		getHead (cmd, head);

		close (pd[0]);		// WRITER MUST close pd[0]
		dup2 (pd[1], 1);	// replace 1 with pd[1]
		close (pd[1]);
	
		// check for redirection in the head
		hasRedirection = checkForRedirection (head);
		if (hasRedirection != -1)
		{
			// handle the redirection
			handleRedirection (head, hasRedirection);
			exec (leftCmd);
		}
		else
		{
			// otherwise no redirection
			exec (head);
		}
	}

	return 1;
}

int getHead (char *cmd, char *head)
{
	int i;
	
	// loop through each character in the command
	for (i = 0; i < strlen (cmd); i++)
	{
		// find the first pipe symbol
		if (cmd[i] == '|')
		{
			// copy the content preceding the "| "
			strcpy (head, cmd);
			head[i - 1] = 0;

			return 1;
		}
	}

	return 0;
}

int getTail (char *cmd, char *tail)
{
	int i;

	// loop through each char in the cmd
	for (i = 0; i < strlen(cmd); i++)
	{
		// find the first pipe symbol
		if (cmd[i] == '|')
		{
			// copy the content following the "| "
			strcpy (tail, cmd + i + 2);
			return 1;
		}
	}

	return 0;
}

int checkForRedirection (char *cmd)
{
	int i;

	// loop through each char in the command
	for (i = 0; i < strlen (cmd); i++)
	{
		// check which kind of redirection
		if (cmd[i] == '<')
		{
			// 1 for input
			return 1;
		}
		if (cmd[i] == '>')
		{
			// check next character for append
			if (cmd[i + 1] == '>')
			{
				// 3 for append
				return 3;
			}
			
			// 2 for output
			return 2;
		}
	}

	// otherwise no redirection
	return -1;
}

int handleRedirection (char *cmd, int type)
{
	char rest[64], copy[64];
	int i, j = 0, length;

	// save a copy of the cmd
	strcpy (copy, cmd);

	// loop through each character in the command
	for (i = 0; i < strlen (cmd); i++)
	{
		// parse out the commands
		if (copy[i] == '<' || copy[i] == '>')
		{
			// eliminate the space
			cmd[i - 1] = 0;

			// retrieve the left side of the redirection
			length = i - 1;
			strncpy (leftCmd, cmd, length);
			
			// check for append
			if (copy[i] == '>' && copy[i + 1] == '>')
			{
				i++;
			}
			
			i += 2;

			// copy the rest of the line
			strcpy(rest, copy + i);
		}
	}

	// input redirection <
	if (type == 1)
	{
		close(0);

		// try to open the file for reading
		if (open (rest, O_RDONLY) != 1)
		{
			printf ("HANDLE_REDIRECTION ERROR: cannot open input file\n");
		}
		else
		{
			printf ("file = %s opened successfully\n", rest);
		}
	}
	// output redirection >
	else if (type == 2)
	{
		close(1);
		
		// try to open the file for writing
		if (open (rest, O_WRONLY | O_CREAT) != 1)
		{
			write (2, "HANDLE_REDIRECTION ERROR: cannot open output file\n", 
			       strlen("HANDLE_REDIRECTION ERROR: cannot open output file\n"));
		}
		else
		{
			write (2, "output file opened successfully\n", strlen("output file opened successfully\n"));
		}
	}
	// append redirection >>
	else if (type == 3)
	{
		close(1);

		// try to open the file for appending
		if (open (rest, O_APPEND | O_WRONLY | O_CREAT) != 1)
		{
			write (2, "HANDLE_REDIRECTION ERROR: cannot open append file\n", 
			       strlen("HANDLE_REDIRECTION ERROR: cannot open append file\n"));
		}
		else
		{
			write (2, "append file opened successfully\n", strlen("append file opened successfully\n"));
		}
	}
	else
	{
		printf ("HANDLE_REDIRECTION ERROR: incorrect redirection type\n");
		return 0;
	}

	return 1;
}
