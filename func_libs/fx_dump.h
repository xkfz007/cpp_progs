#ifndef _FX_DUMP_H
#define _FX_DUMP_H
#include "fx_types.h"
#include <stdio.h>
void dump(FILE *inf,FILE *outf,int NBYTES)
{
    byte buf[BUFSIZ];
    int count;
    long address=0L;
	//int NBYTES=16;
    int i;
    while((count=fread(buf,sizeof(byte),NBYTES,inf))>0)
    {
        fprintf(outf," %06X ",address);
        for(i=0;i<NBYTES;++i)
        {
            if(i==NBYTES/2)
				fprintf(outf," ");
            if(i<count)
            {
                fprintf(outf," %02X",buf[i]);
                if(!isprint(buf[i]))
                        buf[i]='.';
            }
            else
			{
				fprintf(outf,"   ");
				buf[i]=' ';
            }
        }
        fprintf(outf," |%*.*s|\n",NBYTES,NBYTES,buf);
		address+=count;
		fflush(outf);
    }
}
#ifdef __cplusplus
extern "C" {
#endif
	
/*
one C++ version
*/
#if 0
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


#ifdef __cplusplus
}
#endif

#endif