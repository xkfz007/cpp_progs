#ifndef _FX_IMAGE_H
#define _FX_IMAGE_H
#include "fx_types.h"
#include "fx_rational.h"
#include "fx_macros.h"
#include "fx_mathops.h"
#include "fx_string.h"

#define LAMBDA_SHIFT 7
#define LAMBDA_SCALE (1<<LAMBDA_SHIFT)
#define QP2LAMBDA 118 ///< factor to convert from H.263 QP to lambda
#define LAMBDA_MAX (256*128-1)

#define QUALITY_SCALE LAMBDA_SCALE //FIXME maybe remove

/**
 * @brief Undefined timestamp value
 *
 * Usually reported by demuxer that work on containers that do not provide
 * either pts or dts.
 */

#define FX_NOPTS_VALUE          ((int64_t)UINT64_C(0x8000000000000000))

/**
 * Internal time base represented as integer
 */

#define FX_TIME_BASE            1000000

/**
 * Internal time base represented as fractional value
 */

#define FX_TIME_BASE_Q          (AVRational){1, FX_TIME_BASE}

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

/**
 * Compare two timestamps each in its own time base.
 *
 * @return One of the following values:
 *         - -1 if `ts_a` is before `ts_b`
 *         - 1 if `ts_a` is after `ts_b`
 *         - 0 if they represent the same position
 *
 * @warning
 * The result of the function is undefined if one of the timestamps is outside
 * the `int64_t` range when represented in the other's timebase.
 */
int fx_compare_ts(int64_t ts_a, AVRational tb_a, int64_t ts_b, AVRational tb_b)
{
    int64_t a = tb_a.num * (int64_t)tb_b.den;
    int64_t b = tb_b.num * (int64_t)tb_a.den;
    if ((ABS(ts_a)|a|ABS(ts_b)|b) <= INT_MAX)
        return (ts_a*a > ts_b*b) - (ts_a*a < ts_b*b);
    if (fx_rescale_rnd(ts_a, a, b, FX_ROUND_DOWN) < ts_b)
        return -1;
    if (fx_rescale_rnd(ts_b, b, a, FX_ROUND_DOWN) < ts_a)
        return 1;
    return 0;
}

/**
 * Compare the remainders of two integer operands divided by a common divisor.
 *
 * In other words, compare the least significant `log2(mod)` bits of integers
 * `a` and `b`.
 *
 * @code{.c}
 * fx_compare_mod(0x11, 0x02, 0x10) < 0 // since 0x11 % 0x10  (0x1) < 0x02 % 0x10  (0x2)
 * fx_compare_mod(0x11, 0x02, 0x20) > 0 // since 0x11 % 0x20 (0x11) > 0x02 % 0x20 (0x02)
 * @endcode
 *
 * @param a,b Operands
 * @param mod Divisor; must be a power of 2
 * @return
 *         - a negative value if `a % mod < b % mod`
 *         - a positive value if `a % mod > b % mod`
 *         - zero             if `a % mod == b % mod`
 */
int64_t fx_compare_mod(uint64_t a, uint64_t b, uint64_t mod)
{
    int64_t c = (a - b) & (mod - 1);
    if (c > (mod >> 1))
        c -= mod;
    return c;
}

/**
 * Rescale a timestamp while preserving known durations.
 *
 * This function is designed to be called per audio packet to scale the input
 * timestamp to a different time base. Compared to a simple fx_rescale_q()
 * call, this function is robust against possible inconsistent frame durations.
 *
 * The `last` parameter is a state variable that must be preserved for all
 * subsequent calls for the same stream. For the first call, `*last` should be
 * initialized to #fx_NOPTS_VALUE.
 *
 * @param[in]     in_tb    Input time base
 * @param[in]     in_ts    Input timestamp
 * @param[in]     fs_tb    Duration time base; typically this is finer-grained
 *                         (greater) than `in_tb` and `out_tb`
 * @param[in]     duration Duration till the next call to this function (i.e.
 *                         duration of the current packet/frame)
 * @param[in,out] last     Pointer to a timestamp expressed in terms of
 *                         `fs_tb`, acting as a state variable
 * @param[in]     out_tb   Output timebase
 * @return        Timestamp expressed in terms of `out_tb`
 *
 * @note In the context of this function, "duration" is in term of samples, not
 *       seconds.
 */
