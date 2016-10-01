/*
 * a simple implenmentation of UNIX dd
 */
//#define _DD
#ifdef _DD
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fx_log.h"
#define COPY_BLOCKS(inf,outf,buf,block) do{\
    fread(buf,sizeof(char),block,inf);\
    fwrite(buf,sizeof(char),block,outf);\
    } while (0)

#define READ_WRITE(inf,outf,buf,len,block)do{\
    for(i=0;i+block<len;i+=block){\
    COPY_BLOCKS(inf,outf,buf,block);\
    len_copyed+=block;\
    }\
}while(0)

int dd(FILE *inf,FILE *outf,int sp,int length,int block){
    int i;
    char buffer[1024+1];
    int len_copyed=0;
	int left=length;
    //int block=512;
    fseek(inf,sp,SEEK_SET);
    if(left>block){
        READ_WRITE(inf,outf,buffer,length,block);
        left=length-len_copyed;
    }
    if(left>0){
        READ_WRITE(inf,outf,buffer,left,1);
    }
    //printf("length=%d len_copyed=%d\n",length,len_copyed);
    //assert(length==len_copyed);
    return len_copyed;
}
static void usage(){
    fprintf(stdout,
		"A simple implenmentation of UNIX dd. Version 0.2\n"
		"Usage:dd <infile> <outfile> <startpos> <length> <block>\n"
		"      default block size is 512\n"
		);
}
int main(int argc,char* argv[]){
    int block=512;
	FILE *inf,*outf;
	int startp,length;
    if(argc<5){
        //printf("Not enough parameters\n");
        usage();
        exit(-1);
    }
    if(argc>5)
        block=atoi(argv[5]);
    startp=atoi(argv[3]);
    length=atoi(argv[4]);

    if((inf=fopen(argv[1],"rb"))==NULL){
        fx_log(NULL,FX_LOG_ERROR,"Error open file %s\n",argv[1]);
        exit(1);
    }
    if((outf=fopen(argv[2],"wb"))==NULL){
        fx_log(NULL,FX_LOG_ERROR,"Error open file %s\n",argv[2]);
        exit(1);
    }
    dd(inf,outf,startp,length,block);

    fclose(inf);
    fclose(outf);
}


void read_write(FILE*inf,FILE*outf,char*buffer,int block){
    fread(buffer,sizeof(char),block,inf);
    fwrite(buffer,sizeof(char),block,outf);
}
int dd2(FILE *inf,FILE*outf,int sp,int length){
    int block=512;
    int i;
    char buffer[512];
    int len_copyed=0;
	int left;
    fseek(inf,sp,SEEK_SET);
    if(length>block){
        for(i=0;i+block<length;i+=block){
            read_write(inf,outf,buffer,block);
            len_copyed+=block;
            printf("len_copyed=%d\n",len_copyed);
        }
        printf("A:len_copyed=%d length=%d\n",len_copyed,length);
        left=length-len_copyed;
        if(left>0){
            for(i=0;i<left;i++){
                read_write(inf,outf,buffer,1);
                len_copyed++;
            }
        }
    }
    else{
        for(i=0;i<length;i++){
            read_write(inf,outf,buffer,1);
            len_copyed++;
        }
    }
    printf("length=%d len_copyed=%d\n",length,len_copyed);
    assert(length==len_copyed);
    return len_copyed;
}

int test(int argc,char*argv[]){
    char buffer[1024+1];
    char dump[20];
    char tsname[50];
    int startp;
    int length;
    int duration;
    int i=0;
	FILE *idxf,*inf,*outf;
	char *idxfile,*infile;
    if (argc<3){
        printf("Not enough parameters\n");
        exit(1);
    }
    idxfile=argv[1];//"a.idx";
    infile=argv[2];//"a.ts";
   // char *outfile=argv[2];//"b.ts";
    if((idxf=fopen(idxfile,"r"))==NULL){
        fprintf(stderr, "Error when open %s:\n", idxfile);
        exit(-1);
    }
    if((inf=fopen(infile,"rb"))==NULL){
        fprintf(stderr, "Error when open %s:\n", infile);
        exit(-1);
    }
    while(fgets(buffer,1024,idxf)!=NULL){
    //while(fscanf(idxf,"%s %s %d %d %d\n",dump,tsname,&startp,&length,&duration)>0){
        sscanf(buffer,"%s %s %d %d %d\n",dump,tsname,&startp,&length,&duration);
        fprintf(stdout,"tsname=%s startp=%d length=%d\n",tsname,startp,length);
        outf=fopen(tsname,"wb");
        dd2(inf,outf,startp,length);
        fclose(outf);
        fprintf(stdout,"%s is generated\n",tsname);
        i++;
    }
    fclose(idxf);
    fclose(inf);
}

#endif
