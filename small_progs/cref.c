/*
Write a cross-referencer that prints a list of all words in a document, and for
each word, a list of the line numbers on which it occurs. Remove noise words like ``the,''
``and,'' and so on.
*/
//#define _CROSS_REFERENCER
#ifdef _CROSS_REFERENCER
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "fx_memops.h"
#include "fx_rcomment.h"
//#define BUFSIZE 10                                 
//char buf[BUFSIZE];                                 
//int bufp=0;                                        
//int getch(void){                                   
//    return bufp>0?buf[--bufp]:getchar();           
//}                                                  
//void ungetch(int c){                               
//    if(bufp>=BUFSIZE)                              
//        printf("ungetch: too many characters\n");  
//    else                                           
//        buf[bufp++]=c;                             
//}                                                  
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
//int fx_getword(char *word,int lim){
//    char *w=word;
//    int c,d;
//    while(isspace(c=getch())&&c!='\n')//return '\n' to count the linenumber
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
struct linklist{
    int lnum;
    struct linklist *ptr;
};
struct tnode{
    char *word;//variable
    struct linklist *lines;//line numbers 
    struct tnode *left;
    struct tnode *right;
};
#define MAXWORD 100
#define YES 1
#define NO 0

struct tnode* talloc(void){
    return (struct tnode*)malloc(sizeof(struct tnode));
}
struct linklist *lalloc(void){
    return (struct linklist*)malloc(sizeof(struct linklist));
}
//char *fx_strdup(char *s){
//    char *p;
//    p=(char*)malloc(strlen(s)+1);
//    if(p!=NULL)
//        strcpy(p,s);
//    return p;
//}
static void addln(struct tnode *p,int linenum){
    struct linklist *temp;
    temp=p->lines;
    while(temp->ptr!=NULL&&temp->lnum!=linenum)
        temp=temp->ptr;
    if(temp->lnum!=linenum){
        temp->ptr=lalloc();
        temp->ptr->lnum=linenum;
        temp->ptr->ptr=NULL;
    }
}

static struct tnode* addtreex(struct tnode*p,char *w,int linenum){
    int cond;
    if(p==NULL){
        p=talloc();
        p->word=fx_strdup(w);
        p->lines=lalloc();
        p->lines->lnum=linenum;
        p->lines->ptr=NULL;
        p->left=p->right=NULL;
    }
    else if((cond=strcmp(w,p->word))<0)
        p->left=addtreex(p->left,w,linenum);
    else if(cond>0)
        p->right=addtreex(p->right,w,linenum);
    else
        addln(p,linenum);
    return p;
}
static void treeprint(struct tnode* p){
    struct linklist *temp;
    if(p!=NULL){
        treeprint(p->left);
        printf("%10s: ",p->word);
        for(temp=p->lines;temp!=NULL;temp=temp->ptr)
            printf("%4d ",temp->lnum);
        printf("\n");
        treeprint(p->right);
    }
}
static void freetree(struct tnode*p){
    if(p!=NULL){
        freetree(p->left);
        freetree(p->right);
        free(p->word);
        free(p->lines);
    }
}
static int noiseword(char *w){
    static char *nw[]={
        "a","an","and","are","in","is","of","or","that","the","this","to"
    };
    int cond,mid;
    int low=0;
    int high=sizeof(nw)/sizeof(char*)-1;
    while(low<=high){
        mid=(low+high)/2;
        if((cond=strcmp(w,nw[mid]))<0)
            high=mid-1;
        else if(cond>0)
            low=mid+1;
        else 
            return mid;
    }
    return -1;
}

static int cross_referencer(FILE* f){
    struct tnode *root;
    char word[MAXWORD];
    int linenum=1;
    root=NULL;
    while(fx_getword(f,word,MAXWORD)!=EOF){
        if(isalpha(word[0])&&noiseword(word)==-1)
            root=addtreex(root,word,linenum);
        else if(word[0]=='\n')
            linenum++;
    }
    treeprint(root);
    freetree(root);
    return 0;
}

static void usage(){
	fprintf (stderr, "prints a list of all words of stdin or specified files to stdout. Version 0.2\n"
		             "Usage: cref [files...]\n\n"
					 );
}
int main(int argc,char*argv[])
{
    int i;
	if(argc<2)
		cross_referencer(stdin);

	if(argc==2&&(!strcmp(argv[1],"-h")
		||!strcmp(argv[1],"--help"))){
			usage();
			exit(1);
	}

    for(i=1;i<argc;++i)
    {
        FILE *f;
        if((f=fopen(argv[i],"rb"))==0)
            fprintf(stderr,"Can't open %s\n",argv[i]);
        else
        {
            cross_referencer(f);
            fclose(f);
            putchar('\f');
        }
    }
    return 0;
}
#endif