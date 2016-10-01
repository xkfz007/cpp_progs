/*
 * dump.c HEXDUMP utility
 * this is a great program. it is used to print the binary file content on the
 * screen in hex form.
 */
//#define _DUMP
#ifdef _DUMP

#include <stdio.h>
//#include <ctype.h>
#include "fx_dump.h"
static void usage(){
	fprintf (stderr, "Prints a hexdump of stdin or specified files to stdout. Version 0.2\n"
                     "Similar in behavior to 'od -Ax -tx2z [a file]'\n\n"
		             "Usage: dump [files...]\n\n"
					 );
}
#define NBYTES 16
int main(int argc,char*argv[])
{
    int i;
  //  if(argc<2)
  //  {
		//usage();
  //      exit(1);
  //  }
	if(argc<2)
		dump(stdin,stdout,NBYTES);

	if(argc==2&&(!strcmp(argv[1],"-h")
		||!strcmp(argv[1],"--help"))){
			usage();
			exit(1);
	}

    for(i=1;i<argc;++i)
    {
        FILE *f;
        if((f=fopen(argv[i],"rb"))==0)
            fprintf(stderr,"Can't open %s\n",argv[i]);
        else
        {
			fprintf(stdout,"Dump of %s:\n\n",argv[i]);
            dump(f,stdout,NBYTES);
            fclose(f);
            putchar('\f');
        }
    }
    return 0;
}
#endif
