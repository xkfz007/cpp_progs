/*****************************************************************************
 * simple arithmetic expression evaluator
 *****************************************************************************
 * Copyright (c) 2002 Michael Niedermayer <michaelni@gmx.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *****************************************************************************/

/**
 * @file eval.c
 * simple arithmetic expression evaluator.
 *
 * see http://joe.hotchkiss.com/programming/eval/eval.html
 */
#ifdef _EVAL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef NAN
  #define NAN 0
#endif

#ifndef M_E
#define M_E            2.7182818284590452354   /* e */
#endif
#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif
#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif
#ifndef M_LOG2_10
#define M_LOG2_10      3.32192809488736234787  /* log_2 10 */
#endif
#ifndef M_PHI
#define M_PHI          1.61803398874989484820   /* phi / golden ratio */
#endif
#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif
#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif
#ifndef M_SQRT2
#define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
#endif



#define STACK_SIZE 100

typedef struct Parser{
    double stack[STACK_SIZE];
    int stack_index;
    char *s;
    double *const_value;
    const char **const_name;          // NULL terminated
    double (**func1)(void *, double a); // NULL terminated
    const char **func1_name;          // NULL terminated
    double (**func2)(void *, double a, double b); // NULL terminated
    char **func2_name;          // NULL terminated
    void *opaque;
} Parser;

static void evalExpression(Parser *p);

static void push(Parser *p, double d){
    if(p->stack_index+1>= STACK_SIZE){
        fprintf(stderr, "stack overflow in the parser\n");
        return;
    }
    p->stack[ p->stack_index++ ]= d;
//printf("push %f\n", d); fflush(stdout);
}

static double pop(Parser *p){
    if(p->stack_index<=0){
        fprintf(stderr, "stack underflow in the parser\n");
        return NAN;
    }
//printf("pop\n"); fflush(stdout);
    return p->stack[ --p->stack_index ];
}

static int strmatch(const char *s, const char *prefix){
    int i;
    for(i=0; prefix[i]; i++){
        if(prefix[i] != s[i]) return 0;
    }
    return 1;
}

static void evalPrimary(Parser *p){
    double d, d2=NAN;
    char *next= p->s;
    int i;

    /* number */
    d= strtod(p->s, &next);
    if(next != p->s){
        push(p, d);
        p->s= next;
        return;
    }

    /* named constants */
    for(i=0; p->const_name[i]; i++){
        if(strmatch(p->s, p->const_name[i])){
            push(p, p->const_value[i]);
            p->s+= strlen(p->const_name[i]);
            return;
        }
    }

    p->s= strchr(p->s, '(');
    if(p->s==NULL){
        fprintf(stderr, "Parser: missing ( in \"%s\"\n", next);
        return;
    }
    p->s++; // "("
    evalExpression(p);
    d= pop(p);
    if(p->s[0]== ','){
        p->s++; // ","
        evalExpression(p);
        d2= pop(p);
    }
    if(p->s[0] != ')'){
        fprintf(stderr, "Parser: missing ) in \"%s\"\n", next);
        return;
    }
    p->s++; // ")"

         if( strmatch(next, "sinh"  ) ) d= sinh(d);
    else if( strmatch(next, "cosh"  ) ) d= cosh(d);
    else if( strmatch(next, "tanh"  ) ) d= tanh(d);
    else if( strmatch(next, "sin"   ) ) d= sin(d);
    else if( strmatch(next, "cos"   ) ) d= cos(d);
    else if( strmatch(next, "tan"   ) ) d= tan(d);
    else if( strmatch(next, "exp"   ) ) d= exp(d);
    else if( strmatch(next, "log10"   ) ) d= log10(d);
    else if( strmatch(next, "log2"   ) ) d= log10(d)/log10(2);
    else if( strmatch(next, "log"   ) ) d= log(d);
    else if( strmatch(next, "sqrt"   ) ) d= sqrt(d);
    else if( strmatch(next, "squish") ) d= 1/(1+exp(4*d));
    else if( strmatch(next, "gauss" ) ) d= exp(-d*d/2)/sqrt(2*M_PI);
    else if( strmatch(next, "abs"   ) ) d= fabs(d);
    else if( strmatch(next, "max"   ) ) d= d > d2 ? d : d2;
    else if( strmatch(next, "min"   ) ) d= d < d2 ? d : d2;
    else if( strmatch(next, "gte"    ) ) d= d >= d2 ? 1.0 : 0.0;
    else if( strmatch(next, "gt"    ) ) d= d > d2 ? 1.0 : 0.0;
    else if( strmatch(next, "lte"    ) ) d= d >= d2 ? 0.0 : 1.0;
    else if( strmatch(next, "lt"    ) ) d= d > d2 ? 0.0 : 1.0;
    else if( strmatch(next, "eq"    ) ) d= d == d2 ? 1.0 : 0.0;
//    else if( strmatch(next, "l1"    ) ) d= 1 + d2*(d - 1);
//    else if( strmatch(next, "sq01"  ) ) d= (d >= 0.0 && d <=1.0) ? 1.0 : 0.0;
    else{
        int error=1;
        for(i=0; p->func1_name && p->func1_name[i]; i++){
            if(strmatch(next, p->func1_name[i])){
                d= p->func1[i](p->opaque, d);
                error=0;
                break;
            }
        }

        for(i=0; p->func2_name && p->func2_name[i]; i++){
            if(strmatch(next, p->func2_name[i])){
                d= p->func2[i](p->opaque, d, d2);
                error=0;
                break;
            }
        }

        if(error){
            fprintf(stderr, "Parser: unknown function in \"%s\"\n", next);
            return;
        }
    }

    push(p, d);
}

