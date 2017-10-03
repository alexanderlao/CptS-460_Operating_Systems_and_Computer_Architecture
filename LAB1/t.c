#include "ext2.h"
#define BLK 1024

typedef struct ext2_group_desc 	GD;
typedef struct ext2_inode	INODE;
typedef struct ext2_dir_entry_2	DIR;

u16 NSEC = 2;
char buf1[BLK], buf2[BLK];	// 2 I/O buffers of 1KB each

int prints(char *s)
{
	while (*s)
	{
		putc (*s++);
	}
}

int gets(char s[ ])
{
    	while ((*s = getc ()) != '\r')
	{
		putc (*s++);
	}
	
	*s = 0;
}

int getblk(u16 blk, char *buf)
{
	readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *name)
{
	int i;
	char c;
	DIR *dp;

	// assume a DIR has at most 12 direct blocks
	for (i = 0; i < 12; i++)
	{
		if ((u16)ip -> i_block[i])
		{
			getblk ((u16)ip -> i_block[i], buf2);
			dp = (DIR*)buf2;
			
			while ((char*)dp < &buf2[BLK])
			{	
				// save the last byte
				c = dp -> name[dp -> name_len];

				// make the name into a string
				dp -> name[dp -> name_len] = 0;
				
				// show the dp -> name string
				prints (dp -> name);
				putc (' ');

				if (strcmp (dp -> name, name) == 0)
				{
					prints ("\n\r");
					return ((u16)dp -> inode);
				}

				// restore the last byte
				dp -> name[dp -> name_len] = c;
				dp = (char*)dp + dp -> rec_len;
			} 
		}
	}

	// to error () if we can't find the file name
	error ();
}	

main()
{
   /*char name[64];
   while(1){
     prints("What's your name? ");
     gets(name);
     if (name[0]==0)
        break;
     prints("Welcome "); prints(name); prints("\n\r");
   }
   prints("return to assembly and hang\n\r");*/

	char *cp, *name[2], filename[64];
	u16 i, ino, blk, iblk;
	u32 *up;
	GD *gp;
	INODE *ip;
	DIR *dp;
	name[0] = "boot";
	name[1] = filename;

	prints ("bootname: ");
	gets (filename);
	
	if (filename[0] == 0) name[1] = "mtx";
	
	// read blk#2 to get the GD 0
	getblk (2, buf1);
	gp = (GD*)buf1;

	iblk = (u16)gp -> bg_inode_table;	// inodes begin block
	getblk (iblk, buf1);			// read in the first inode block
	ip = (INODE*)buf1 + 1;			// ip -> root inode #2

	// search for the system name
	for (i = 0; i < 2; i++)
	{
		ino = search (ip, name[i]) - 1;
		if (ino < 0) error ();

		getblk (iblk + (ino / 8), buf1);	// read inode block of ino
		ip = (INODE*)buf1 + (ino % 8);
	}

	// read indirect block into buf2, if any
	if ((u16)ip -> i_block[12])
	{
		getblk ((u16)ip -> i_block[12], buf2);
	}

	setes (0x1000);		// set ES to the loading segment
	
	// load direct blocks
	for (i = 0; i < 12; i++)
	{
		getblk ((u16)ip -> i_block[i], 0);
		inces ();
		putc ('*');	// show a * for each direct block loaded
	}

	// load indirect blocks, if any
	if ((u16)ip -> i_block[12])
	{
		up = (u32*)buf2;
		
		while (*up)
		{
			getblk ((u16)*up, 0);
			inces ();
			putc ('.');	// show a . for each indirect block loaded
			up++;
		}
	}

	prints ("ready to go?");
	getc ();
}
