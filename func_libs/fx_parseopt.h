#ifndef _FX_PARSEOPT_H
#define _FX_PARSEOPT_H

#include "fx_log.h"

static int parse_opt_string(char* arg,char **x,char**y,char delim){
	char *p;
	if(arg==NULL){
		fx_log(NULL,FX_LOG_ERROR,"Error:empty arg\n");
		return 0;
	}
	p=arg;
	while(*p&&*p!=delim)
		p++;
	if(p==arg//format ":y"
		||*p=='\0'//format "x"
		||*(p+1)=='\0'//format "x:"
		){
		fx_log(NULL,FX_LOG_ERROR,"Error:invalid format of arg %s\n",arg);
		return -1;
	}
	*x=arg;
	*y=p+1;
	*p='\0';
	return  1;
}

#define PARSE_OPT(name,type,func) \
static int name(char* arg,type *x,type *y,char delim){\
	int ret;\
	char *a,*b;\
	if((ret=parse_opt_string(arg,&a,&b,delim))>0){\
		*x=func(a);\
		*y=func(b);\
		return 1;\
	}\
\
	return ret;\
}
PARSE_OPT(parse_opt_int,int,atoi);
PARSE_OPT(parse_opt_int64,int64_t,atoll);


#endif