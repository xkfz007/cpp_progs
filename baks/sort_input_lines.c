/*
 * this program is used to sort the lines that you input
 */
#ifdef _SORT_INPUT_LINES
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define MAXLINES 512
#define BUFFERSIZE 200
int comp(const void *,const void *);
int main()
{
    size_t nls,maxlines=MAXLINES;
    char **lines=(char**)calloc(maxlines,sizeof(char*));
    char s[BUFFERSIZE];
    int i;

    for(nls=0;fgets(s,BUFFERSIZE,stdin)!=NULL;++nls)
    {
        if(nls==maxlines)
        {
            char **tlines;
            maxlines+=MAXLINES;
            tlines=realloc(lines,maxlines*sizeof(char*));
            assert(tlines);
            lines=tlines;
        }
        lines[nls]=(char*)calloc(strlen(s)+1,sizeof(char));
        strcpy(lines[nls],s);
    }
    printf("ok\n");
    qsort(lines,nls,sizeof lines[0],comp);
    printf("the reuslt is:\n");
    for(i=0;i<nls;++i)
    {
        fputs(lines[i],stdout);
        fflush(stdout);
        //printf("%d:%s\n",i,lines[i]);
        free(lines[i]);
    }
    free(lines);
    return 0;
}
int comp(const void *p1,const void *p2)
{
    const char *ps1=*(const char**)p1;
    const char *ps2=*(const char**)p2;
   return  strcmp(ps1,ps2);
}
#endif
