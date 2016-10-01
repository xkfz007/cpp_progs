
#define _SIMPLE_TAIL
#ifdef _SIMPLE_TAIL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEFLINES 10
#define MAXLINES 10
#define MAXLEN 100
void error(char *s){
    printf("%s\n",s);
    exit(1);
}
int main(int argc,char* argv[]){
    char *p,*buf,*bufend;
    char line[MAXLEN];
    char *lineptr[MAXLINES];
    int first,i,last,len,n,nlines;
    if(argc==1)
        n=DEFLINES;
    else if(argc==2&&(*++argv)[0]=='-')
        n=atoi(argv[0]+1);
    if(n<1||n>MAXLINES)
        n=MAXLINES;
    for(i=0;i<MAXLINES;i++)
        lineptr[i]=NULL;
    if((p=buf=(char*)malloc(MAXLINES*MAXLEN))==NULL)
        error("tail:cannot allocate buf");
    bufend=buf+MAXLINES*MAXLEN;
    last=0;
    nlines=0;
    while(fgets(line,MAXLEN,stdin)!=NULL){
        len=strlen(line);
        if(p+len+1>=bufend)
            p=buf;
        lineptr[last]=p;
        strcpy(lineptr[last],line);
        if(++last>=MAXLINES)
            last=0;
        p+=len+1;
        nlines++;
    }
    if(n>nlines)
        n=nlines;
    first=last-n;
    if(first<0)
        first+=MAXLINES;
    for(i=first;n-->0;i=(i+1)%MAXLINES)
        printf("%s",lineptr[i]);
    free(buf);

    return 0;
}

#endif