int64_t fx_rescale_delta(AVRational in_tb, int64_t in_ts,  AVRational fs_tb, int duration, int64_t *last, AVRational out_tb){
    int64_t a, b, this;

    ASSERT(in_ts != FX_NOPTS_VALUE);
    ASSERT(duration >= 0);

    if (*last == FX_NOPTS_VALUE || !duration || in_tb.num*(int64_t)out_tb.den <= out_tb.num*(int64_t)in_tb.den) {
simple_round:
        *last = fx_rescale_q(in_ts, in_tb, fs_tb) + duration;
        return fx_rescale_q(in_ts, in_tb, out_tb);
    }

    a =  fx_rescale_q_rnd(2*in_ts-1, in_tb, fs_tb, FX_ROUND_DOWN)   >>1;
    b = (fx_rescale_q_rnd(2*in_ts+1, in_tb, fs_tb, FX_ROUND_UP  )+1)>>1;
    if (*last < 2*a - b || *last > 2*b - a)
        goto simple_round;

    this = fx_clip64(*last, a, b);
    *last = this + duration;

    return fx_rescale_q(this, fs_tb, out_tb);
}

/**
 * Add a value to a timestamp.
 *
 * This function guarantees that when the same value is repeatly added that
 * no accumulation of rounding errors occurs.
 *
 * @param[in] ts     Input timestamp
 * @param[in] ts_tb  Input timestamp time base
 * @param[in] inc    Value to be added
 * @param[in] inc_tb Time base of `inc`
 */
int64_t fx_add_stable(AVRational ts_tb, int64_t ts, AVRational inc_tb, int64_t inc)
{
    int64_t m, d;

    if (inc != 1)
        inc_tb = fx_mul_q(inc_tb, (AVRational) {inc, 1});

    m = inc_tb.num * (int64_t)ts_tb.den;
    d = inc_tb.den * (int64_t)ts_tb.num;

    if (m % d == 0)
        return ts + m / d;
    if (m < d)
        return ts;

    {
        int64_t old = fx_rescale_q(ts, ts_tb, inc_tb);
        int64_t old_ts = fx_rescale_q(old, inc_tb, ts_tb);
        return fx_rescale_q(old + 1, inc_tb, ts_tb) + (ts - old_ts);
    }
}



/**
 * Pixel format.
 *
 * @note
 * FX_PIX_FMT_RGB32 is handled in an endian-specific manner. An RGBA
 * color is put together as:
 *  (A << 24) | (R << 16) | (G << 8) | B
 * This is stored as BGRA on little-endian CPU architectures and ARGB on
 * big-endian CPUs.
 *
 * @par
 * When the pixel format is palettized RGB32 (FX_PIX_FMT_PAL8), the palettized
 * image data is stored in AVFrame.data[0]. The palette is transported in
 * AVFrame.data[1], is 1024 bytes long (256 4-byte entries) and is
 * formatted the same as in FX_PIX_FMT_RGB32 described above (i.e., it is
 * also endian-specific). Note also that the individual RGB32 palette
 * components stored in AVFrame.data[1] should be in the range 0..255.
 * This is important as many custom PAL8 video codecs that were designed
 * to run on the IBM VGA graphics adapter use 6-bit palette components.
 *
 * @par
 * For all the 8 bits per pixel formats, an RGB32 palette is in data[1] like
 * for pal8. This palette is filled in automatically by the function
 * allocating the picture.
 */
