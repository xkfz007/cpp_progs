/*
from C program language 5.12
dcl is based on the grammar that specifies a declarator, which is spelled out precisely in
Appendix A, Section 8.5; this is a simplified form:
dcl: optional *'s direct-dcl
direct-dcl name
                (dcl)
                direct-dcl()
                direct-dcl[optional size]
examples:
  char **argv
  argv: pointer to char

  int (*daytab)[13]
  daytab: pointer to array[13] of int

  int *daytab[13]
  daytab: array[13] of pointer to int

  void *comp()
  comp: function returning pointer to void

  void (*comp)()
  comp: pointer to function returning void

  char (*(*x())[])()
  x: function returning pointer to array[] of

  pointer to function returning char
  char (*(*x[3])())[5]

  x: array[3] of pointer to function returning
  pointer to array[5] of char

  A much more complicated:http://cdecl.org/
*/
//#define _DCL
#ifdef _DCL
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#define MAXTOKEN 100

enum{NAME,PARENS,BRACKETS};
enum{NO,YES};

int tokentype;
char token[MAXTOKEN];
char name[MAXTOKEN];
char datatype[MAXTOKEN];
char out[1000];
int prevtoken=NO;

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
void errmsg(char *msg){
    printf("%s",msg);
    prevtoken=YES;
}

int gettoken(FILE* f){
    char *p=token;
    int c;
    if(prevtoken==YES){
        prevtoken=NO;
        return tokentype;
    }
    while((c=fgetc(f))==' '||c=='\t')
        ;
    if(c=='('){
        if((c=fgetc(f))==')'){
            strcpy(token,"()");
            return tokentype=PARENS;
        }
        else{
            ungetc(c,f);
            return tokentype='(';
        }
    }
    else if(c=='['){
        for(*p++=c;(*p++=fgetc(f))!=']';)
            ;
        *p='\0';
        return tokentype=BRACKETS;
    }
    else if(isalpha(c)){
        for(*p++=c;isalnum(c=fgetc(f));)
            *p++=c;
        *p='\0';
        ungetc(c,f);
        return tokentype=NAME;
    }
    else
        return tokentype=c;
}

int compare(char **s,char **t){
    return strcmp(*s,*t);
}

int typespec(void){
    static char *types[]={
        "char","int","void"
    };
    char *pt=token;
    if(bsearch(&pt,types,sizeof(types)/sizeof(char*),sizeof(char*),compare)==NULL)
        return NO;
    else
        return YES;
}
int typequal(void){
    static char *typeq[]={
        "const","volatile"
    };
    char *pt=token;
    if(bsearch(&pt,typeq,sizeof(typeq)/sizeof(char*),sizeof(char*),compare)==NULL)
        return NO;
    else
        return YES;
}
void dcl(FILE* f);
void dclspec(FILE *f){
    char temp[MAXTOKEN];
    temp[0]='\0';
    gettoken(f);
    do{
        if(tokentype!=NAME){
            prevtoken=YES;
            dcl(f);
        }
        else if(typespec()==YES){
            strcat(temp," ");
            strcat(temp,token);
            gettoken(f);
        }
        else if(typequal()==YES){
            strcat(temp," ");
            strcat(temp,token);
            gettoken(f);
        }
        else
            errmsg("unknown type in parameter list\n");
    }
    while(tokentype!=','&&tokentype!=')');
    strcat(out,temp);
    if(tokentype==',')
        strcat(out,",");
}
void parmdcl(FILE* f){
    do{
        dclspec(f);
    }while(tokentype==',');
    if(tokentype!=')')
        errmsg("missing ) in parameter declaration\n");
}
void dirdcl(FILE* f){
    int type;
    if(tokentype=='('){
        dcl(f);
        if(tokentype!=')')
            printf("error:missing )\n");
    }
    else if(tokentype==NAME) {
        if(name[0]=='\0')
            strcpy(name,token);
    }
    else
        prevtoken=YES;
    while((type=gettoken(f))==PARENS||type==BRACKETS||type=='(')
        if(type==PARENS)
            strcat(out," function returning");
    else if(type=='('){
        strcat(out," function expecting");
        parmdcl(f);
        strcat(out," and returning");
    }
        else{
        strcat(out, " array");
        strcat(out,token);
        strcat(out," of");
    }
}
void dcl(FILE* f){
    int ns;
    for(ns=0;gettoken(f)=='*';)
        ns++;
    dirdcl(f);
    while(ns-->0)
        strcat(out," pointer to");
}

int dcl_main(FILE* f){
    while(gettoken(f)!=EOF){
        strcpy(datatype,token);
        out[0]='\0';
        dcl(f);
        if(tokentype!='\n')
            printf("syntax error\n");
        printf("%s: %s %s\n",name,out,datatype);
    }
    return 0;
}

int nexttoken(FILE* f){
    int type;
    type=gettoken(f);
    prevtoken=YES;
    return type;
}
int undcl_main(FILE* f){
    int type;
    char temp[MAXTOKEN];
    while(gettoken(f)!=EOF){
        strcpy(out,token);
        while((type=gettoken(f))!='\n')
            if(type==PARENS||type==BRACKETS)
                strcat(out,token);
            else if(type=='*'){
                if((type=nexttoken(f))==PARENS||             
                    type==BRACKETS)
                    sprintf(temp,"(*%s)",out);
                else
                    sprintf(temp,"*%s",out);
                strcpy(out,temp);
            }
            else if(type==NAME){
                sprintf(temp,"%s %s",token,out);
                strcpy(out,temp);
            }
            else {
                printf("invalid input at %s\n",token);
            }
    }

    printf("%s\n",out);
    return 0;
}
int main(){
    undcl_main(stdin);

}
#endif