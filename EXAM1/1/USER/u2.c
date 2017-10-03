#include "ucode.c"
#include "crt0.c"
int color;

main(/*int argc, char *argv[]*/)
{ 
  char name[64]; int pid, cmd;
  int i;

  while(1){
    pid = getpid();
    color = 0x0C;

    // print out the command line arguments
    printf ("inside main(): argc = %d\n", argc);
    
    for (i = 0; i < argc; i++)
    {
	printf ("argv[%d] = %s\n", i, argv[i]);
    }
       
    printf("----------------------------------------------\n");
    printf("Je suis processus %d En mode U: Segment de course=%x\n",getpid(), getcs());
    show_menu();
    printf("Commander ? ");
    gets(name); 
    if (name[0]==0) 
        continue;

    cmd = find_cmd(name);
    switch(cmd){
           case 0 : getpid();   break;
           case 1 : ps();       break;
           case 2 : chname();   break;
           case 3 : kfork();    break;
           case 4 : kswitch();  break;
           case 5 : wait();     break;
           case 6 : exit();     break;
	   case 7 : ufork();     break;
           case 8 : uexec();     break;

           default: invalid(name); break;
    }
  }
}
