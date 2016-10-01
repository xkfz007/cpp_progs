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


#define BIT_DEPTH 8
//original SSIM calculation
static double get_ssim(pixel* pic_r, int stride_r,
					   pixel* pic_i, int stride_i,
					   int width,int height) 
{

	static const double K1 = 0.01f, K2 = 0.03f;
	static const int32_t win_width=8,win_height=8;
	double max_pix_value_sqd;
	double C1, C2;

	double win_pixels = (double) (win_width * win_height);
	double win_pixels_bias = win_pixels;

	double mb_ssim, mean_i, mean_r;
	double var_i, var_r, cov_ir;
	int32_t imean_i, imean_r, ivar_i, ivar_r, icov_ir;
	double cur_distortion = 0.0;
	int32_t i, j, n, m, win_cnt = 0;
	int32_t overlapSize = 8;
	pixel* p_i,*p_r;

	max_pix_value_sqd = ((1<<BIT_DEPTH)-1)*((1<<BIT_DEPTH)-1);
	C1 = K1 * K1 * max_pix_value_sqd;
	C2 = K2 * K2 * max_pix_value_sqd;

	for(j = 0; j <= height-win_height; j+=overlapSize) {
		for(i = 0; i <= width-win_width; i+=overlapSize) {

			imean_i = 0;
			imean_r = 0; 
			ivar_i  = 0;
			ivar_r  = 0;
			icov_ir = 0;

			p_i=pic_i+i;
			p_r=pic_r+i;

			for ( n = 0;n < 0 + win_height;n ++)
			{
				for (m = 0;m < 0 + win_width;m ++)
				{
					pixel px_i=*(p_i+m);
					pixel px_r=*(p_r+m);
					imean_i   += px_i;
					imean_r   += px_r;
					ivar_i    += px_i*px_i;
					ivar_r    += px_r*px_r;
					icov_ir += px_i * px_r;
				}
				p_i+=stride_i;
				p_r+=stride_r;
			}

			mean_i = (double) imean_i / win_pixels;
			mean_r = (double) imean_r / win_pixels;

			var_i    = ((double) ivar_i - ((double) imean_i) * mean_i) / win_pixels_bias;
			var_r    = ((double) ivar_r - ((double) imean_r) * mean_r) / win_pixels_bias;
			cov_ir = ((double) icov_ir - ((double) imean_i) * mean_r) / win_pixels_bias;

			mb_ssim  = (double) ((2.0 * mean_i * mean_r + C1) * (2.0 * cov_ir + C2));
			mb_ssim /= (double) (mean_i * mean_i + mean_r * mean_r + C1) * (var_i + var_r + C2);

			cur_distortion += mb_ssim;
			win_cnt++;

		}
		pic_i += overlapSize*stride_i;
		pic_r += overlapSize*stride_r;
	}
	cur_distortion /= (double) win_cnt;

	if (cur_distortion >= 1.0 && cur_distortion < 1.01) // avoid double accuracy problem at very low QP(e.g.2)
		cur_distortion = 1.0;

	return cur_distortion;

}	
/* structural similarity metric */
void ssim_4x4x2_core(const pixel *pix1, intptr_t stride1, const pixel *pix2, intptr_t stride2, int sums[2][4])
{
	int x,y,z;
	for (z = 0; z < 2; z++)
	{
		uint32_t s1 = 0, s2 = 0, ss = 0, s12 = 0;
		for (y = 0; y < 4; y++)
		{
			for (x = 0; x < 4; x++)
			{
				int a = pix1[x + y * stride1];
				int b = pix2[x + y * stride2];
				s1 += a;
				s2 += b;
				ss += a * a;
				ss += b * b;
				s12 += a * b;
			}
		}

		sums[z][0] = s1;
		sums[z][1] = s2;
		sums[z][2] = ss;
		sums[z][3] = s12;
		pix1 += 4;
		pix2 += 4;
	}
}

static float ssim_end_1(int s1, int s2, int ss, int s12)
{
	/* Maximum value for 10-bit is: ss*64 = (2^10-1)^2*16*4*64 = 4286582784, which will overflow in some cases.
	* s1*s1, s2*s2, and s1*s2 also obtain this value for edge cases: ((2^10-1)*16*4)^2 = 4286582784.
	* Maximum value for 9-bit is: ss*64 = (2^9-1)^2*16*4*64 = 1069551616, which will not overflow. */

#define PIXEL_MAX ((1 << BIT_DEPTH) - 1)

#define type int
	static const int ssim_c1 = (int)(.01 * .01 * PIXEL_MAX * PIXEL_MAX * 64 + .5);
	static const int ssim_c2 = (int)(.03 * .03 * PIXEL_MAX * PIXEL_MAX * 64 * 63 + .5);
	type fs1 = (type)s1;
	type fs2 = (type)s2;
	type fss = (type)ss;
	type fs12 = (type)s12;
	type vars = (type)(fss * 64 - fs1 * fs1 - fs2 * fs2);
	type covar = (type)(fs12 * 64 - fs1 * fs2);
	return (float)(2 * fs1 * fs2 + ssim_c1) * (float)(2 * covar + ssim_c2)
		/ ((float)(fs1 * fs1 + fs2 * fs2 + ssim_c1) * (float)(vars + ssim_c2));
#undef type
#undef PIXEL_MAX
}

static float ssim_end_4(int sum0[5][4], int sum1[5][4], int width)
{
	float ssim = 0.0;
	int i;

	for (i = 0; i < width; i++)
	{
		ssim += ssim_end_1(sum0[i][0] + sum0[i + 1][0] + sum1[i][0] + sum1[i + 1][0],
			sum0[i][1] + sum0[i + 1][1] + sum1[i][1] + sum1[i + 1][1],
			sum0[i][2] + sum0[i + 1][2] + sum1[i][2] + sum1[i + 1][2],
			sum0[i][3] + sum0[i + 1][3] + sum1[i][3] + sum1[i + 1][3]);
	}

	return ssim;
}
/* Function to calculate SSIM for each row */
static float get_ssim_x264(pixel *pix1, intptr_t stride1, pixel *pix2, intptr_t stride2, int width, int height, void *buf, int32_t* cnt)
{
	int x,y,z = 0;
	float ssim = 0.0;

	int(*sum0)[4] = (int(*)[4])buf;
	int(*sum1)[4] = sum0 + (width >> 2) + 3;
	width >>= 2;
	height >>= 2;
	//	printf("width=%d height=%d ....\n",width,height);

	for (y = 1; y < height; y++)
	{
		for (; z <= y; z++)
		{
			void* swap = sum0;
			sum0 = sum1;
			sum1 = (int(*)[4])swap;
			for (x = 0; x < width; x += 2)
			{
				ssim_4x4x2_core(&pix1[(4 * x + (z * stride1))], stride1, &pix2[(4 * x + (z * stride2))], stride2, &sum0[x]);
			}
		}

		for (x = 0; x < width - 1; x += 4)
		{
			ssim += ssim_end_4(sum0 + x, sum1 + x, MIN(4, width - x - 1));
		}
	}

	*cnt = (height - 1) * (width - 1);
	return ssim;
}


static double qp2qscale(int qp)
{
        return 0.85 * pow(2.0, ( qp - 12.0 ) / 6.0);
}

static double qscale2qp(double qscale)
{
        return 12.0 + 6.0 * log(qscale/0.85) / log(2.0) ;
}
#endif