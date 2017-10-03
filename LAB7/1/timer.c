/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

extern int column, row, color;

u16 tick, hours, minutes, seconds;

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

  printf("timer init\n");
  tick = hours = minutes = seconds = 0; 
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
  // save a copy of the current row, column and color
  int savedRow = row;
  int savedColumn = column;
  int savedColor = color;

  // set the row and column to the bottom right corner
  row = 24;
  column = 70;
  color = 0x0A; // yellow

  // increment the seconds as long as we're not over 60
  if (!(++tick % 60)) seconds++;

  if (tick % 60 == 0)
  {
	// if we hit 60 seconds
	if (!(seconds % 60))
	{
		// increment the minutes and reset the seconds
		minutes++;
		seconds = 0;
	}
	
	// if we hit 60 minutes
	if (!(minutes % 60) && !(seconds % 60))
	{
		// increment the hours and reset the minutes
		hours++;
		minutes = 0;
	}
  }

  // display the clock
  putc (hours / 10 + '0');
  putc (hours % 10 + '0');
  putc (':');
  putc (minutes / 10 + '0');
  putc (minutes % 10 + '0');
  putc (':');
  putc (seconds / 10 + '0');
  putc (seconds % 10 + '0');

  // restore the original row, column and color
  row = savedRow;
  column = savedColumn;
  color = savedColor;

  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
}

