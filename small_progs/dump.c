/*
 * dump.c HEXDUMP utility
 * this is a great program. it is used to print the binary file content on the
 * screen in hex form.
 */
#define _DUMP
#ifdef _DUMP

#include <stdio.h>
//#include <ctype.h>
#include "fx_dump.h"
void usage(){
	fprintf (stderr, "Prints a hexdump of specified files to stdout. Version 0.2\n"
                     "Similar in behavior to 'od -Ax -tx2z [a file]'\n\n"
		             "Usage: dump [files...]\n\n"
					 );
}
#define NBYTES 16
int main(int argc,char*argv[])
{
    int i;
    if(argc<2)
    {
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
            dump(f,NBYTES);
            fclose(f);
            putchar('\f');
        }
    }
    return 0;
}
#endif

/*
one C++ version
*/
#ifdef _DISPLAY
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
const int NUM=16;

int main(int argc,char*argv[])
{
    if(argc<2)
    {
        cout<<"Usage:Display <filename> <filename> ..."<<endl;
        return 1;
    }
    int i=1;
    do{
        ifstream in(argv[i]);
        if(!in)
        {
            cout<<"Cannot open file:"<<argv[i]<<endl;
            continue;
        }
        char line[NUM];
        int count=0;
        int j,k;
        cout.setf(ios::uppercase);
        cout<<"File:"<<argv[i]<<endl;
        while(!in.eof())
        {
            for(j=0;j<NUM&&!in.eof();++j)
                in.get(line[j]);
            if(j<NUM)
                j--;
            for(k=0;k<j;k++)
                cout<<setw(3)<<hex<<(int)line[k];
            for(;k<16;k++)
                cout<<setw(3)<<" ";
            cout<<"\t";
            for(k=0;k<j;k++)
                if(isprint(line[k]))
                    cout<<line[k];
                else
                    cout<<".";
            cout<<endl;
            count++;
            if(count==16){
                count=0;
                cout<<"Press ENTER to continue:";
                cin.get();
                cout<<endl;
            }
            
        }
        in.close();
        i++;
    }while(i<argc);

    return 0;
}
#endif