/*
 * struct_write.cc
 *
 *  Created on: 2016-5月-23 上午1:30:34
 *      Author: Felix
 */

/*
 * C语言中将结构体写入文件
 可以使用fwrite()将一个结构体写入文件：
  fwrite(&some_struct,sizeof somestruct,1,fp);
对应的fread函数可以再把它读出来，此处fwrite受到一个结构的指针并把这个结构的内存映像作为字节流写入文件。sizeof操作符计算出结构占用的字节数。
但 是这样用内存映像写出的数据文件却是不能够移植的，尤其是当结构中包含浮点成员或指针的时候。结构的内存布局跟机器和编译器都有关。不同的编译器可能使用 不同数量的填充位，不同机器上基本类型的大小和字节顺序也不尽相同。因此，作为内存映像写出的结构在别的机器上（甚至是被别的编译器编译之后）不一定能被 读回来。
  同时注意如果结构包含任何指针（char*字符串或指向其他数据结构的指针），则只有指针值会被写入文件。当它们再次被读回来的时候可能已经失效。最后为了广泛的可移植性，你必需用“b”标志打开文件。
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char c;
    int h;
    short n;
    long m;
    float f;
    double d1;
    char *s;
    double d2;
}st;
int struct_write(void)
{
    FILE *fp;
    st sa,sb;
    char *str="abcdefg";
    sa.c='K';
    sa.h=-3;
    sa.n=20;
    sa.m=100000000;
    sa.f=33.32f;
    sa.d1=78.572;
    sa.d2=33.637;
    sa.s=str;
    fp=fopen("st.txt","w+");
    if(!fp)
    {
        printf("errror!\n");
        exit(-1);
    }
    printf("sa:c=%c,h=%d,n=%d,m=%d,f=%f,d1=%f,s=%s,d2=%f\n",sa.c,sa.h,sa.n,sa.m,sa.f,sa.d1,sa.s,sa.d2);
    printf("sizeof(sa)=%d:&c=%x,&h=%x,&n=%x,&m=%x,&f=%x,&d1=%x,&s=%x,&d2=%x\n",sizeof(sa),&sa.c,&sa.h,&sa.n,&sa.m,&sa.f,&sa.d1,&sa.s,&sa.d2);
    fwrite(&sa,sizeof(sa),1,fp);
    rewind(fp);
    fread(&sb,sizeof(sb),1,fp);
    printf("sb:c=%c,h=%d,n=%d,m=%d,f=%f,d1=%f,s=%s,d2=%f\n",sb.c,sb.h,sb.n,sb.m,sb.f,sb.d1,sb.s,sb.d2);

    fclose(fp);

    return 0;
}
//从文件中读出结构体：

int struct_read(void)
{
    FILE *fp;
    st sb;
    fp=fopen("st.txt","r");
    if(!fp)
    {
        printf("errror!\n");
        exit(-1);
    }
    fread(&sb,sizeof(sb),1,fp);
    printf("sb:c=%c,h=%d,n=%d,m=%d,f=%f,d1=%f,s=%s,d2=%f\n",sb.c,sb.h,sb.n,sb.m,sb.f,sb.d1,sb.s,sb.d2);
    printf("sizeof(sb)=%d:&c=%x,&h=%x,&n=%x,&m=%x,&f=%x,&d1=%x,&s=%x,&d2=%x\n",sizeof(sb),&sb.c,&sb.h,&sb.n,&sb.m,&sb.f,&sb.d1,&sb.s,&sb.d2);

    fclose(fp);

    return 0;
}
//在linux平台下的GCC编译器进行编译后的结果如下：
//首先是结构体写入文件：
//sa:c=K,h=-3,n=20,m=100000000,f=33.320000,d1=78.572000,s=abcdefg,d2=33.637000
//sizeof(sa)=40:&c=bfb98a10,&h=bfb98a14,&n=bfb98a18,&m=bfb98a1c,&f=bfb98a20,&d1=bfb98a24,&s=bfb98a2c,&d2=bfb98a30
//sb:c=K,h=-3,n=20,m=100000000,f=33.320000,d1=78.572000,s=abcdefg,d2=33.637000
//从文件中读出结构体：
//sb:c=K,h=-3,n=20,m=100000000,f=33.320000,d1=78.572000,s=���o��,d2=33.637000
//sizeof(sb)=40:&c=bfbc9964,&h=bfbc9968,&n=bfbc996c,&m=bfbc9970,&f=bfbc9974,&d1=bfbc9978,&s=bfbc9980,&d2=bfbc9984
//
//在windows xp 平台下利用Visual C++编译器编译后结果如下：
//写入结构体：
//sa:c=K,h=-3,n=20,m=100000000,f=33.320000,d1=78.572000,s=abcdefg,d2=33.637000
//sizeof(sa)=48:&c=12ff28,&h=12ff2c,&n=12ff30,&m=12ff34,&f=12ff38,&d1=12ff40,&s=12ff48,&d2=12ff50
//sb:c=K,h=-3,n=20,m=100000000,f=33.320000,d1=78.572000,s=abcdefg,d2=33.637000
//读出结构体：
//sb:c=K,h=-3,n=20,m=100000000,f=33.320000,d1=78.572000,s=e,d2=33.637000
//sizeof(sb)=48:&c=12ff28,&h=12ff2c,&n=12ff30,&m=12ff34,&f=12ff38,&d1=12ff40,&s=12
//
//从上面的结果我们可以得到如下几个结论：
//1. 如果结构体中含有指针，是很容易出问题的，从上面的结果中（高亮）部分可以看到字符串的输出结果是不一样的，这说明，在进行写入文件的时候，char*所 指向的字符串没有写入文件，只是将指针写入，当从文件中读出结构体，再次得到这个指针的时候，由于程序运行的内存位置变化，所以原来指针所指向的内容也变 了，所以输出也变了。
//
//2. 还有一个比较重要的是结构体的内存对其问题（之前也讨论过）。可以看到，不同的编译器采取的方式是不一样的。 gcc中的结构体大小为40字节，而VC下是48个字节。
//
//并 且GCC下，结构体内存的起始位置是4的倍数，而VC中是8的倍数。这是因为，结构体的起始地址与其中所包含的拥有最多字节的类型有关。之前也说过，因为 GCC的处理方式是如果结构体内有超过4个字节的类型，那么结构体起始位置以4的倍数开始， 而VC中是以最大字节数的为准。
//
//ps：4的倍数就是地址的最低两位是00，8的倍数就是地址最低三位为000
//然后我分别在linux下读入winxp写的结构体文件和在winxp下读入linux下写的结构体文件，两个平台下的程序都崩溃了。




