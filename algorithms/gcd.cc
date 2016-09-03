/*
 * gcd.cc
 *
 *  Created on: 2016-5月-22 下午10:30:29
 *      Author: Felix
 */

/*
 * 求最大公约数
 * 求两个数的最大公约数问题，即常说的GCD问题，是一个经常用到的问题。
 * 这个问题的求解最常用的方法就是辗转相除的方法。这个方法其实就是下面的一个公式：
 *
 * gcd(a,b)=gcd(b,a%b)
 * 记住了这个公式就很容易写出它的程序了。
 */

//第一：可以采用递归的方法：
unsigned gcd(unsigned a,unsigned b){
    if(!b)
        return a;
    if(a<b)
        return gcd(b,a);
    else
        return gcd(b,a%b);
}
//第二：所有的递归都可以转为迭代实现：
unsigned gcd2(unsigned a,unsigned b)
{
    unsigned tmp;
    while(b!=0)
    {
        tmp=a%b;
        a=b;
        b=tmp;
    }
    return a;
}
//另一种不常见的方法是二进制gcd。 这种方法主要用到了gcd的如下几个性质：
// 1. 如果a,b都是偶数, 则gcd(a, b) = gcd(a/2, b/2)
// 2. 如果a是奇数, b是偶数,  则gcd(a, b) =  gcd(a, b/2)
// 3. 如果a,b都是奇数, 则gcd(a, b) = gcd((a-b)/2, b)
unsigned binary_gcd(unsigned x,unsigned y){
    if(y==0)
        return x;
    if(x<y)
        return gcd(y,x);
    else{
        if(((x|y)&0x1)==0)
            return 2*gcd(x>>1,y>>1);
        else if((x&0x1)==0)
            return gcd(x>>1,y);
        else if((y&0x1)==0)
            return gcd(x,y>>1);
        else
            return gcd(y,x-y);
    }
}
unsigned binary_gcd2(unsigned x,unsigned y)
{
    unsigned common_power_of_two=0;
    unsigned tmp;
    if(x==0)
        return y;
    if(y==0)
        return x;
    while(((x|y)&0x1)==0)
    {
        x>>=1;
        y>>=1;
        ++common_power_of_two;
    }
    while((x&0x1)==0)
        x>>=1;
    while(y)
    {
        while((y&0x1)==0)
            y>>=1;
        tmp=y;
        y=(x>y)?x-y:y-x;
        x=tmp;
    }
    return x<<=common_power_of_two;

}
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;
int main()
{
    unsigned a,b;
   unsigned val,val2,val3,val4;
    srand(time(0));
    int i=0;
    while(i<1000)
    {
        a=rand()%1000;
        b=rand()%1000;
        val=gcd(a,b);
        if(val==1)
            continue;
        val2=binary_gcd(a,b);
        val3=gcd2(a,b);
        val4=binary_gcd2(a,b);
        cout<<i<<'\t';
        cout<<"("<<a<<","<<b<<")\t";
        cout<<val<<'\t'<<val2<<'\t'<<val3<<'\t'<<val4<<'\t';
        if(val==val2&&val2==val3&&val3==val4)
            cout<<"OK";
        else
            cout<<"Bad";
        cout<<endl;
        i++;
    }
    return 0;
}



