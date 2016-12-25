/*
Write a program that reads a C program and prints in alphabetical order each
group of variable names that are identical in the first 6 characters, but different somewhere
thereafter. Don't count words within strings and comments. Make 6 a parameter that can be set
from the command line.
*/
#define _PRINT_VARIABLES
#ifdef _PRINT_VARIABLES
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define BUFSIZE 10                                 
char buf[BUFSIZE];                                 
int bufp=0;                                        
int getch(void){                                   
    return bufp>0?buf[--bufp]:getchar();           
}                                                  
void ungetch(int c){                               
    if(bufp>=BUFSIZE)                              
        printf("ungetch: too many characters\n");  
    else                                           
        buf[bufp++]=c;                             
}                                                  
int c_comment(void){
    int c;
    while((c=getch())!=EOF)
        if(c=='*')
            if((c=getch())=='/')
                break;
            else
                ungetch(c);
    return c;
}
int fx_getword(char *word,int lim){
    char *w=word;
    int c,d;
    while(isspace(c=getch()))
        ;
    if(c!=EOF)
        *w++=c;
    if(isalpha(c)||c=='_'||c=='#'){
        for(;--lim>0;w++)
            if(!isalnum(*w=getch())&&*w!='_'){
                ungetch(*w);
                break;
            }
    }
    else if(c=='\''||c=='"'){
        for(;--lim>0;w++)
            if((*w=getch())=='\\')
                *++w=getch();
        else if(*w==c){
            w++;
            break;
        }
        else if(*w==EOF)
            break;
            
    }
    else if(c=='/')
        if((d=getch())=='*')
            c=c_comment();
    else
        ungetch(d);
    *w='\0';
    return c;
}
struct tnode{
    char *word;
    int match;
    struct tnode *left;
    struct tnode *right;
};
#define MAXWORD 100
#define YES 1
#define NO 0

int compare(char *s,struct tnode *p,int num,int *found){
    int i;
    char *t=p->word;
    for(i=0;*s==*t;i++,s++,t++)
        if(*s=='\0')
            return 0;
    if(i>=num){
        *found=YES;
        p->match=YES;
    }
    return *s-*t;
}
struct tnode* talloc(void){
    return (struct tnode*)malloc(sizeof(struct tnode));
}
char *strdup_(char *s){
    char *p;
    p=(char*)malloc(strlen(s)+1);
    if(p!=NULL)
        strcpy(p,s);
    return p;
}

struct tnode* addtreex(struct tnode*p,char *w,int num,int *found){
    int cond;
    if(p==NULL){
        p=talloc();
        p->word=strdup_(w);
        p->match=*found;
        p->left=p->right=NULL;
    }
    else if((cond=compare(w,p,num,found))<0)
        p->left=addtreex(p->left,w,num,found);
    else if(cond>0)
        p->right=addtreex(p->right,w,num,found);
    return p;
}
void treeprint(struct tnode* p){
    if(p!=NULL){
        treeprint(p->left);
        if(p->match)
            printf("%s]n",p->word);
        treeprint(p->right);
    }
}
void freetree(struct tnode*p){
    if(p!=NULL){
        freetree(p->left);
        freetree(p->right);
        free(p->word);
    }
}

int main(int argc,char *argv[]){
    struct tnode *root;
    char word[MAXWORD];
    int found=NO;
    int num;
    num=(--argc&&(*++argv)[0]=='-')?atoi(argv[0]+1):6;
    root=NULL;
    while(fx_getword(word,MAXWORD)!=EOF){
        if(isalpha(word[0])&&strlen(word)>=num)
            root=addtreex(root,word,num,&found);
        found=NO;
    }
    treeprint(root);
    freetree(root);
    return 0;
}
#endif