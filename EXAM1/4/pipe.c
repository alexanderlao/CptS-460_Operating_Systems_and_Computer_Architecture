OFT oft[NOFT];
PIPE pipe[NPIPE];

PIPE* allocatePipe ()
{
	int i;

	// loop through the array of pipes
	for (i = 0; i < NPIPE; i++)
	{
		// find an empty pipe
		if (!pipe[i].busy)
		{
			// initialize and return the pipe
			pipe[i].busy = 1;
			pipe[i].head = 0;
			pipe[i].tail = 0;
			pipe[i].data = 0;
			pipe[i].room = PSIZE;
			pipe[i].nwriter = 1;
			pipe[i].nreader = 1;

			return &pipe[i];
		}
	}

	return 0;
}

OFT* allocateOft (int newMode)
{
	int i;
	
	// loop through the oft array
	for (i = 0; i < NOFT; i++)
	{
		// find a usable oft
		if (oft[i].refCount == 0)
		{
			// initialize and return the oft
			oft[i].mode = newMode;
			oft[i].refCount = 1;
			oft[i].pipe_ptr = 0;
		
			return &oft[i];
		}
	}

	return 0;
}

// displays the opened pipe descriptors of the running process
int pfd ()
{
	int i;

	printf ("PROC %d opened pipe descriptors\n", running -> pid);
	printf ("FD          MODE          refCount\n");

	// loop through the proc's oft
	for (i = 0; i < NFD; i++)
	{	
		// find valid oft
		if (running -> fd[i])
		{
			printf ("%d     -     %d     -     %d\n",
				i, running -> fd[i] -> mode, running -> fd[i] -> refCount);
		}
	}

	return 0;
}

int showPipe (PIPE *pp)
{
	int i, j = pp -> tail;

	printf ("head = %d", pp -> head);
	printf ("tail = %d", pp -> tail);
	printf ("data = %d", pp -> data);
	printf ("room = %d", pp -> room);
	printf ("#readers = %d", pp -> nreader);
	printf ("#writers = %d\n", pp -> nwriter);

	printf ("Pipe content = ");
	
	for (i = pp -> data; i > 0; i--)
	{
		printf ("%c", pp -> buf[j++]);
	}

	printf ("\n");

	return 0;
}

int kpipe (int pd[2])
{
	OFT *readPtr = 0, *writePtr = 0;
	int i = 0, j = 0;

	// allocate a new pipe object
	PIPE *newPipe = allocatePipe ();

	// allocate two OFTs
	readPtr = allocateOft (READ_PIPE);
	writePtr = allocateOft (WRITE_PIPE);

	// have the OFTs point to the same pipe object
	readPtr -> pipe_ptr = newPipe;
	writePtr -> pipe_ptr = newPipe;

	printf ("readPtr mode = %d, refCOunt = %d\n", readPtr -> mode, readPtr -> refCount);
	printf ("writePtr mode = %d, refCOunt = %d\n", writePtr -> mode, writePtr -> refCount);

	// find two free entries in the processes's fd array
	while (i < NFD && running -> fd[i] != 0) i++;
	while (j < NFD && (j == i || running -> fd[j] != 0)) j++;

	printf ("i = %d, j = %d\n", i, j);

	// let fd[i] point to the readOft and fd[j] point to the writeOft
	running -> fd[i] = readPtr;
	running -> fd[j] = writePtr;

	// write index i to pd[0] and j to pd[1] in user space
	put_word (i, running -> uss, pd++);
	put_word (j, running -> uss, pd);

	return 0;
}

int closePipe (int fd)
{
	OFT *op;
	PIPE *pp;
	
	// get the process's OFT at the given fd
	op = running -> fd[fd];
	running -> fd[fd] = 0;			// clear its entry

	// check for a reader pipe
	if (op -> mode == READ_PIPE)
	{
		pp = op -> pipe_ptr;
		pp -> nreader--;

		// check for the last reader
		if (--op -> refCount <= 0)
		{
			// check for any writers
			if (pp -> nwriter <= 0)
			{
				// clear the pipe if no more writers
				pp -> busy = 0;
				return;
			}
		}

		// wakeup all BLOCKED writers
		kwakeup (&pp -> room);
		return;
	}
	// check for a writer pipe
	else if (op -> mode == WRITE_PIPE)
	{
		pp = op -> pipe_ptr;
		pp -> nwriter--;
		
		// check for the last writer
		if (--op -> refCount <= 0)
		{
			// check for any readers
			if (pp -> nreader <= 0)
			{
				// clear the pipe if no more readers
				pp -> busy = 0;
				return;
			}
		}

		// wakeup all BLOCKED readers
		kwakeup (&pp -> room);
		return;
	}
}

int readPipe (int fd, char *buf, int n)
{
	int r = 0;
	PIPE *pp = running -> fd[fd] -> pipe_ptr;

	if (n <= 0) return 0;

	// validate fd
	if (running -> fd[fd] == 0) return -1;

	// validate reader pipe
	if (running -> fd[fd] -> mode != READ_PIPE) return -1;

	printf ("pipe before read\n");
	showPipe (pp);

	// while there's still bytes to read
	while (n)
	{	
		// while there's data to read
		while (pp -> data)
		{
			// put the byte in user space
			put_byte (pp -> buf[pp -> tail++], running -> uss, buf++);

			if (pp -> tail == PSIZE) pp -> tail = 0;

			pp -> room++;
			pp -> data--;
			r++;
			n--;

			if (n == 0) break;
		}

		printf ("pipe after read\n");
		showPipe (pp);		

		if (r)
		{
			kwakeup (&pp -> room);
			return r;
		}
		
		// check if there's still a writer
		if (pp -> nwriter > 0)
		{
			printf ("READER WAKING UP WRITERS FOR ROOM!\n");
			printf ("READER GOING TO SLEEP FOR DATA!\n");
			kwakeup (&pp -> room);
			ksleep (&pp -> data);
			continue;
		}
		else
		{
			// no writer so close the pipe
			closePipe (fd);
			break;
		}
	}

	return r;
}

int writePipe (int fd, char *buf, int n)
{
	int r = 0, i = 0;
	PIPE *pp = running -> fd[fd] -> pipe_ptr;
	char c;

	if (n <= 0) return 0;

	// validate fd
	if (running -> fd[fd] == 0) return -1;

	// validate writer pipe
	if (running -> fd[fd] -> mode != WRITE_PIPE) return -1;

	// check for a valid pipe
	if (pp -> nreader <= 0)
	{
		printf ("Broken pipe error!\n");
		return -1;
	}

	printf ("pipe before write\n");
	showPipe (pp);

	// while there's still data to write
	while (n)
	{
		while (pp -> room)
		{
			// get and write each byte one by one
			c = get_byte (running -> uss, buf + i);

			// write a byte from buf to the pipe
			pp -> buf[pp -> head++] = c;
			
			// if the head ptr is at the end, reset it
			if (pp -> head == PSIZE) pp -> head = 0;
			
			pp -> room--;
			pp -> data++;
			n--;
			r++;
			i++;

			// break if there's no more bytes to write
			if (n == 0) break;
		}

		printf ("pipe after write\n");
		showPipe (pp);

		printf ("WRITER IS WAKING UP READERS!\n");

		// wakeup ALL readers, if any
		kwakeup (&pp -> data);

		if (n == 0) return r;	// finished writing n bytes

		printf ("WRITER GOING TO SLEEP FOR ROOM!\n");

		// still has data to write but pipe has no room
		ksleep (&pp -> room);
	}
}






















