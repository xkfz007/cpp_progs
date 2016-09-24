/*
 * calculate psnr between original yuv and reconstructed yuv
 */
#define _PSNR
#ifdef _PSNR
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fx_types.h"
#include "fx_fileops.h"
typedef unsigned char byte;

static int show_frame_number=0;
static int calc_average=0;
static int depth=8;

#include "fx_log.c"

static double calc_psnr(byte* orig,byte* rec,int N){
    int i;
    int err_sum=0;
    int e;
    double mse;
	int Max=(1<<depth)-1;
    for(i=0;i<N;i++){
        e=*(orig+i)-*(rec+i);
        err_sum+=e*e;
    }
    mse=(err_sum*1.0/N);
	if(mse)
		return 10*log10(Max*Max/mse);
	else
		return 0;
}
static void calc_all(FILE* p_orig,FILE* p_rec,int from,int to,int width,int height){
    double psnr[3],psnr_sum[3];
    int pic_num;
    int frame_sz;
    int luma_sz,chroma_sz;
    byte *orig_frm,*rec_frm;
	int ret;
    //frame_sz=height*width+height/2*width/2*2;
    luma_sz=height*width;
    chroma_sz=height/2*width/2;
	frame_sz=luma_sz+2*chroma_sz;
    if(NULL==(orig_frm=(byte*)calloc(frame_sz,sizeof(byte)))
            ||NULL==(rec_frm=(byte*)calloc(frame_sz,sizeof(byte)))) {
        general_log(NULL,GENL_LOG_ERROR,"Merrory allocation error!\n");
        exit(-2);
    }
    pic_num=from;
    psnr_sum[0]=psnr_sum[1]=psnr_sum[2]=0.0;
    if((ret=fseeko(p_orig,(uint64_t)from*frame_sz, SEEK_SET))) {
    //if((ret1=fseek(p_orig,from*frame_sz, SEEK_SET))) {
		general_log(NULL,GENL_LOG_ERROR,"Seek origin file error\n");
		exit(-2);
	}
    if((ret=fseeko(p_rec,(uint64_t)from*frame_sz, SEEK_SET))) {
    //if((ret1=fseek(p_rec,from*frame_sz, SEEK_SET))) {
		general_log(NULL,GENL_LOG_ERROR,"Seek rec file error\n");
		exit(-2);
	}
	//ret1=fread(orig_frm,sizeof(byte),frame_sz,p_orig);
	//fprintf(stderr,"error %d",errno);

	//ret2=fread(rec_frm,sizeof(byte),frame_sz,p_rec);
	//fprintf(stderr,"error %d",errno);

	
	while(pic_num<=to)
	{
		if(frame_sz!=(ret=fread(orig_frm,sizeof(byte),frame_sz,p_orig)))
		{
			general_log(NULL,GENL_LOG_ERROR,"Read origin file error, ret=%d\n",ret);
			exit(-2);
		}
		if(frame_sz!=(ret=fread(rec_frm,sizeof(byte),frame_sz,p_rec))){
			general_log(NULL,GENL_LOG_ERROR,"Read rec file error, ret=%d\n",ret);
			exit(-2);
		}

        psnr[0]=calc_psnr(orig_frm,                  rec_frm,                  luma_sz);
        psnr[1]=calc_psnr(orig_frm+luma_sz,          rec_frm+luma_sz,          chroma_sz);
        psnr[2]=calc_psnr(orig_frm+luma_sz+chroma_sz,rec_frm+luma_sz+chroma_sz,chroma_sz);
        psnr_sum[0]+=psnr[0];
        psnr_sum[1]+=psnr[1];
        psnr_sum[2]+=psnr[2];
		if(show_frame_number)
			fprintf(stdout,"%4d ",pic_num);
		fprintf(stdout,"%7.4f %7.4f %7.4f\n",psnr[0],psnr[1],psnr[2]);
		fflush(stdout);
		general_log(NULL,GENL_LOG_INFO,"Frame %d is done\n",pic_num);
		pic_num++;
	}
	if(calc_average)
		printf("avg: %7.4f %7.4f %7.4f\n",psnr_sum[0]/(pic_num-from),psnr_sum[1]/(pic_num-from),psnr_sum[2]/(pic_num-from));
    free(orig_frm);
    free(rec_frm);
}

