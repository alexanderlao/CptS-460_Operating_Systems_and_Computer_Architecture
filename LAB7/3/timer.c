/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

typedef struct tq{
    struct tq *next;		// next element pointer
    int       time;		// requested time
    PROC      *proc;		// pointer to PROC
    int	      (*action) ();	// 0|1|handler function pointer
}TQE;

u16 tick;
TQE *tq, tqe[NPROC];		// tq = timer queue pointer

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));
}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */
  int i;

  printf("timer init\n");
  tick = 0;

  out_byte(TIMER_MODE, SQUARE_WAVE);	// set timer to run continuously
  out_byte(TIMER0, TIMER_COUNT);	// timer count low byte
  out_byte(TIMER0, TIMER_COUNT >> 8);	// timer count high byte 
  enable_irq(TIMER_IRQ); 
}

/*===================================================================*
 *		    timer interrupt handler       		     *
 *===================================================================*/
int thandler()
{
  TQE *current;

  tick++;
  tick %= 60;

  // every second
  if (tick == 0)
  {
	// check if there's something in the timer queue
	if (tq)
	{
		current = tq;
		
		// loop through the timerQueue
		while (current)
		{
			// decrease the entries' time
			current -> time--;
			
			// print out the timerQueue
			printTimerQueue();

			// if we decreased and hit zero seconds
			if (current -> time <= 0)
			{
				// wakeup the sleeping proc
				printf ("waking up sleeper proc %d\n", current -> proc -> pid);
				kwakeup (current -> proc -> pid);
				
				// iterate to the next entry
				current = current -> next;

				// clear out this spot in the queue
				tqe[current -> proc -> pid].proc = 0;
				tqe[current -> proc -> pid].next = 0;
			}
			else break;
		}
	}
  }

  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
}

printTimerQueue ()
{
	TQE *current;
	
	// set current to the beginning of the timerQueue
	current = tq;

	printf ("timerQueue: ");
	
	// loop through the queue
	while (current)
	{
		printf ("[proc %d, time = %d] -> ", current -> proc -> pid, current -> time);
		current = current -> next;
	}

	printf ("\n");
}

int itimer (int newTime)
{
	// fill in TQE[pid] information
	TQE newEntry, *previous, *current;

	newEntry.time = newTime;
	newEntry.proc = running;
	newEntry.action = 0;		// 0 for wakeup

	tqe[running -> pid].proc = running;

	lock();

	printf ("adding running -> pid = %d\n", running -> pid);
	
	// traverse the timer queue to compute the
	// position to insert the newEntry
	// check if the queue is empty
	if (tq == 0)
	{
		// just add in the newEntry
		tq = &tqe[running -> pid];
		tqe[running -> pid] = newEntry;
	}
	// the queue is not empty
	else
	{
		// keep track of the position in the queue
		previous = tq;
		current = tq;

		// loop through the queue
		while (current != 0)
		{
			// the newEntry's time is less than the current's time
			if ((newTime - current -> time) < 0) break;

			// otherwise it's greater so we need to decrease
			// the newEntry's time as we move along
			newTime -= current -> time;

			// update our iterators
			previous = current;
			current = current -> next;
		}

		// check if we're at a valid entry
		if (current != 0)
		{
			// update current's time
			current -> time -= newTime;
		}

		// update the newEntry's time
		newEntry.time = newTime;

		// check if we're at the beginning
		if (current == tq)
		{
			// set the newEntry at the beginning
			newEntry.next = tq;
			tq = &tqe[running -> pid];
		}
		// otherwise we're somewhere in the middle
		else
		{
			// add in the newEntry
			tqe[newEntry.proc -> pid].next = current;
			previous -> next = &tqe[newEntry.proc -> pid];
		}
	}

	unlock();

	printTimerQueue ();
	printf ("proc %d going to sleep\n", running -> pid);
	ksleep (running -> pid);
}