/*
 * remove all comments from a valid c program
 */
#ifndef _FX_RCOMMENT_H
#define _FX_RCOMMENT_H
#include <stdio.h>

//void echo_quote(int c){
//    int d;
//    putchar(c);
//    while((d=getchar())!=c){//search for end
//        putchar(d);
//        if(d=='\\')
//            putchar(getchar());//ignore the escape seq
//    }
//    putchar(d);
//}
//
//void in_comment(void){
//    int c,d;
//    c=getchar();
//    d=getchar();
//    while(c!='*'||d!='/'){
//        c=d;
//        d=getchar();
//    }
//}
//void rcomment(int c){
//    int d;
//    if(c=='/')
//        if((d=getchar())=='*')
//            in_comment();
//        else if(d=='/'){
//            putchar(c);
//            rcomment(d);
//        }
//        else{
//            putchar(c);
//            putchar(d);
//        }
//    else if(c=='\''||c=='"')
//        echo_quote(c);
//    else
//        putchar(c);
//}
//int main(){
//    int c;
//	FILE *f;
//	f=stdin;
//	while((c=fgetc(f))!=EOF)
//        rcomment(c);
//    return 0;
//}

typedef enum ctx{
    C_COMMENT,
	CPP_COMMENT,
	STRING_LITERAL,
	CHAR_LITERAL,
	FILE_END,
}Context;

static int handle_c_comment(FILE* f){
    int ch;
    while((ch=fgetc(f))!=EOF){
        if(ch=='*'){
            while((ch=fgetc(f))!=EOF&&ch=='*')
                ;
            if(ch=='/')
                break;
        }
    }
    return ch;
}
static int handle_cpp_comment(FILE* f){
    int ch;
    while((ch=fgetc(f))!=EOF&&ch!='\n')
        ;
    return ch;
}
static int handle_literal(FILE* f, FILE* outf, char delimiter){
    char ch;
    if(outf!=NULL)
        fprintf(outf,"%c",delimiter);
    while((ch=fgetc(f))!=EOF){
        if(outf!=NULL)
            fprintf(outf,"%c",ch);
        if(ch==delimiter)
            break;
		else if (ch=='\\'){
			if((ch=fgetc(f))!=EOF&&outf!=NULL)
				fprintf(outf,"%c",ch);
		}
    }
    return ch;
}
static Context handle_code(FILE* f,FILE* outf){
    int ch;
    while((ch=fgetc(f))!=EOF){
        switch(ch){
            case '/':
                if((ch=fgetc(f))==EOF){
					fprintf(outf,"/");
                    return FILE_END;
                }
                else {
                    if(ch=='*')
                        return C_COMMENT;
                    else if(ch=='/')
                        return CPP_COMMENT;
                    else{
						fprintf(outf,"/");
						ungetc(ch,outf);
                        break;
                    }
                }
            case '\"':
                return STRING_LITERAL;
            case '\'':
                return CHAR_LITERAL;
            default:
				fprintf(outf,"%c",ch);
        }
    }
    return FILE_END;    
}
static int remove_comments(FILE* f, FILE* outf){
	Context contxt;
	//FILE* f,*outf;
	//f=stdin;
	//outf=stdout;
	while((contxt=handle_code(f,outf))!=FILE_END)
		switch(contxt){
				case C_COMMENT:
					handle_c_comment(f);
					break;
				case CPP_COMMENT:
					handle_cpp_comment(f);
					break;
				case STRING_LITERAL:
					handle_literal(f,outf,'\"');
					break;
				case CHAR_LITERAL:
					handle_literal(f,outf,'\'');
					break;
	}
	return 0;
}


static int skipblanks(FILE* f){
    int c;
    while((c=fgetc(f))==' '||c=='\t')
        ;
    ungetc(c,f);
}

static int fx_getword(FILE* f, char *word,int lim){
    char *p=word;
    int ch,d;
    while(isspace(ch=fgetc(f))&&ch!='\n'&&ch!=' ')//return '\n' and ' '
        ;
    if(ch!=EOF)
        *p++=ch;

    /*word*/
    if(isalpha(ch)||ch=='_'){//||c=='#'){
        for(;--lim>0;p++)
            if(!isalnum(*p=fgetc(f))&&*p!='_'){
                //ungetch(*p);
                ungetc(*p,f);
                break;
            }
    }
    else if(ch=='\''||ch=='"'){/*literal*/
        //for(;--lim>0;p++)
        //    if((*p=fgetc())=='\\')//escape
        //        *++p=fgetc();
        //else if(*p==ch){//end of literal
        //    p++;
        //    break;
        //}
        //else if(*p==EOF)
        //    break;
        ch=handle_literal(f,NULL,ch);
            
    }
    else if(ch=='/'){
        if((d=fgetc(f))=='*')
            ch=handle_c_comment(f);
        else if(d=='/')
            ch=handle_cpp_comment(f);
        else
            //ungetch(d);
            ungetc(d,f);
    }
    *p='\0';
    return ch;
}
#endif