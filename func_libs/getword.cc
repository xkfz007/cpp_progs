/*
 * This file contains the some different implenments of getwords()
 */
/*
 * 这儿所说的getword函数，是指从一段字符串中提取出其中的单词。其实这儿主要是分析利用一些标点符号和一些空白字符进行分割而成的单词，
 * 还没有达到编译器所使用的对字符串的分析。编译器是根据语言的文法对语言的源代码进行分析。一般需要利用编译原理中的文法分析来得到具体的token，而这儿的比较简单。
 * 这儿列出了C和C++中的不同实现方式
 */

//#define _GETWORDS
#ifdef _WORDCOUNT
#include <stdio.h>
/*
 * This is a simple implementation of wc on UNIX
*/
static void word_count(){
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

//count words in string, using blance space as delimiter
static int word_count(const char *str){
    int nw=0,nl=0,nc=0;
    int in_word=0;//whether we are in a word now
    const char *p=str;
    while(*p!='\0'){
        if(*p==' '||*p=='\t'||*p=='\n')
            in_word=0;
        else if(!in_word){
            in_word=1;
            nw++;
        }
    }
    return nw;
}
#endif

#ifdef _GETWORDS_C
//test program of getwords
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
 */
/*
 * 在C++中可以利用string类的一些函数来实现，包括find_first_of和find_first_not_of
 */
#ifdef _GETWORDS_CPP
#include <iostream>
#include <string>
#include <vector>
using namespace std;
int getwords(vector<string>& words,const string& line,const string& sep=" :;,.\t\v\r\n\f[]{}()<>+-=*&^%$#@!~`\'\"|\\/?"){
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