static int count_frames(FILE*fp,int width,int height){
	uint64_t len=0;
    int framebytes=height*width+height/2*width/2*2;
	if(fseeko(fp,0,SEEK_END)<0)
		return -1; 
	if((len=ftello(fp))<0)
		return -1;

	return len/framebytes;

}
static void usage(){
	printf("Usage:\n"
		   "\tpsnr <width> <height> <orig.yuv> <rec.yuv> from to\n"
		   "or\n"
           "\tpsnr -s <width>x<height> -i <orig.yuv>:<rec.yuv> -r from:to -n -c\n"
		   "\t-n: show frame number\n"
		   "\t-c: calculate averge psnr values\n"
		   "Examples:\n"
		   "\tpsnr 176 144 foreman_qcif.yuv foreman_qcif_rec.yuv 5 100\n"
		   "\tpsnr -s 176x144 -i foreman_qcif.yuv:foreman_qcif_rec.yuv -r 5:100\n"
		   );
}
static int parse_opt_string(char* arg,char **x,char**y,char delim){
	char *p;
	if(arg==NULL){
		general_log(NULL,GENL_LOG_ERROR,"Error:empty arg\n");
		return 0;
	}
	p=arg;
	while(*p&&*p!=delim)
		p++;
	if(p==arg//format ":y"
		||*p=='\0'//format "x"
		||*(p+1)=='\0'//format "x:"
		){
		general_log(NULL,GENL_LOG_ERROR,"Error:invalid format of arg %s\n",arg);
		return -1;
	}
	*x=arg;
	*y=p+1;
	*p='\0';
	return  1;
}
static int parse_opt_int(char* arg,int *x,int *y,char delim){
	int ret;
	char *a,*b;
	if((ret=parse_opt_string(arg,&a,&b,delim))>0){
		*x=atoi(*a);
		*y=atoi(*b);
		return 1;
	}

	return ret;
}

int main(int argc,char*argv[])
{
    FILE *p_orig, *p_rec;
    int height,width;
    int from=0,to=-1;
	int i;
	char *reso=NULL;//resolution
	char *infile=NULL;
	char *range=NULL;
	char *org_file,*rec_file;
	int opt_cnt=0;
	int ret;
    if(argc<2) {
        usage();
        exit(-1);
    }
	for(i=1;i<argc;){
		if(!strcmp(argv[i],"-s")){
			reso=argv[++i];
			opt_cnt++;
			continue;
		}
		else if(!strcmp(argv[i],"-i")){
			infile=argv[++i];
			opt_cnt++;
			continue;
		}
		else if(!strcmp(argv[i],"-r")){
			range=argv[++i];
			opt_cnt++;
			continue;
		}
		else if(!strcmp(argv[i],"-n")){
			show_frame_number=1;
			opt_cnt++;
		}
		else if(!strcmp(argv[i],"-c")){
			calc_average=1;
			opt_cnt++;
		}
		i++;
	}
	if(opt_cnt==0){
		width=atoi(argv[1]);
		height=atoi(argv[2]);
		org_file=argv[3];
		rec_file=argv[4];
		if(argc>5)
			from=atoi(argv[5]);
		if(argc>6)
			to=atoi(argv[6]);
	}
	else{
		if(reso==NULL){
			general_log(NULL,GENL_LOG_ERROR,"width and height are needed\n");
			return -1;
		}
		if((ret=parse_opt_int(reso,&width,&height,'x'))<0)
			return -1;
		if(infile==NULL){
			general_log(NULL,GENL_LOG_ERROR,"original and reconstructed files are needed\n");
			return -1;
		}
		if((ret=parse_opt_int(reso,&org_file,&rec_file,':'))<0)
			return -1;
		if(range!=NULL){
			if((ret=parse_opt_int(reso,&from,&to,':'))<0)
				return -1;
		}
	}
	
    if(width<=0||height<=0){
        general_log(NULL,GENL_LOG_ERROR,"width or height can't be 0 or negative\n");
		return -1;
    }
    if(NULL==(p_orig=fopen(org_file,"rb"))) {
        general_log(NULL,GENL_LOG_ERROR,"can't open the original yuv file\n");
		return -1;
    }
    if(NULL==(p_rec=fopen(rec_file,"rb"))) {
        general_log(NULL,GENL_LOG_ERROR,"can't open the reconstruct yuv file\n");
		return -1;
    }
	if(to<0){//get the frame number of input files
		int c1,c2;
		if((c1=count_frames(org_file,width,height))<0||
			(c2=count_frames(rec_file,width,height))<0)
		{
			general_log(NULL,GENL_LOG_ERROR,"could not count frames\n");
			to=1<<16;
		}
		else
			to=MAX(0,MIN(c1,c2));
	}

    calc_all(p_orig,p_rec,from,to,width,height);

    fclose(p_orig);
    fclose(p_rec);

    return 0;
}

#endif

