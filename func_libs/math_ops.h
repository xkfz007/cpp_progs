#ifndef _MATH_OPS_H
#define _MATH_OPS_H

//�ж�һ�����ǲ���2��n�η�
#define ISPOWER2(x) ((x > 0) && (0 == (x & (x - 1))))
/*
 * ���ð�λ�������õ�x��y�еĽ�С������y^((x^y)&(-(x<y)))
 * ��x<yʱ��y^((x^y)&(-(x<y))) = y^((x^y)&(-1)) = y^((x^y)&0xffffffff) = y^(x^y) = (y^y)^x = 0^x = x
 * ��x>=yʱ��y^((x^y)&(-(x<y))) = y^((x^y)&(-0)) = y^((x^y)&0) = y^0 = y
 */
#define MIN(x,y) (y^((x^y)&(-(x<y))))

//�������������
#define max(a,b,c) ((a)>(b)?((a)>(c)?(a):(c)):((b)>(c)?(b):(c)))
#define MAX(a,b,c) ((a)>((b)>(c)?(b):(c))?(a):((b)>(c)?(b):(c)))
//��ʵ��SWAP����
#define SWAP(x,y) ((x)==(y)?NULL:((x)^=(y),(y)^=(x),(x)^=(y)))
#define SWAP_(x,y) (temp=x,x=y,y=temp)//��ʹ��ǰ����һ����x��yͬ���͵�tmp��ʱ����

/****************************************************************************
 * Macros
 ****************************************************************************/
#define X264_MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define X264_MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define X264_MIN3(a,b,c) X264_MIN((a),X264_MIN((b),(c)))
#define X264_MAX3(a,b,c) X264_MAX((a),X264_MAX((b),(c)))
#define X264_MIN4(a,b,c,d) X264_MIN((a),X264_MIN3((b),(c),(d)))
#define X264_MAX4(a,b,c,d) X264_MAX((a),X264_MAX3((b),(c),(d)))
#define XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)
#define FIX8(f) ((int)(f*(1<<8)+.5))
#define ALIGN(x,a) (((x)+((a)-1))&~((a)-1))
#define ARRAY_ELEMS(a) ((sizeof(a))/(sizeof(a[0])))

#endif