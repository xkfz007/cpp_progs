#ifndef _FX_MATHOPS_H
#define _FX_MATHOPS_H

//decide if x is power of 2
#define FX_ISPOWER2(x) ((x > 0) && (0 == (x & (x - 1))))
/*
 * get min of two numbers using bitwise ops
 * y^((x^y)&(-(x<y)))
 * x<y:  y^((x^y)&(-(x<y))) = y^((x^y)&(-1)) = y^((x^y)&0xffffffff) = y^(x^y) = (y^y)^x = 0^x = x
 * x>=y: y^((x^y)&(-(x<y))) = y^((x^y)&(-0)) = y^((x^y)&0) = y^0 = y
 */
#define FX_MIN_BW(x,y) (y^((x^y)&(-(x<y))))
#define FX_SWAP_BW(x,y) ((x)==(y)?NULL:((x)^=(y),(y)^=(x),(x)^=(y)))

#define FX_MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define FX_MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define FX_MIN3(a,b,c) FX_MIN((a),FX_MIN((b),(c)))
#define FX_MAX3(a,b,c) FX_MAX((a),FX_MAX((b),(c)))
#define FX_MIN4(a,b,c,d) FX_MIN((a),FX_MIN3((b),(c),(d)))
#define FX_MAX4(a,b,c,d) FX_MAX((a),FX_MAX3((b),(c),(d)))

#define FX_XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)
#define FX_FIX8(f) ((int)(f*(1<<8)+.5))
#define FX_ALIGN(x,a) (((x)+((a)-1))&~((a)-1))
#define FX_ARRAY_ELEMS(a) ((sizeof(a))/(sizeof(a[0])))

#if !HAVE_LOG2F
#define LOG2F(x) (logf(x)/0.693147180559945f)
#define LOG2(x) (log(x)/0.693147180559945)
#endif

#endif