//#define _CALC_EXPRESSION_
#ifdef _CALC_EXPRESSION_
/*
insert operator '+' or '-' into expression '123456789=100' to make it sense
from https://blog.svpino.com/2015/05/07/five-programming-problems-every-software-engineer-should-be-able-to-solve-in-less-than-1-hour
*/
/*
 * 添加"+"或"-"，使得表达式"123456789=100"成立
 * 这是一个小算法题，题目来自于这里，解答在这里
 * 说实话，开始没有做出来，使用的是暴力破解的方法进行穷举。
 * 下面分析基本是从作者那边copy过来的。
 * 这个题目使用采用分而治之的方法来进行解决。
 * 假设f是相应的算法，那么上面的问题可以表示为：
 * f(123456789)=100,
 * 根据分治法可以分解为下面三个子问题：
 * 1+f(2..9)=100
 * 1-f(2..9)=100
 * f(12,3..9)=100
 * 即：
 * f(2...9)=99
 * f(-2,3..9)=99 //这个地方比较有技巧，这儿主要是为了表示成一致的方式
 * f(12,3...9)=100
 * 上面每一个还可以再继续分解：
 * 2+f(3..9)=99
 * 2-f(3...9)=99
 * f(23,4..9)=99
 *
 * -2+f(3..9)=99
 * -2-f(3..9)=99
 * f(-23,4..9)=99
 *
 * 12+f(3..9)=100
 * 12-f(3..9)=100
 * f(123,4..9)=100
 *
 * 当然可以继续分解下去，那么什么时候分解结束呢？这个分解结束其实就是我们代码中递归调用的结束条件。
 *  *
 */
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
using namespace std;
int total=0;
//a simple calculator: calculate expression in str
//forms: 1+2-3+4-5+6
//also can be implemented use strtok in C
int calc_expr(string str){
    size_t len=str.length();
    size_t p0=0;
    size_t p1=str.find_first_of("+-",p0);
    int result=0;
    int sign=1;
    while(p1<=len){
        string num=str.substr(p0,p1-p0);
        int val=atoi(num.c_str());
        result+=val*sign;
        if(p1<len){
            if(str[p1]=='+')
                sign=1;
            else if(str[p1]=='-')
                sign=-1;
        }
        p0=p1+1;
        if(p0<len)
            p1=str.find_first_of("+-",p0);
        else
            p1=len+1;
        if(p1==string::npos)
            p1=len;
    }
    return result;
}
void print(const string& str){
    total++;
    printf("%5d:%s=%d\n",total,str.c_str(),calc_expr(str));
}
string atos(int num){
    stringstream ss;
    ss<<num;
    return ss.str();
}
//brute force
//can be used to test whether our algorithm is right
void get_expr(int d,string expr){
    if(d==9){
        if(calc_expr(expr)==100)
            print(expr);
    }
    else if(d<9){
        get_expr(d+1,expr+""+atos(d+1));
        get_expr(d+1,expr+"+"+atos(d+1));
        get_expr(d+1,expr+"-"+atos(d+1));
    }

}
int arr[]={1,2,3,4,5,6,7,8,9};
#if 0
void f(int sum,int number,int idx,string str){
    if(idx>=9){
        if(sum==number)
            print(str);
        return;
    }
    f(sum-number,arr[idx],idx+1,str+"+"+atos(arr[idx]));
    f(sum-number,-arr[idx],idx+1,str+"-"+atos(arr[idx]));
    int val=number>=0?number*10+arr[idx]:number*10-arr[idx];
    f(sum,val,idx+1,str+atos(arr[idx]));
}
#endif

#if 0
void f(int sum,int number,int d,string str){
    if(d>=9){
        if(sum==number)
            print(str);
        return;
    }
    f(sum-number,d+1,d+1,str+"+"+atos(d+1));
    f(sum-number,-(d+1),d+1,str+"-"+atos(d+1));
    int val=number>=0?number*10+(d+1):number*10-(d+1);
    f(sum,val,d+1,str+atos(d+1));
}
#endif
#if 1
//remove the parameter 'd' in the previous version
// maybe this is the best version
void f(int sum,int number,string str){
    int d=abs(number)%10;//get the latest digit we are processing
    if(d>=9){
        if(sum==number){
            printf("number=%d ",number);
            print(str);
        }
        return;
    }
    f(sum-number,d+1,str+"+"+atos(d+1));
    f(sum-number,-(d+1),str+"-"+atos(d+1));
    int val=number>=0?number*10+(d+1):number*10-(d+1);
    f(sum,val,str+atos(d+1));
}
#endif
int main()
{
    //f(100,1,1,"1");
    f(100,1,"1");
}

#endif
