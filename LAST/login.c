//***********************************************************************
//                   LOGIC of login.c file
//***********************************************************************

/* PROBLEMS WITH THIS FILE: undefined symbol: _exec
			    undefined symbol: _settty
			    undefined symbol: _chuid
*/

char *tty;
int stdin, stdout;
int uid, gid;
char userDir[64], userFullName[64], userProgram[64];

main (int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
  char username[64], password[64];
  tty =  argv[1];

  close(0); close(1); //close(2); // login process may run on different terms

  // open its own tty as stdin, stdout, stderr
  stdin  = open(tty, 00);
  stdout = open(tty, 01);

  settty(tty);   // store tty string in PROC.tty[] for putc()

  // NOW we can use printf, which calls putc() to our tty
  printf("LOGIN : open %s as stdin, stdout, stderr\n", tty);

  signal(2,1);  // ignore Control-C interrupts so that 
                // Control-C KILLs other procs on this tty but not the main sh

  while(1)
  {
    /* 1. show login:        to stdout
    2. read user nmae        from stdin
    3. show passwd:
    4. read user passwd

    5. verify user name and passwd from /etc/passwd file

    6. if (user account valid)
       {
          setuid to user uid.
          chdir to user HOME directory.
          exec to the program in users's account
       }
       printf("login failed, try again\n"); */

    // prompt the user for their login information
    printf ("Username: ");
    gets (username);
    printf ("Password: ");
    gets(password);

    // check the user's login information
    if (checkLogin (username, password) == 1)
    {
	// successful login
	printf ("successful login for %s!\n", username);
	
	// set the user's properties
	chuid (uid, gid);
	chdir (userDir);
	exec (userProgram);
	
	return 1;
    }

    printf ("incorrect username or password. try again.\n");
  }

  exit (0);
}

int checkLogin (char username[], char password[])
{
	int passwordFile = open ("etc/passwd", 00);
	int usernameOK = 0, passwordOK = 0;
	char buf[128];
	char* token = 0;

	// check if the password file opened correctly
	if (passwordFile == 0)
	{
		return 0;
	}

	// read the password file into buf
	// and tokenize it
	read (passwordFile, buf, 128);
	token = strtok (buf,":\n");

	while (token != 0)
	{
		// check if the username matches
		if (strcmp (token, username) == 0)
		{
			usernameOK = 1;
		}

		// once we have a matching username, check the password
		if (usernameOK == 1)
		{
			token = strtok (0, ":\n");
			
			// check if the password matches
			if (strcmp (token, password) == 0)
			{
				// retrieve the user's information
				uid = atoi (strtok (0, ":\n"));
				gid = atoi (strtok (0, ":\n"));
				strcpy (userFullName, strtok (0, ":\n"));
				strcat (userDir, strtok (0, ":\n"));
				strcpy (userProgram, strtok (0, ":\n"));

				close (passwordFile);
				return 1;
			}
		}

		// get the next token
		token = strtok (0, ":\n");
	}

	// no match was found
	close (passwordFile);
	return 0;
}
