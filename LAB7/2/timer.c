/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

extern int column, row, color;

u16 tick;
PROC* p;

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

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
  tick++;
  tick %= 60;

  // every second
  if (tick == 0)
  {
	if (running -> time == 0 && running -> inkmode == 0)
	{
		out_byte(0x20, 0x20);
		tswitch();
	}
	else if (running -> time >= 1 && running -> inkmode == 0)
	{
		printf ("proc %d time = %d\n", running -> pid, running -> time);

		// decrease its time by 1 second
		running -> time--;
	}
  }

  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
}

