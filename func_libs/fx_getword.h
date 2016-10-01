#ifndef _FX_GETWORD_H
#define _FX_GETWORD_H

#include <string.h>
//check if ch is in s, like strchr
static int is_in_str(const char*s,char ch){
    int flag=0;
    while(*s!='\0'){
        if(ch==*s){
            flag=1;
            break;
        }
        s++;
    }
    return flag;
}
//in this verion, string will be changed
int getwords_using_no_lib(char* words[],char*line,const char*delim){
    int in_word=0;
    int ch;
    char *p;
    int cnt=0;
    p=line;
    while(*p!='\0'){
        if(is_in_str(delim,*p)){
            in_word=0;
            *p='\0';
        }
        else if(!in_word) {
            in_word=1;
            words[cnt++]=p;
        }
        p++;
    }
    return cnt;
}

/*
*  using strtok
*/
static int getwords_using_strtok(char* words[],char *line,const char* delim){
    int count;
    char* token;
    token=strtok(line,delim);
    if(token==NULL)
        return 0;
    count=0;
    words[count++]=token;
    while((token=strtok(NULL,delim))!=NULL)
        words[count++]=token;
    return count;
}
/*
*getwords: implemented using strpbrk
*/
static int getwords_using_strpbrk(char* words[],char* line,const char* delim){
    char *pos;
    char*pos2;
    int count=0;
    pos2=line;
    while((pos=strpbrk(pos2,delim))!=NULL){
        *pos='\0';
        if(pos!=pos2)
            words[count++]=pos2;
        pos2=pos+1;
    }
    if(*pos2!='\0')
        words[count++]=pos2;
    return count;
}
#define fx_getwords getwords_using_strpbrk



#endif