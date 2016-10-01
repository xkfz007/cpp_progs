/*
a program to count the occurrences of each C keyword
*/
//#define _KEYWORDS
#ifdef _KEYWORDS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "fx_rcomment.h"

#define MAXWORD 1000
struct key {
    char *word;
    int count;
}
keytab[]={
    "auto",0,
    "break",0,
    "case",0,
    "char",0,
    "const",0,
    "continue",0,
    "default",0,
    "do",0,
    "double",0,
    "else",0,
    "enum",0,
    "extern",0,
    "float",0,
    "for",0,
    "goto",0,
    "if",0,
    "int",0,
    "long",0,
    "register",0,
    "return",0,
    "short",0,
    "signed",0,
    "sizeof",0,
    "static",0,
    "struct",0,
    "switch",0,
    "typedef",0,
    "union",0,
    "unsigned",0,
    "void",0,
    "volatile",0,
    "while",0,
};
#define NKEYS (sizeof keytab/sizeof(keytab[0]))
#define BUFSIZE 10                                 
char buf[BUFSIZE];                                 
int bufp=0;                                        
//int getch(void){                                   
//    return bufp>0?buf[--bufp]:getchar();           
//}                                                  
//void ungetch(int c){                               
//    if(bufp>=BUFSIZE)                              
//        printf("ungetch: too many characters\n");  
//    else                                           
//        buf[bufp++]=c;                             
//}                                                  
int binsearch(char *word,struct key tab[],int n){
    int cond;
    int low,high,mid;
    low=0;
    high=n-1;
    while(low<=high){
        mid=(low+high)/2;
        if((cond=strcmp(word,tab[mid].word))<0)
            high=mid-1;
        else if(cond>0)
            low=mid+1;
        else
            return mid;
    }
    return -1;
}
//int c_comment(void){
//    int c;
//    while((c=getch())!=EOF)
//        if(c=='*')
//            if((c=getch())=='/')
//                break;
//            else
//                ungetch(c);
//    return c;
//}
//int getword(char *word,int lim){
//    char *w=word;
//    int c,d;
//    while(isspace(c=getch()))
//        ;
//    if(c!=EOF)
//        *w++=c;
//    if(isalpha(c)||c=='_'||c=='#'){
//        for(;--lim>0;w++)
//            if(!isalnum(*w=getch())&&*w!='_'){
//                ungetch(*w);
//                break;
//            }
//    }
//    else if(c=='\''||c=='"'){
//        for(;--lim>0;w++)
//            if((*w=getch())=='\\')
//                *++w=getch();
//        else if(*w==c){
//            w++;
//            break;
//        }
//        else if(*w==EOF)
//            break;
//            
//    }
//    else if(c=='/')
//        if((d=getch())=='*')
//            c=c_comment();
//    else
//        ungetch(d);
//    *w='\0';
//    return c;
//}
int keywords(FILE* f){
    int n;
    char word[MAXWORD];
    //FILE* f=stdin;
    while(fx_getword(f,word,MAXWORD)!=EOF)
        if(isalpha(word[0]))
            if((n=binsearch(word,keytab,NKEYS))>=0)
                keytab[n].count++;
    for(n=0;n<NKEYS;n++)
        if(keytab[n].count>0)
            printf("%4d %s\n",
                    keytab[n].count,keytab[n].word);
    return 0;
}
static void usage(){
	fprintf (stderr, "Count keywords in C/C++ programs of stdin or specified files to stdout. Version 0.2\n"
		             "Usage: keywords [files...]\n\n"
					 );
}
int main(int argc,char*argv[])
{
    int i;
	if(argc<2)
		keywords(stdin);

	if(argc==2&&(!strcmp(argv[1],"-h")
		||!strcmp(argv[1],"--help"))){
			usage();
			exit(1);
	}

    for(i=1;i<argc;++i)
    {
        FILE *f;
        if((f=fopen(argv[i],"r"))==0)
            fprintf(stderr,"Can't open %s\n",argv[i]);
        else
        {
			fprintf(stdout,"Dump of %s:\n\n",argv[i]);
            keywords(f);
            fclose(f);
            putchar('\f');
        }
    }
    return 0;
}
#endif