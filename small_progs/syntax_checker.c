//rudimentary syntax checker for c program
//#define _SYNTAX_CHECKER
#ifdef _SYNTAX_CHECKER
#include <stdio.h>
#include <ctype.h>
#include "fx_rcomment.h"
int brace,brack,paren;
void search(int c){
    if(c=='{')
        ++brace;
    else if(c=='}')
        --brace;
    else if(c=='[')
        ++brack;
    else if(c==']')
        --brack;
    else if(c=='(')
        ++paren;
    else if(c==')')
        --paren;
}
//void in_comment(FILE* f){
//    int c,d;
//    c=fgetc(f);
//    d=fgetc(f);
//    while(c!='*'||d!='/'){
//        c=d;
//        d=fgetc(f);
//    }
//}
//void in_quote(int c){
//    int d;
//    while((d=getchar())!=c)
//            if(d=='\\')
//            getchar();
//}
void syntax_checker(FILE* f){
    int ch;
    while((ch=fgetc(f))!=EOF){
        if(ch=='/'){
            if((ch=fgetc(f))=='*')
                ch=handle_c_comment(f);
            else if(ch=='/')
                ch=handle_cpp_comment(f);
            else
                search(ch);
        }
        else if(ch=='\''||ch=='"')
            handle_literal(f,NULL,ch);
        else
            search(ch);
        if(brace<0){
            printf("Unbalanced braces\n");
            brace=0;
        }
        else if(brack<0){
            printf("Unbalanced brackets\n");
            brack=0;
        }
        else if(paren<0){
            printf("Unbalanced parentheses\n");
            paren=0;
        }
    }
    if(brace>0)
        printf("Unbalanced braces\n");
    if(brack>0)
        printf("Unbalanced brackets\n");
    if(paren>0)
        printf("Unbalanced parentheses\n");

}
static void usage(){
	fprintf (stderr, "Syntax checker of stdin or specified files to stdout. Version 0.2\n"
		             "Usage: syntax [files...]\n\n"
					 );
}
int main(int argc,char*argv[])
{
    int i;
	if(argc<2)
		syntax_checker(stdin);

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
            syntax_checker(f);
            fclose(f);
            putchar('\f');
        }
    }
    return 0;
}
#endif