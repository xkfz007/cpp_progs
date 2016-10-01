
#define _SIMPLE_CALCULATOR
#ifdef _SIMPLE_CALCULATOR
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#define ABS(x) ((x)<0?-(x):(x))

double atof_(char s[]){
    double val,power;
    int i,sign;
    for(i=0;isspace(s[i]);i++);
    sign=(s[i]=='-')?-1:1;
    if(s[i]=='+'||s[i]=='-')
        i++;
    for(val=0.0;isdigit(s[i]);i++)
        val=10.0*val+(s[i]-'0');
    if(s[i]=='.')
        i++;
    for(power=1.0;isdigit(s[i]);i++){
        val=10.0*val+(s[i]-'0');
        power*=10;
    }
    val= sign*val/power;
    if(s[i]=='e'||s[i]=='E'){
        int exp;
        sign=(s[++i]=='-')?-1:1;
        if(s[i]=='+'||s[i]=='-')
            i++;
        for(exp=0;isdigit(s[i]);i++)
            exp=10*exp+(s[i]-'0');
        if(sign)
            while(exp-->0)
                val*=10;
        else
            while(exp-->0)
                val/=10;
    }
    return val;
}

#define MAXVAL 1000
int sp=0;
double val[MAXVAL];
void push(double f){
    if(sp<MAXVAL)
        val[sp++]=f;
    else
        printf("error:stack full\n");
}
double pop(void){
    if(sp>0)
        return val[--sp];
    else
        printf("error:stack empty\n");
    return 0.0;
}

#define BUFSIZE 100
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


void mathfnc(char s[]){
    double op2;
    if(strcmp(s,"sin")==0)
        push(sin(pop()));
    else if(strcmp(s,"cos")==0)
        push(cos(pop()));
    else if(strcmp(s,"exp")==0)
        push(exp(pop()));
    else if(strcmp(s,"pow")==0) {
        op2=pop();
        push(pow(pop(),op2));
    }
    else
        printf("error: %s not supported\n",s);
}

#define MAXOP 1000
#define NUMBER '0'
#define NAME 'n'
int getop(char s[]){
    int i,c;
    while((s[0]=c=getch())==' '||c=='\t')
        ;
    s[1]='\0';
    i=0;
    if(islower(c)){
        while(islower(s[++i]=c=getch()))
            ;
        s[i]='\0';
        if(c!=EOF)
            ungetch(c);
        if(strlen(s)>1)
            return NAME;
        else 
            return c;
    }
    if(!isdigit(c)&&c!='.'&&c!='-')
        return c;
    if(c=='-')
        if(isdigit(c=getch())||c=='.')
            s[++i]=c;
        else
        {
            if(c!=EOF)
                ungetch(c);
            return '-';
        }
        if(isdigit(c))
            while(isdigit(s[++i]=c=getch()))
                ;
        if(c=='.')
            while(isdigit(s[++i]=c=getch()))
                ;
        s[i]='\0';
        if(c!=EOF)
            ungetch(c);
        return NUMBER;
}

int main(){
    int type;
    double op1,op2;
    char s[MAXOP];
    while((type=getop(s))!=EOF){
        switch(type){
        case NUMBER:
            push(atof_(s));
            break;
        case NAME:
            mathfnc(s);
            break;
        case '+':
            push(pop()+pop());
            break;
        case '-':
            op2=pop();
            push(pop()-op2);
            break;
        case '*':
            push(pop()*pop());
            break;
        case '/':
            op2=pop();
            if(ABS(op2)>1e-10)
                push(pop()/op2);
            else
                printf("error:zero divisor\n");
            break;
        case '%':
            op2=pop();
            if(ABS(op2)>1e-10)
                push(fmod(pop(),op2));
            else
                printf("error:zero divisor\n");
            break;
        case '\n':
            printf("\t%.8g\n",pop());
            break;
        default:
            printf("error: unknown command %s\n",s);
            break;
        }
    }
}
#endif