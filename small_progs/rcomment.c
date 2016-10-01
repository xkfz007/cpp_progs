/*
 * remove comments in C/C++ programs
 */
//#define _REMOVE_COMMENTS
#ifdef _REMOVE_COMMENTS

#include <stdio.h>
#include "fx_rcomment.h"
static void usage(){
	fprintf (stderr, "remove comments in C/C++ programs of stdin or specified files to stdout. Version 0.2\n"
		             "Usage: rcomment [files...]\n\n"
					 );
}
int main(int argc,char*argv[])
{
    int i;
	if(argc<2)
		remove_comments(stdin,stdout);

	if(argc==2&&(!strcmp(argv[1],"-h")
		||!strcmp(argv[1],"--help"))){
			usage();
			exit(1);
	}

    for(i=1;i<argc;++i)
    {
        FILE *f;
        if((f=fopen(argv[i],"r"))==0)
            fprintf(stderr,"Can't open %s\n",argv[i]);
        else
        {
			fprintf(stdout,"Dump of %s:\n\n",argv[i]);
            remove_comments(f,stdout);
            fclose(f);
            putchar('\f');
        }
    }
    return 0;
}
#endif