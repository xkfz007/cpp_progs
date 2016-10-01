/*
 * calculate psnr/ssim between original yuv and reconstructed yuv
 */
//#define _MEASURE
#ifdef _MEASURE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fx_types.h"
#include "fx_fileops.h"
#include "fx_mathops.h"
#include "fx_image.h"
#include "fx_parseopt.h"
#include "fx_log.h"

static int show_frame_number=0;
static int calc_average=0;
static int depth=8;

#define MEA ssim
#define _MEA_STR(M) #M
#define MEA_STR(M) _MEA_STR(M)
#define _MEA_FUN(M) calc_##M
#define MEA_FUNC(M) _MEA_FUN(M)

static double calc_psnr(byte* orig,byte* rec,int w,int h){
	int stride1=w;
	int stride2=w;
	double mse;
	int Max;
	mse=get_mse(orig,stride1,rec,stride2,w,h);
	Max=(1<<depth)-1;
	if(mse)
		return get_psnr(mse,Max);
	else
		return 0;
}
static double calc_ssim(byte* orig,byte* rec,int w,int h){
	int stride1=w;
	int stride2=w;
	double ssim_value;
	ssim_value=get_ssim(orig,stride1,rec,stride2,w,h);
	return ssim_value;
}
static void calc_all_measure(FILE* p_orig,FILE* p_rec,int from,int to,int width,int height,
						 double(*calc_measure)(byte*,byte*,int,int)){
    double mvalues[3],msum[3];
    int pic_num;
    int frame_sz;
    int luma_sz,chroma_sz;
    byte *orig_frm,*rec_frm;
	int ret,ret2;
    //frame_sz=height*width+height/2*width/2*2;
    luma_sz=height*width;
    chroma_sz=height/2*width/2;
	frame_sz=luma_sz+2*chroma_sz;
    if(NULL==(orig_frm=(byte*)calloc(frame_sz,sizeof(byte)))
            ||NULL==(rec_frm=(byte*)calloc(frame_sz,sizeof(byte)))) {
        fx_log(NULL,FX_LOG_ERROR,"Merrory allocation error!\n");
        exit(-2);
    }
    pic_num=from;
    msum[0]=msum[1]=msum[2]=0.0;
    if((ret=fseeko(p_orig,(uint64_t)from*frame_sz, SEEK_SET))) {
    //if((ret1=fseek(p_orig,from*frame_sz, SEEK_SET))) {
		fx_log(NULL,FX_LOG_ERROR,"Seek origin file error\n");
		exit(-2);
	}
    if((ret=fseeko(p_rec,(uint64_t)from*frame_sz, SEEK_SET))) {
    //if((ret1=fseek(p_rec,from*frame_sz, SEEK_SET))) {
		fx_log(NULL,FX_LOG_ERROR,"Seek rec file error\n");
		exit(-2);
	}
	//ret1=fread(orig_frm,sizeof(byte),frame_sz,p_orig);
	//fprintf(stderr,"error %d",errno);

	//ret2=fread(rec_frm,sizeof(byte),frame_sz,p_rec);
	//fprintf(stderr,"error %d",errno);

	
	while(pic_num<=to
		&&(ret=fread(orig_frm,sizeof(byte),frame_sz,p_orig))>0
		&&(ret2=fread(rec_frm,sizeof(byte),frame_sz,p_rec))>0
		)
	{
		if(frame_sz!=ret)
		{
			fx_log(NULL,FX_LOG_ERROR,"Read origin file error, ret=%d\n",ret);
			exit(-2);
		}
		if(frame_sz!=ret2){
			fx_log(NULL,FX_LOG_ERROR,"Read rec file error, ret=%d\n",ret);
			exit(-2);
		}

        mvalues[0]=calc_measure(orig_frm,                  rec_frm,                  width,height);
        mvalues[1]=calc_measure(orig_frm+luma_sz,          rec_frm+luma_sz,          width/2,height/2);
        mvalues[2]=calc_measure(orig_frm+luma_sz+chroma_sz,rec_frm+luma_sz+chroma_sz,width/2,height/2);
        msum[0]+=mvalues[0];
        msum[1]+=mvalues[1];
        msum[2]+=mvalues[2];
		if(show_frame_number)
			fprintf(stdout,"%4d ",pic_num);
		fprintf(stdout,"%7.4f %7.4f %7.4f\n",mvalues[0],mvalues[1],mvalues[2]);
		fflush(stdout);
		//fx_log(NULL,FX_LOG_INFO,"Frame %d is done\n",pic_num);
		pic_num++;
	}
	if(calc_average)
		printf("avg: %7.4f %7.4f %7.4f\n",msum[0]/(pic_num-from),msum[1]/(pic_num-from),msum[2]/(pic_num-from));
    free(orig_frm);
    free(rec_frm);
}

static int count_frames(FILE*fp,int width,int height){
	uint64_t len=0;
    int framebytes=height*width+height/2*width/2*2;
	if(fseeko(fp,(uint64_t)0,SEEK_END)<0)
		return -1; 
	if((len=ftello(fp))<0)
		return -1;

	return (int)(len/framebytes);

}

static static void usage(){
	printf("Program used to calculate "MEA_STR(MEA)" between two yuv files.Version 0.4\n"
		   "Usage:\n"
		   "   "MEA_STR(MEA)" <width> <height> <orig.yuv> <rec.yuv> from to\n"
		   "or\n"
           "   "MEA_STR(MEA)" -s <width>x<height> -i <orig.yuv>:<rec.yuv> -r from:to -n -c\n"
		   "   -n: show frame number\n"
		   "   -c: calculate averge psnr values\n"
		   "Examples:\n"
		   "   "MEA_STR(MEA)" 176 144 foreman_qcif.yuv foreman_qcif_rec.yuv 5 100\n"
		   "   "MEA_STR(MEA)" -s 176x144 -i foreman_qcif.yuv:foreman_qcif_rec.yuv -r 5:100\n"
		   );
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
			fx_log(NULL,FX_LOG_ERROR,"width and height are needed\n");
			return -1;
		}
		if((ret=parse_opt_int(reso,&width,&height,'x'))<0)
			return -1;
		if(infile==NULL){
			fx_log(NULL,FX_LOG_ERROR,"original and reconstructed files are needed\n");
			return -1;
		}
		if((ret=parse_opt_string(reso,&org_file,&rec_file,':'))<0)
			return -1;
		if(range!=NULL){
			if((ret=parse_opt_int(reso,&from,&to,':'))<0)
				return -1;
		}
	}
	
    if(width<=0||height<=0){
        fx_log(NULL,FX_LOG_ERROR,"width or height can't be 0 or negative\n");
		return -1;
    }
    if(NULL==(p_orig=fopen(org_file,"rb"))) {
        fx_log(NULL,FX_LOG_ERROR,"can't open the original yuv file\n");
		return -1;
    }
    if(NULL==(p_rec=fopen(rec_file,"rb"))) {
        fx_log(NULL,FX_LOG_ERROR,"can't open the reconstruct yuv file\n");
		return -1;
    }
	if(to<0){//get the frame number of input files
		int c1,c2;
		if((c1=count_frames(p_orig,width,height))<0||
			(c2=count_frames(p_rec,width,height))<0)
		{
			fx_log(NULL,FX_LOG_ERROR,"could not count frames\n");
			to=1<<16;
		}
		else
			to=MAX(0,MIN(c1,c2));
	}

    calc_all_measure(p_orig,p_rec,from,to,width,height,MEA_FUNC(MEA));

    fclose(p_orig);
    fclose(p_rec);

    return 0;
}

#endif

