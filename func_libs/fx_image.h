#ifndef _FX_IMAGE_H
#define _FX_IMAGE_H
#include "fx_types.h"

static double get_psnr(double mse,int Max){
	return 10*log10(Max*Max/mse);
}

static double get_ssd(byte* pix1, int stride1,
					  byte* pix2, int stride2,
					  int w, int h)
{

    int x,y;
    double sum=0;
    int e;
    double mse;
    for(y=0;y<h;y++){
		for(x=0;x<w;x++){
			int d=pix1[x]-pix2[x];
			sum+=d*d;
		}
		pix1+=stride1;
		pix2+=stride2;
    }
	return sum;
}
static double get_mse(byte* pix1, int stride1,
					  byte* pix2, int stride2,
					  int w, int h)
{
	double ssd_sum=get_ssd(pix1,stride1,pix2,stride2,w,h);
	return ssd_sum/(w*h);
}
#endif