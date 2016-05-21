/*
This is a simple implementation of wc on UNIX
*/
//#define _GETWORDS
#ifdef _GETWORDS
#include <stdio.h>
void word_count(){
    int nw=0,nl=0,nc=0;
    int in_word=0;//whether we are in a word now
    char ch;
    while((ch=fgetc(stdin))!=EOF){
        nc++;
        if(ch=='\n')
            nl++;
        if(ch==' '||ch=='\t'||ch=='\n')
            in_word=0;
        else if(!in_word){
            in_word=1;
            nw++;
        }
    }
    printf("%3d %3d %3d\n",nl,nw,nc);
}

#if 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int is_in_str(const char*s,char ch){
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
int getwords_using_strtok(char* words[],char *line,const char* delim){
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
int getwords_using_strpbrk(char* words[],const char* line,const char* delim){
    char *pos,*pos2;
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
#define getwords getwords_using_no_lib
int main()
{
    char buf[BUFSIZ];
    char *words[100];
    char *delim=" :;,.\t\v\r\n\f[]{}()<>+-=*&^%$#@!~`\'\"|\\/?";
    //FILE* fp=fopen(path,"r");
    //if(!fp)
    //{
    //printf("Can't open file:%s\n",path);
    //exit(-1);
    //}
    FILE* fp=stdin;
    while(fgets(buf,BUFSIZ,fp)!=NULL)
    {
        printf("%s",buf);
        int count=getwords(words,buf,delim);
        int i;
        if(count>0)
            for(i=0;i<count;i++)
                printf("==>%d:%s\n",i,words[i]);
    }
    fclose(fp);
    return 0;
}
#undef getwords
#endif

/*
* a c++ version of getwords
* */
#if 0
#include <iostream>
#include <string>
#include <vector>
using namespace std;
int getwords(vector<string>& words,string line,string sep=" :;,.\t\v\r\n\f[]{}()<>+-=*&^%$#@!~`\'\"|\\/?"){
    string word;
    string::size_type pos=0,pos2=0;
    int cnt=0;
    while((pos2=line.find_first_not_of(sep,pos))
        !=string::npos){
            pos=line.find_first_of(sep,pos2);
            if(pos!=string::npos){
                word=line.substr(pos2,pos-pos2);
            }
            else{
                word=line.substr(pos2);
            }
            words.push_back(word);
            cnt++;
    }
    return cnt;
}
int main(){
    string line;
    string sep(" :;,.\t\v\r\n\f[]{}()<>+-=*&^%$#@!~`\'\"|\\/?");
    vector<string> words;
    while(getline(cin,line)){
        int cnt=getwords(words,line,sep);
        if(cnt>0){
            vector<string>::iterator ite;
            cout<<line<<endl;
            for(ite=words.begin();ite!=words.end();++ite)
                cout<<"==>"<<*ite<<endl;
            words.clear();
        }
    }
    return 0;
}
#endif
#endif