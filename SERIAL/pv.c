struct semaphore{
  int value;
  PROC *queue;
};

int P(struct semaphore *s)
{
  // write YOUR C code for P()
  int_off();

  s -> value--;

  if (s -> value < 0)
  {
	running -> status = BLOCK;
	enqueue(&s -> queue, running);
	tswitch();
  }

  int_on();
}

int V(struct semaphore *s)
{
  // write YOUR C code for V()
  PROC *p;
  int_off();

  s -> value++;
  
  if (s -> value <= 0)
  {
	p = dequeue(&s -> queue);
	p -> status = READY;
	enqueue(&readyQueue, p);
  }

  int_on();
}

