/*
 *the innards of a table-lookup package
 *Implement a simple version of the #define processor (i.e., no arguments)
 *suitable for use with C programs, based on the routines of this section. You may also find
 *getch and ungetch helpful.
*/
//#define _PREPROCESS
#ifdef _PREPROCESS

#include <stdio.h>
//#include <ctype.h>
#include "fx_preprocess.h"
static void usage(){
	fprintf (stderr, "Preprocess C/C++ programs to stdout. Version 0.2\n"
		             "Usage: preprocess [files...]\n\n"
					 );
}
int main(int argc,char*argv[])
{
    int i;
	if(argc<2)
		preprocess(stdin,stdout);

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
            preprocess(f,stdout);
            fclose(f);
            putchar('\f');
        }
    }
    return 0;
}
#endif