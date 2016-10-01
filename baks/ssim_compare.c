/*
this program is used to compare the SSIM calculation codes of JM and that of x264
*/
#define _SSIM_COMPARE
#ifdef _SSIM_COMPARE
#include <stdio.h>
#include <stdlib.h>
#include "fx_types.h"

static void usage(){
	printf("Program used to calculate ssim between two yuv files.Version 0.4\n"
		   "Usage:\n"
		   "\tssim <width> <height> <orig.yuv> <rec.yuv> from to\n"
		   "or\n"
           "\tssim -s <width>x<height> -i <orig.yuv>:<rec.yuv> -r from:to -n -c\n"
		   "\t-n: show frame number\n"
		   "\t-c: calculate averge psnr values\n"
		   "Examples:\n"
		   "\tssim 176 144 foreman_qcif.yuv foreman_qcif_rec.yuv 5 100\n"
		   "\tssim -s 176x144 -i foreman_qcif.yuv:foreman_qcif_rec.yuv -r 5:100\n"
		   );
}

int main(){
	FILE* fp_i,*fp_r;
	//fp_i=fopen("e:/sequences/BlowingBubbles_416x240_50.yuv","rb");
	fp_i=fopen("e:/sequences/Mobisode2_416x240_30.yuv","rb");
	//fp_r=fopen("BlowingBubbles_416x240_50_rec.yuv","rb");
	fp_r=fopen("F:/encoder_test_output/x265_output/rec.yuv","rb");
	int frame_to_be_encoded=100;
	const int width=416,height=240;
	/*
	pixel* py_i=(pixel*)malloc(sizeof(pixel)*width*height);
	pixel* pu_i=(pixel*)malloc(sizeof(pixel)*width*height/4);
	pixel* pv_i=(pixel*)malloc(sizeof(pixel)*width*height/4);
	pixel* py_r=(pixel*)malloc(sizeof(pixel)*width*height);
	pixel* pu_r=(pixel*)malloc(sizeof(pixel)*width*height/4);
	pixel* pv_r=(pixel*)malloc(sizeof(pixel)*width*height/4);
	*/
	pixel py_i[width*height];
	pixel pu_i[width/2*height/2];
	pixel pv_i[width/2*height/2];
	pixel py_r[width*height];
	pixel pu_r[width/2*height/2];
	pixel pv_r[width/2*height/2];
	int m_ssimBuf[8*(width/4+3)];
	int dump;
	for(int i=0;i<frame_to_be_encoded;i++)
	{
		fread(py_i,sizeof(pixel),width*height,fp_i);
		fread(pu_i,sizeof(pixel),width*height/4,fp_i);
		fread(pv_i,sizeof(pixel),width*height/4,fp_i);

		fread(py_r,sizeof(pixel),width*height,fp_r);
		fread(pu_r,sizeof(pixel),width*height/4,fp_r);
		fread(pv_r,sizeof(pixel),width*height/4,fp_r);

		double ssimy1=get_ssim(py_r,width,py_i,width,width,height);
		double ssimu=get_ssim(pu_r,width/2,pu_i,width/2,width/2,height/2);
		double ssimv=get_ssim(pv_r,width/2,pv_i,width/2,width/2,height/2);
		//double ssimy2=calculateSSIM(py_r,width,py_i,width,width,height,m_ssimBuf,&dump);
		//ssimy2/=dump;
		//printf("SSIMY=%.4f %.4f \n",ssimy1,ssimy2);
		//printf("SSIMY=%.4f ",ssimy1);
		//printf("SSIMU=%.4f ",ssimu);
		//printf("SSIMV=%.4f \n",ssimv);
		printf(" [Y %.4lf U %.4lf V %.4lf ]\n", ssimy1, ssimu, ssimv);

	}
	/*
	free(py_i);
	free(pu_i);
	free(pv_i);
	free(py_r);
	free(pu_r);
	free(pv_r);
	*/
	fclose(fp_i);
	fclose(fp_r);

}
#endif