enum AVPixelFormat {
    FX_PIX_FMT_NONE = -1,
    FX_PIX_FMT_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    FX_PIX_FMT_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    FX_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    FX_PIX_FMT_BGR24,     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
    FX_PIX_FMT_YUV422P,   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    FX_PIX_FMT_YUV444P,   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    FX_PIX_FMT_YUV410P,   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    FX_PIX_FMT_YUV411P,   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    FX_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
    FX_PIX_FMT_MONOWHITE, ///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
    FX_PIX_FMT_MONOBLACK, ///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
    FX_PIX_FMT_PAL8,      ///< 8 bits with FX_PIX_FMT_RGB32 palette
    FX_PIX_FMT_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of FX_PIX_FMT_YUV420P and setting color_range
    FX_PIX_FMT_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of FX_PIX_FMT_YUV422P and setting color_range
    FX_PIX_FMT_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of FX_PIX_FMT_YUV444P and setting color_range
#if FF_API_XVMC
    FX_PIX_FMT_XVMC_MPEG2_MC,///< XVideo Motion Acceleration via common packet passing
    FX_PIX_FMT_XVMC_MPEG2_IDCT,
#define FX_PIX_FMT_XVMC FX_PIX_FMT_XVMC_MPEG2_IDCT
#endif /* FF_API_XVMC */
    FX_PIX_FMT_UYVY422,   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    FX_PIX_FMT_UYYVYY411, ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    FX_PIX_FMT_BGR8,      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
    FX_PIX_FMT_BGR4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    FX_PIX_FMT_BGR4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
    FX_PIX_FMT_RGB8,      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
    FX_PIX_FMT_RGB4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    FX_PIX_FMT_RGB4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
    FX_PIX_FMT_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    FX_PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped

    FX_PIX_FMT_ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    FX_PIX_FMT_RGBA,      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    FX_PIX_FMT_ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    FX_PIX_FMT_BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

    FX_PIX_FMT_GRAY16BE,  ///<        Y        , 16bpp, big-endian
    FX_PIX_FMT_GRAY16LE,  ///<        Y        , 16bpp, little-endian
    FX_PIX_FMT_YUV440P,   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
    FX_PIX_FMT_YUVJ440P,  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of FX_PIX_FMT_YUV440P and setting color_range
    FX_PIX_FMT_YUVA420P,  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
#if FF_API_VDPAU
    FX_PIX_FMT_VDPAU_H264,///< H.264 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
    FX_PIX_FMT_VDPAU_MPEG1,///< MPEG-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
    FX_PIX_FMT_VDPAU_MPEG2,///< MPEG-2 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
    FX_PIX_FMT_VDPAU_WMV3,///< WMV3 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
    FX_PIX_FMT_VDPAU_VC1, ///< VC-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
#endif
    FX_PIX_FMT_RGB48BE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
    FX_PIX_FMT_RGB48LE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

    FX_PIX_FMT_RGB565BE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
    FX_PIX_FMT_RGB565LE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
    FX_PIX_FMT_RGB555BE,  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
    FX_PIX_FMT_RGB555LE,  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined

    FX_PIX_FMT_BGR565BE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
    FX_PIX_FMT_BGR565LE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
    FX_PIX_FMT_BGR555BE,  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
    FX_PIX_FMT_BGR555LE,  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined

#if FF_API_VAAPI
    /** @name Deprecated pixel formats */
    /**@{*/
    FX_PIX_FMT_VAAPI_MOCO, ///< HW acceleration through VA API at motion compensation entry-point, Picture.data[3] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers
    FX_PIX_FMT_VAAPI_IDCT, ///< HW acceleration through VA API at IDCT entry-point, Picture.data[3] contains a vaapi_render_state struct which contains fields extracted from headers
    FX_PIX_FMT_VAAPI_VLD,  ///< HW decoding through VA API, Picture.data[3] contains a VASurfaceID
    /**@}*/
    FX_PIX_FMT_VAAPI = FX_PIX_FMT_VAAPI_VLD,
#else
    /**
     *  Hardware acceleration through VA-API, data[3] contains a
     *  VASurfaceID.
     */
    FX_PIX_FMT_VAAPI,
#endif

    FX_PIX_FMT_YUV420P16LE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    FX_PIX_FMT_YUV420P16BE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    FX_PIX_FMT_YUV422P16LE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    FX_PIX_FMT_YUV422P16BE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    FX_PIX_FMT_YUV444P16LE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    FX_PIX_FMT_YUV444P16BE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
#if FF_API_VDPAU
    FX_PIX_FMT_VDPAU_MPEG4,  ///< MPEG-4 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
#endif
    FX_PIX_FMT_DXVA2_VLD,    ///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

    FX_PIX_FMT_RGB444LE,  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
    FX_PIX_FMT_RGB444BE,  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
    FX_PIX_FMT_BGR444LE,  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
    FX_PIX_FMT_BGR444BE,  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
    FX_PIX_FMT_YA8,       ///< 8 bits gray, 8 bits alpha