static void evalPow(Parser *p){
    int neg= 0;
    if(p->s[0]=='+') p->s++;

    if(p->s[0]=='-'){
        neg= 1;
        p->s++;
    }

    if(p->s[0]=='('){
        p->s++;;
        evalExpression(p);

        if(p->s[0]!=')')
            fprintf(stderr, "Parser: missing )\n");
        p->s++;
    }else{
        evalPrimary(p);
    }

    if(neg) push(p, -pop(p));
}

static void evalFactor(Parser *p){
    evalPow(p);
    while(p->s[0]=='^'){
        double d;

        p->s++;
        evalPow(p);
        d= pop(p);
        push(p, pow(pop(p), d));
    }
}

static void evalTerm(Parser *p){
    evalFactor(p);
    while(p->s[0]=='*' || p->s[0]=='/'){
        int inv= p->s[0]=='/';
        double d;

        p->s++;
        evalFactor(p);
        d= pop(p);
        if(inv) d= 1.0/d;
        push(p, d * pop(p));
    }
}

static void evalExpression(Parser *p){
    evalTerm(p);
    while(p->s[0]=='+' || p->s[0]=='-'){
        int sign= p->s[0]=='-';
        double d;

        p->s++;
        evalTerm(p);
        d= pop(p);
        if(sign) d= -d;
        push(p, d + pop(p));
    }
}

static double x264_eval(char *s, double *const_value, const char **const_name,
                 double (**func1)(void *, double), const char **func1_name,
                 double (**func2)(void *, double, double), char **func2_name,
                 void *opaque){
    Parser p;

    p.stack_index=0;
    p.s= s;
    p.const_value= const_value;
    p.const_name = const_name;
    p.func1      = func1;
    p.func1_name = func1_name;
    p.func2      = func2;
    p.func2_name = func2_name;
    p.opaque     = opaque;

    evalExpression(&p);
    return pop(&p);
}
static double calc_eq(char*eq){

    double const_values[]={
        M_PI,
        M_E,
        M_PHI,
        0
    };
    static const char *const_names[]={
        "PI",
        "E",
        "PHI",
        NULL
    };
    static double (*func1[])(void *, double)={
//      (void *)bits2qscale,
        //(void *)qscale2bits,
        NULL
    };
    static const char *func1_names[]={
//      "bits2qp",
        //"qp2bits",
        NULL
    };

    double q=0.0 ;
    q= x264_eval(eq, const_values, const_names, func1, func1_names, NULL, NULL, NULL);
    return q;
}
void help(){
    fprintf(stdout,"usage:eval.exe <expression>\n");
    fprintf(stdout,"built-in functions:\n"
        "\t\t\tsinh,cosh,tanh,sin,cos,tan,\n\
         \t\t\texp,log,squish,gauss,\n\
         \t\t\tabs,max,min,gt,gte,lt,lte,eq\n");
    fprintf(stdout,"built-in constants:\n"
        "\t\t\tPI,E\n");
}
int main(int argc,char*argv[]){
    double v;
    int i;
    char *eq;
    if(argc<2){
        help();
        return -1;
    }
    for(i=1;i<argc;i++){
        eq=argv[i];
        v=calc_eq(eq);
        fprintf(stdout,"%s=%7.4f\n",eq,v);
    }

}

#endif
