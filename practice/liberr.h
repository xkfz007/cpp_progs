#ifndef _LIBERR_H_
#define _LIBERR_H_
#include <stdarg.h>
#define MAXLINELEN 4096
void err_ret(const char*fmt,...);
void err_quit(const char*fmt,...);
void log_ret(char*logfile ,const char*fmt,...);
void log_quit(char*logfile,const char*fmt,...);
void err_prn(const char*fmt,va_list ap,char*logfile);
#endif