    FX_PIX_FMT_Y400A = FX_PIX_FMT_YA8, ///< alias for FX_PIX_FMT_YA8
    FX_PIX_FMT_GRAY8A= FX_PIX_FMT_YA8, ///< alias for FX_PIX_FMT_YA8

    FX_PIX_FMT_BGR48BE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
    FX_PIX_FMT_BGR48LE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

    /**
     * The following 12 formats have the disadvantage of needing 1 format for each bit depth.
     * Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
     * If you want to support multiple bit depths, then using FX_PIX_FMT_YUV420P16* with the bpp stored separately is better.
     */
    FX_PIX_FMT_YUV420P9BE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    FX_PIX_FMT_YUV420P9LE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    FX_PIX_FMT_YUV420P10BE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    FX_PIX_FMT_YUV420P10LE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    FX_PIX_FMT_YUV422P10BE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    FX_PIX_FMT_YUV422P10LE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    FX_PIX_FMT_YUV444P9BE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    FX_PIX_FMT_YUV444P9LE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    FX_PIX_FMT_YUV444P10BE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    FX_PIX_FMT_YUV444P10LE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    FX_PIX_FMT_YUV422P9BE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    FX_PIX_FMT_YUV422P9LE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    FX_PIX_FMT_VDA_VLD,    ///< hardware decoding through VDA
    FX_PIX_FMT_GBRP,      ///< planar GBR 4:4:4 24bpp
    FX_PIX_FMT_GBRP9BE,   ///< planar GBR 4:4:4 27bpp, big-endian
    FX_PIX_FMT_GBRP9LE,   ///< planar GBR 4:4:4 27bpp, little-endian
    FX_PIX_FMT_GBRP10BE,  ///< planar GBR 4:4:4 30bpp, big-endian
    FX_PIX_FMT_GBRP10LE,  ///< planar GBR 4:4:4 30bpp, little-endian
    FX_PIX_FMT_GBRP16BE,  ///< planar GBR 4:4:4 48bpp, big-endian
    FX_PIX_FMT_GBRP16LE,  ///< planar GBR 4:4:4 48bpp, little-endian
    FX_PIX_FMT_YUVA422P,  ///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    FX_PIX_FMT_YUVA444P,  ///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
    FX_PIX_FMT_YUVA420P9BE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
    FX_PIX_FMT_YUVA420P9LE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
    FX_PIX_FMT_YUVA422P9BE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
    FX_PIX_FMT_YUVA422P9LE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
    FX_PIX_FMT_YUVA444P9BE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
    FX_PIX_FMT_YUVA444P9LE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    FX_PIX_FMT_YUVA420P10BE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    FX_PIX_FMT_YUVA420P10LE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    FX_PIX_FMT_YUVA422P10BE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    FX_PIX_FMT_YUVA422P10LE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    FX_PIX_FMT_YUVA444P10BE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    FX_PIX_FMT_YUVA444P10LE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
    FX_PIX_FMT_YUVA420P16BE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    FX_PIX_FMT_YUVA420P16LE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    FX_PIX_FMT_YUVA422P16BE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    FX_PIX_FMT_YUVA422P16LE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    FX_PIX_FMT_YUVA444P16BE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    FX_PIX_FMT_YUVA444P16LE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

    FX_PIX_FMT_VDPAU,     ///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

    FX_PIX_FMT_XYZ12LE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
    FX_PIX_FMT_XYZ12BE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
    FX_PIX_FMT_NV16,         ///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    FX_PIX_FMT_NV20LE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    FX_PIX_FMT_NV20BE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

    FX_PIX_FMT_RGBA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    FX_PIX_FMT_RGBA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    FX_PIX_FMT_BGRA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    FX_PIX_FMT_BGRA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

    FX_PIX_FMT_YVYU422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

    FX_PIX_FMT_VDA,          ///< HW acceleration through VDA, data[3] contains a CVPixelBufferRef

    FX_PIX_FMT_YA16BE,       ///< 16 bits gray, 16 bits alpha (big-endian)
    FX_PIX_FMT_YA16LE,       ///< 16 bits gray, 16 bits alpha (little-endian)

