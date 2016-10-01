
#define _MACRO_PREPROCESSOR
#ifdef _MACRO_PREPROCESSOR
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "fx_rcomment.h"
#include "fx_log.h"
#include "fx_memops.h"
#include "fx_string.h"
#include "fx_getword.h"
//char buf[BUFSIZ];                                 
//int bufp=0;                                        
//int getch(void){                                   
//    return bufp>0?buf[--bufp]:getchar();           
//}                                                  
//void ungetch(int c){                               
//    if(bufp>=BUFSIZ)                              
//        printf("ungetch: too many characters\n");  
//    else                                           
//        buf[bufp++]=c;                             
//}                                                  
//char *strdup_(char *s){
//    char *p;
//    p=(char*)malloc(strlen(s)+1);
//    if(p!=NULL)
//        strcpy(p,s);
//    return p;
//}

//int c_comment(FILE* f){
//    int c;
//    while((c=getch())!=EOF)
//        if(c=='*')
//            if((c=getch())=='/')
//                break;
//            else
//                ungetch(c);
//    return c;
//}
struct nlist{
    struct nlist *next;
    char *name;
    char *defn;
};

#define HASHSIZE 101
static struct nlist *hashtab[HASHSIZE];
/* hash: form hash value for string s */
unsigned hash(char *s)
{
    unsigned hashval;
    for(hashval=0;*s!='\0';s++)
        hashval=*s+32*hashval;
    return hashval%HASHSIZE;
}
/* lookup: look for s in hashtab */
struct nlist *lookup(char *s){
    struct nlist *np;
    for(np=hashtab[hash(s)];np!=NULL;np=np->next)
        if(strcmp(s,np->name)==0)
            return np;
    return NULL;
}
/* install: put (name, defn) in hashtab */
struct nlist *install(char *name,char*defn){
    struct nlist *np;
    unsigned hashval;
    if((np=lookup(name))==NULL){
        np=(struct nlist *)fx_malloc(sizeof(struct nlist));
        if(np==NULL||(np->name=fx_strdup(name))==NULL)
            return NULL;
        hashval=hash(name);
        np->next=hashtab[hashval];
        hashtab[hashval]=np;
    }
    else
        fx_free((void*)np->defn);
    if((np->defn=fx_strdup(defn))==NULL)
        return NULL;
    return np;
}
/* remove a name and definition from the table*/
void undef(char*s){
    int h;
    struct nlist *prev,*np;
    prev=NULL;
    h=hash(s);
    for(np=hashtab[h];np!=NULL;np=np->next){
        if(strcmp(s,np->name)==0)
            break;
        prev=np;
    }
    if(np!=NULL){
        if(prev==NULL)
            hashtab[h]=np->next;
        else
            prev->next=np->next;
        fx_free(np->name);
        fx_free(np->defn);
        fx_free(np);
    }
}

void greed_until_nextline(FILE *f,int ch){
    while(ch!=EOF&&ch!='\n')
        ch=fgetc(f);
}
//void skipblanks(){
//    int c;
//    while((c=getch())==' '||c=='\t')
//        ;
//    ungetch(c);
//}
#define MAXWORD 100
void getdef(FILE* f){
    int i;
    char def[MAXWORD],dir[MAXWORD],name[MAXWORD];
    skipblanks(f);
    if(!isalpha(fx_getword(f, dir, MAXWORD))){
        fx_log(NULL,FX_LOG_WARNING,"getdef:expecting a directive after #");
        greed_until_nextline(f,dir[0]);
    }
    else if(strcmp(dir,"define")==0){
        skipblanks(f);
        if(!isalpha(fx_getword(f, name, MAXWORD))){
            fx_log(NULL,FX_LOG_WARNING,"getdef:non-alpha -name expected");
            greed_until_nextline(f,name[0]);
        }
        else{
            skipblanks(f);
            for(i=0;i<MAXWORD-1;i++)
                if((def[i]=fgetc(f))==EOF||
                        def[i]=='\n')
                    break;
            def[i]='\0';
            if(i<=0){
                fx_log(NULL,FX_LOG_WARNING,"getdef:incomplete define");
                greed_until_nextline(f,'\n');
            }
            else
                install(name,def);
        }
    }
    else if(strcmp(dir,"undef")==0){
        skipblanks(f);
        if(!isalpha(fx_getword(f, name, MAXWORD))){
           fx_log(NULL,FX_LOG_WARNING,"getdef:non-alpha in undef");
            greed_until_nextline(f,name[0]);
        }
        else
            undef(name);
    }
    else{
        fx_log(NULL,FX_LOG_WARNING,"getdef:expecting a directive after #");
        greed_until_nextline(f,dir[0]);
    }
}

int preprocess(FILE* f,FILE* outf){
    char w[MAXWORD];
    struct nlist *p;
    while(fx_getword(f,w,MAXWORD)!=EOF){
        if(strcmp(w,"#")==0)
            getdef(f);
        else if(!isalpha(w[0]))
            fprintf(outf,"%s",w);
        else if((p=lookup(w))==NULL)
            fprintf(outf,"%s",w);
        else 
            fx_ungets(f,p->defn);
    }
    return 0;
}
#endif