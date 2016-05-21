/*
 * This is a simple file viewer
 */
//#define _SIMPLE_FILE_VIEWER
#ifdef _SIMPLE_FILE_VIEWER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define NMAX 100
#define NROWS 24 
#define NCOLS 79 
#define HORIZ 20 
#define RANDOM() rand()%NMAX
// defination of stack    
typedef  fpos_t  TYPE;
TYPE stack[NMAX];
int _top;
void init();
int empty();
void push(TYPE p);
TYPE pop();
TYPE top();
int full();

char screen[NROWS][BUFSIZ];

size_t nlines,offset=0;
void read_a_screen(FILE*f)
{
    int i;
    clearerr(f);
    for(i=0;i<NROWS&&fgets(screen[i],BUFSIZ,f);i++)
        screen[i][strlen(screen[i])-1]='\0';
    nlines=i;
}
void display()
{
    int i;
    for(i=0;i<nlines;i++)
        if(offset<strlen(screen[i]))
            fprintf(stdout,"%-.*s\n",NCOLS,screen[i]+offset);
        else
            fputc('\n',stdout);

}
int main(int argc,char**argv)
{
   FILE*f;
   fpos_t top_pos;
   if(argc<2)
   {
       printf("Too few arguments\n");
       return -1;
   }
   if(NULL==(f=fopen(argv[1],"r")))
   {
       printf("Can't open the file:%s\n",argv[1]);
       return -1;
   }
    init();
TOP:
    rewind(f);
    fgetpos(f,&top_pos);
    read_a_screen(f);
    display();
    for(;;)
    {
        int c=getchar();
        switch(toupper(c))
        {
            case 'N':
                if(!feof(f))
                {
                    push(top_pos);
                    fgetpos(f,&top_pos);
                    read_a_screen(f);
                }
                display();
                break;
            case 'P':
                if(!empty())
                {
                    top_pos=pop();
                    fsetpos(f,&top_pos);
                    read_a_screen(f);

                }
                display();
                break;
            case 'T':
                while(!empty())
                    pop();
                goto TOP;
                break;
            case 'B':
                while(!feof(f))
                {
                    push(top_pos);
                    fgetpos(f,&top_pos);
                    read_a_screen(f);
                }
                display();
                break;
            case 'L':
                if(offset>0)
                    offset-=HORIZ;
                display();
                break;
            case 'R':
                if(offset<BUFSIZ-HORIZ)
                    offset+=HORIZ;
                display();
                break;
            case 'Q':
                return 0;
        }
    }

    return 0;
}

void init()
{
    _top=-1;
}
int empty()
{
    return _top==-1;
}
void push(TYPE p)
{
    stack[++_top]=p;
}
TYPE pop()
{
    return stack[_top--];
}
TYPE top()
{
    return stack[_top];
}
int full()
{
    return _top==NMAX-1;
}
#endif