    FX_PIX_FMT_GBRAP,        ///< planar GBRA 4:4:4:4 32bpp
    FX_PIX_FMT_GBRAP16BE,    ///< planar GBRA 4:4:4:4 64bpp, big-endian
    FX_PIX_FMT_GBRAP16LE,    ///< planar GBRA 4:4:4:4 64bpp, little-endian
    /**
     *  HW acceleration through QSV, data[3] contains a pointer to the
     *  mfxFrameSurface1 structure.
     */
    FX_PIX_FMT_QSV,
    /**
     * HW acceleration though MMAL, data[3] contains a pointer to the
     * MMAL_BUFFER_HEADER_T structure.
     */
    FX_PIX_FMT_MMAL,

    FX_PIX_FMT_D3D11VA_VLD,  ///< HW decoding through Direct3D11, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer

    /**
     * HW acceleration through CUDA. data[i] contain CUdeviceptr pointers
     * exactly as for system memory frames.
     */
    FX_PIX_FMT_CUDA,

    FX_PIX_FMT_0RGB=0x123+4,///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
    FX_PIX_FMT_RGB0,        ///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
    FX_PIX_FMT_0BGR,        ///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
    FX_PIX_FMT_BGR0,        ///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

    FX_PIX_FMT_YUV420P12BE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    FX_PIX_FMT_YUV420P12LE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    FX_PIX_FMT_YUV420P14BE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    FX_PIX_FMT_YUV420P14LE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    FX_PIX_FMT_YUV422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    FX_PIX_FMT_YUV422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    FX_PIX_FMT_YUV422P14BE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    FX_PIX_FMT_YUV422P14LE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    FX_PIX_FMT_YUV444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    FX_PIX_FMT_YUV444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    FX_PIX_FMT_YUV444P14BE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    FX_PIX_FMT_YUV444P14LE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    FX_PIX_FMT_GBRP12BE,    ///< planar GBR 4:4:4 36bpp, big-endian
    FX_PIX_FMT_GBRP12LE,    ///< planar GBR 4:4:4 36bpp, little-endian
    FX_PIX_FMT_GBRP14BE,    ///< planar GBR 4:4:4 42bpp, big-endian
    FX_PIX_FMT_GBRP14LE,    ///< planar GBR 4:4:4 42bpp, little-endian
    FX_PIX_FMT_YUVJ411P,    ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of FX_PIX_FMT_YUV411P and setting color_range

    FX_PIX_FMT_BAYER_BGGR8,    ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
    FX_PIX_FMT_BAYER_RGGB8,    ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
    FX_PIX_FMT_BAYER_GBRG8,    ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
    FX_PIX_FMT_BAYER_GRBG8,    ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */
    FX_PIX_FMT_BAYER_BGGR16LE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian */
    FX_PIX_FMT_BAYER_BGGR16BE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian */
    FX_PIX_FMT_BAYER_RGGB16LE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian */
    FX_PIX_FMT_BAYER_RGGB16BE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian */
    FX_PIX_FMT_BAYER_GBRG16LE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian */
    FX_PIX_FMT_BAYER_GBRG16BE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian */
    FX_PIX_FMT_BAYER_GRBG16LE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian */
    FX_PIX_FMT_BAYER_GRBG16BE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian */
#if !FF_API_XVMC
    FX_PIX_FMT_XVMC,///< XVideo Motion Acceleration via common packet passing
#endif /* !FF_API_XVMC */
    FX_PIX_FMT_YUV440P10LE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    FX_PIX_FMT_YUV440P10BE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    FX_PIX_FMT_YUV440P12LE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    FX_PIX_FMT_YUV440P12BE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    FX_PIX_FMT_AYUV64LE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    FX_PIX_FMT_AYUV64BE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian

    FX_PIX_FMT_VIDEOTOOLBOX, ///< hardware decoding through Videotoolbox

    FX_PIX_FMT_P010LE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
    FX_PIX_FMT_P010BE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian

    FX_PIX_FMT_GBRAP12BE,  ///< planar GBR 4:4:4:4 48bpp, big-endian
    FX_PIX_FMT_GBRAP12LE,  ///< planar GBR 4:4:4:4 48bpp, little-endian

    FX_PIX_FMT_GBRAP10BE,  ///< planar GBR 4:4:4:4 40bpp, big-endian
    FX_PIX_FMT_GBRAP10LE,  ///< planar GBR 4:4:4:4 40bpp, little-endian

    FX_PIX_FMT_MEDIACODEC, ///< hardware decoding through MediaCodec

    FX_PIX_FMT_NB,        ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
};

#endif