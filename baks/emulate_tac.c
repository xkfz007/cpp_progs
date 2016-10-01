/*
this program emulates the tool "tac" simplely,which output the content of file reversely
*/
#include <stdio.h>
#include <string.h>

#if 0
int main(int argc,char*argv[]){
    char *p1,*p2,*p3,*p4;
    char buf[BUFSIZ];
    FILE *fp;
    int sz;
    if((fp=fopen(argv[1],"r"))==NULL)
    {
        fprintf(stderr,"Can't open file:%s\n",argv[1]);
        return -1;
    }
    fseek(fp, 0,SEEK_END);
    sz=ftell(fp);
    printf("sz=%d BUFSIZ=%d\n",sz,BUFSIZ);
    rewind(fp);
    fread(buf,sizeof(char),sz,fp);
   // fprintf(stdout,"%s",buf);
    p1=strchr(buf,'\n');
    p2=strrchr(buf,'\n');
    *p2='\0';
    do{
        p2=strrchr(buf,'\n');
        p3=p2+1;
        printf("%s\n",p3);
        *p2='\0';
    }while(p2!=p1);
    if(p2==p1){
        *p2='\0';
        printf("%s\n",buf);
    }
    return 0;
}
#endif