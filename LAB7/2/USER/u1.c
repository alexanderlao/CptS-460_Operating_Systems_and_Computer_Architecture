#include "ucode.c"
int color;

main(int argc, char *argv[])
{ 
  int pid, segment, i, j;
 
  pid = getpid();
  color = 0x000B + (pid % 5);
  segment = (pid+1)*0x1000;   
  printf("==============================================\n");
  printf("I am proc %din U mode: segment=%x\n", pid, segment);
  show_menu();

  while(1){
       // large delay loop
       for (i = 0; i < 63 * 1024; i++)
       {
		for (j = 0; j < 63 * 1024; j++)
		{

		}
       }
  }
}



