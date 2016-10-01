#ifndef _FX_DUMP_H
#define _FX_DUMP_H
#include "fx_types.h"
#include <stdio.h>
void dump(FILE *f,int NBYTES)
{
    FX_Byte buf[512];
    int count;
    long address=0L;
	//int NBYTES=16;
    int i;
    while((count=fread(buf,sizeof(FX_Byte),NBYTES,f))>0)
    {
        fprintf(stdout," %06X ",address);
        for(i=0;i<NBYTES;++i)
        {
            if(i==NBYTES/2)
                putchar(' ');
            if(i<count)
            {
                fprintf(stdout," %02X",buf[i]);
                if(!isprint(buf[i]))
                        buf[i]='.';
            }
            else
            {
            fputs("   ",stdout);
            buf[i]=' ';
            }
        }
        fprintf(stdout," |%*.*s|\n",NBYTES,NBYTES,buf);
		address+=count;
		fflush(stdout);
    }
}
#ifdef __cplusplus
extern "C" {
#endif
	


#ifdef __cplusplus
}
#endif

#endif