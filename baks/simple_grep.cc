
#define _SIMPLE_GREP
#ifdef _SIMPLE_GREP
#include <stdio.h>
#include <string.h>
#define MAXLINE 1000
//int getline_(char line[],int lim){
//    int i,c;
//    i=0;
//    while(i<lim-1&&((c=getchar())!=EOF)&&c!='\n')
//        line[i++]=c;
//    if(c==EOF&&i==0)
//        return -1;
//    if(c=='\n')
//        line[i++]=c;
//    line[i]='\0';
//    return i;
//}
int strindex(char src[],char itm[]){
    int i,j;
    for(i=0;src[i]!='\0';i++){
        int k;
        for(j=0,k=i;itm[j]!='\0'&&itm[j]==src[k];j++,k++);
        if(itm[j]=='\0')
            return i;
    }
    return -1;
}
int strrindex(char s[],char t[]){
    int i,j;
    int pos=-1;
    for(i=0;s[i]!='\0';i++){
        int k;
        for(j=0,k=i;t[j]!='\0'&&t[j]==s[k];j++,k++);
        if(t[j]=='\0')
            pos=i;
    }
    return pos;
}

//maybe quicker than strrindex
int strrindex2(char s[],char t[]){
    int i,j;
    int ls=strlen(s);
    int lt=strlen(t);
    for(i=ls-lt;i>=0;i--){
        int k;
        for(j=0,k=i;t[j]!='\0'&&t[j]==s[k];j++,k++);
        if(t[j]=='\0')
            return i;
    }
    return -1;
}

int main(){
    char line[MAXLINE];
    int found=0;
    char *pat="main";
    while(fgets(line,MAXLINE,stdin)>0)
        if(strindex(line,pat)>=0){
            printf("%s",line);
            found++;
        }
}
#endif