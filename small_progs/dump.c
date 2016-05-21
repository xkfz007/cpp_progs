/*
 * this is a great program. it is used to print the binary file content on the
 * screen in hex form.
 *    Description:  使用十六进制的形式输出文本文件
 */
//#define _DUMP
#ifdef _DUMP

#include <stdio.h>
#include <ctype.h>

#define NBYTES 16
void dump(FILE*,char*);
int main(int argc,char*argv[])
{
    int i;
    if(argc<1)
    {
        printf("usage:dump <file>\n");
        exit(1);
    }
    for(i=1;i<argc;++i)
    {
        FILE *f;
        if((f=fopen(argv[i],"rb"))==0)
            fprintf(stderr,"Can't open %s\n",argv[i]);
        else
        {
            dump(f,argv[i]);
            fclose(f);
            putchar('\f');
        }
    }
    return 0;
}
void dump(FILE *f,char*s)
{
    unsigned char buf[NBYTES];
    int count;
    long size=0L;
    int i;
    printf("Dump of %s:\n\n",s);
    while((count=fread(buf,1,NBYTES,f))>0)
    {
        printf(" %06X ",size+=count);
        for(i=0;i<NBYTES;++i)
        {
            if(i==NBYTES/2)
                putchar(' ');
            if(i<count)
            {
                printf(" %02X",buf[i]);
                if(!isprint(buf[i]))
                        buf[i]='.';
            }
            else
            {
            fputs("   ",stdout);
            buf[i]=' ';
            }
        }
        printf(" |%16.16s|\n",buf);

    }
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