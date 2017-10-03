extern PROC *readyQueue, *freeList;

/****************** utility.c file ***********************/

// remove and return first PROC in queue 
PROC *dequeue(PROC **queue)
{
	// remove from the front of the queue
	PROC *p = *queue;

	// check if p is valid
	if (p == 0)
	{
		// return nothing
		return 0;
	}

	// fix the queue
	*queue = p -> next;

	return p;
}

PROC *get_proc(PROC **list)  // allocate a FREE proc from freeList
{
	PROC* p;
	
	// just return the value dequeued from the list
	if (list)
	{
		p = dequeue (list);
		return p;
	}

	// return 0 if something went wrong
	return 0;
}

int put_proc(PROC **list, PROC *p)   // return a PROC to freeList
{
	// check if the list is empty
	if (!(*list))
	{
		// put p as the first proc in the list
		*list = p;
		p -> next = 0;
	}
	// otherwise there's at least one proc in the list
	else
	{
		// insert it at the front
		p -> next = *list;
		*list = p;
	}
}

// enter a PROC into queue by priority
int enqueue(PROC **queue, PROC *p)
{
	// set cur to the front of the queue
	PROC *cur = *queue;
	PROC *front = *queue;

	// the queue has at least one item in it
	if (cur != 0)
	{
		// iterate through the queue while there's another link
		// and the priority is greater
		while ((cur -> next) && ((cur -> priority) >= (p -> priority)))
		{	
			// check ahead so we don't go too far
			if ((cur -> next -> priority) < (p -> priority))
			{
				// link p after cur
				p -> next = cur -> next;
				cur -> next = p;
				return 1;
			}
		
			// otherwise keep iterating
			cur = cur -> next;
		}

		// at this point, if the priority of cur is less than
		// cur's priority, we need to add at the end
		if (p -> priority == 0)
		{
			// add p at the end
			cur -> next = p;
			p -> next = 0;
		}
		// otherwise check where we are in the queue
		// in the middle
		else if (cur -> next)
		{
			// link p after cur and before cur's next
			p -> next = cur -> next;
			cur -> next = p;
		}
		// still at the front
		else if (cur == front)
		{
			// link p at the front
			p -> next = cur;
			*queue = p;
		}
		// end of the queue
		else
		{
			// link p at the end
			cur -> next = p;
			p -> next = 0;
		}
	}
	else
	{
		// the queue is empty
		*queue = p;
		p -> next = 0;
	}
}

// print queue and print list functions 
int printList(char *name, PROC *p)
{
	myprintf ("%s: ", name);

	// printing the freeList
	if (p)
	{
		while (p)
		{
			myprintf ("%d -> ", p -> pid);
			p = p -> next;
		}
		printf ("NULL\n");
	}
	else
	{
		printf ("%s is empty\n", name);
	}
}

int printQueue(char *name, PROC *p)
{
	myprintf ("%s: ", name);

	// printing the queue
	if (p)
	{
		while (p)
		{
			myprintf ("%d [%d] -> ", p -> pid, p -> priority);
			p = p -> next;
		}
		printf ("NULL\n");
	}
	else
	{
		printf ("%s is empty\n", name);
	}
}

int printSleepList(char *name, PROC *p)
{
	myprintf ("%s: ", name);

	// printing the freeList
	if (p)
	{
		while (p)
		{
			myprintf ("%d [%d] -> ", p -> pid, p -> priority);
			p = p -> next;
		}
		printf ("NULL\n");
	}
	else
	{
		printf ("%s is empty\n", name);
	}
}
