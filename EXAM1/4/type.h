typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define NPROC    9
#define SSIZE 1024

/******* PROC status ********/
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

#define NFD	    10
#define NOFT	    10

#define NPIPE       10
#define PSIZE	    10
#define READ_PIPE   4
#define WRITE_PIPE  5
#define READ        0
#define WRITE       1

typedef struct pipe
{
    char buf[PSIZE];		// circular data buffer
    int head, tail;		// circular buf index
    int data, room;		// number of data and room in the pipe
    int nreader, nwriter;       // number of readers, writers on pipe
    int busy;			// pipe is being used
}PIPE;

typedef struct oft
{
    int mode;			// READ_PIPE | WRITE_PIPE
    int refCount;		// number of PROCs using this pipe end
    struct pipe *pipe_ptr;	// pointer to the PIPE object
}OFT;

typedef struct proc{
    struct proc *next;
    int    *ksp;               // at offset 2

    int    uss, usp;           // at offsets 4,6
    int    inkmode;            // at offset 8

    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    char   name[32];           // name string of PROC

    OFT    *fd[NFD];	       // opened file descriptors; copy on fork()

    int    kstack[SSIZE];      // per proc stack area
}PROC;










