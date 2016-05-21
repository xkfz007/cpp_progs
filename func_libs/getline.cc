/*
 implementation of getline
*/
//#define _GETLINE
#ifdef _GETLINE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define getline getline2

//first version: stops at blank line
int getline1(char s[],int lim,FILE*fp){
    int ch,i=0;
    while((ch=fgetc(fp))!=EOF&&ch!='\n'&&i<lim){
        s[i++]=ch;
    }
    s[i]='\0';
    return i;//the length of line
}
//second version: can read the blank line
//maybe this is the perfect version
int getline2(char s[],int lim,FILE*fp){
    int ch,i=0;
    while((ch=fgetc(fp))!=EOF&&ch!='\n'&&i<lim){
        s[i++]=ch;
    }
    if(ch==EOF&&i==0)//end of file
        return -1;
    s[i]='\0';
    return i;//the length of line
}
//third version: include newline character LF
//this is same with getline of glibc
int getline3(char s[],int lim,FILE*fp){
    int ch,i=0;
    while((ch=fgetc(fp))!=EOF&&ch!='\n'&&i<lim){
        s[i++]=ch;
    }
    if(ch==EOF&&i==0)//end of file
        return -1;
    if(ch=='\n')//add \n like glibc getline
        s[i++]=ch;
    s[i]='\0';
    return i;//the length of line
}
void simple_test(){
    char line[200];
    int i=0;
    while(getline(line,200,stdin)>-1)
        printf("%d:%s\n",i++,line);
}
void find_the_longest_line(){
    char cur_line[200];
    char longest_line[200];
    int len;
    int max_len=-1;
    while((len=getline(cur_line,200,stdin))>-1){
        if(len>max_len) {
            strcpy(longest_line,cur_line);
            max_len=len;
        }
    }
    printf("MAX:%d:%s\n",max_len,longest_line);
}
int main(){
    find_the_longest_line();
}

#endif