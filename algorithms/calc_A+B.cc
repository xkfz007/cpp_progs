/*
 * calc_A+B.cc
 *
 *  Created on: 2016-5月-22 下午10:33:39
 *      Author: Felix
 */
/*
 * 计算A+B
 * 一个比较简单的程序题。题目虽简单，但写起来需要注意的东西还是很多的。
 * 题目：读入两个小于100的正整数A和B,计算A+B.需要注意的是:A和B的每一位数字由对应的英文单词给出.
 * 输入：测试输入包含若干测试用例,每个测试用例占一行,格式为"A + B =",相邻两字符串有一个空格间隔.当A和B同时为0时输入结束,相应的结果不要输出.输出：
 * 对每个测试用例输出1行,即A+B的值.
 * 样例输入：one + two =
 * three four + five six =
 * zero seven + eight nine =
 * zero + zero =
 * 样例输出：3 90 96
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
using namespace std;
const string digits[10]={"zero","one","two","three","four","five","six","seven","eight","nine"};
string& trim(string& str){
    if(str.empty())
        return str;
    str.erase(0,str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ')+1);

    return str;
}
int index(const string& s){
    for(int i=0;i<10;i++)
        if(s==digits[i])
            return i;
    return -1;
}
int str2int(const string& s){
    int n=0;
    size_t pos=0;
    size_t pos2=s.find(' ');
    string sub;
    int di;
    while(pos2!=string::npos){
        sub=s.substr(pos,pos2-pos);
//        cout<<sub<<endl;
        di=index(sub);
        n=n*10+di;
        pos=pos2+1;
        pos2=s.find(' ',pos);
    }
    sub=s.substr(pos);
//    cout<<sub<<endl;
    di=index(sub);
    n=n*10+di;
    return n;
}
int add_str(string str){
    size_t pos,pos2;
    pos=str.find('+');
    string s1=str.substr(0,pos);
    pos2=str.find('=');
    string s2=str.substr(pos+1,pos2-(pos+1));
    trim(s1);trim(s2);
    int a1=str2int(s1);
    int a2=str2int(s2);

    return a1+a2;
}
string int2str(int a){
    ostringstream out;
    out<<a;
    string s=out.str();
    string s2;
    string::iterator ite;
    for(ite=s.begin();ite!=s.end();++ite){
        s2+=digits[*ite-'0'];
        s2+=' ';
    }
    return s2;
}
int test1(){
    for(int i=0;i<10;i++){
        int a1=rand()%1000;
        int a2=rand()%1000;
        int sum=a1+a2;
        string s1=int2str(a1);
        string s2=int2str(a2);
        string str=s1+" + "+s2+" = ";
        int sum2=add_str(str);
        cout<<a1<<' '<<a2<<" <"<<str<<"> "<<sum<<" "<<sum2;
        if(sum==sum2)
            cout<<" OK"<<endl;
        else
            cout<<" NO"<<endl;

    }

    return 0;
}
int main1(){
    string str;
    int sum;
    while(getline(cin,str)){
        sum=add_str(str);
        cout<<sum<<endl;
    }
    return 0;
}
int main(){
    test1();
}





