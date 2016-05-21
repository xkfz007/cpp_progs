/*
 * =====================================================================================
 *
 *       Filename:  BigInteger.cc
 *
 *    Description: C++ BigInteger 
 *
 *        Version:  1.0
 *        Created:  03/23/2012 05:09:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fangzhen (), fzhu@jdl.ac.cn
 *        Company:  JDL
 *
 * =====================================================================================
 */
#ifdef _BIGINTEGER
#include <deque>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

using std::deque;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::ostream;
using std::istream;

class DividedByZeroException{};
class BigInteger{
    public:
        BigInteger(int);
        BigInteger(string&);
        BigInteger();
        BigInteger(const BigInteger&);
        BigInteger& operator=(const BigInteger& rhs);
        
        BigInteger& operator++();
        BigInteger operator++(int);
        BigInteger& operator--();
        BigInteger operator--(int);

        BigInteger& operator+=(const BigInteger&);
        BigInteger& operator-=(const BigInteger&);
        BigInteger& operator*=(const BigInteger&);
        BigInteger& operator/=(const BigInteger&)throw(DividedByZeroException);
        BigInteger& operator%=(const BigInteger&)throw(DividedByZeroException);

        friend BigInteger operator-(const BigInteger&);
        friend BigInteger operator+(const BigInteger&, const BigInteger&);
        friend BigInteger operator-(const BigInteger&, const BigInteger&);
        friend BigInteger operator*(const BigInteger&, const BigInteger&);
        friend BigInteger operator/(const BigInteger&, const BigInteger&)throw(DividedByZeroException);
        friend BigInteger operator%(const BigInteger&, const BigInteger&)throw(DividedByZeroException);
        friend bool operator>(const BigInteger&, const BigInteger&);
        friend bool operator<(const BigInteger&, const BigInteger&);
        friend bool operator==(const BigInteger&, const BigInteger&);
        friend bool operator!=(const BigInteger&, const BigInteger&);
        friend bool operator>=(const BigInteger&, const BigInteger&);
        friend bool operator<=(const BigInteger&, const BigInteger&);
        
        friend BigInteger abs(const BigInteger&);
        friend BigInteger pow(const BigInteger&, int);


        friend ostream& operator<<(ostream&,const BigInteger&);
        friend istream& operator>>(istream&, BigInteger&);
    private:
        void trim();
        //positive operation
        void add(const vector<char>&);
        void substract(const vector<char>&);
        void multiply(const vector<char>&);
        void devide(const vector<char>&);
        void mod(const vector<char>&);

        bool g_than(const vector<char>&)const;
        bool g_equal(const vector<char>&)const;
        bool equal(const vector<char>&)const;

    public:
        static const BigInteger ZERO;
        static const BigInteger ONE;
        static const BigInteger TEN;
    private:
        vector<char> digits;
        bool sign;
};

const BigInteger BigInteger::ZERO=BigInteger(0);
const BigInteger BigInteger::ONE=BigInteger(1);
const BigInteger BigInteger::TEN=BigInteger(10);

BigInteger::BigInteger():sign(true){ }

BigInteger::BigInteger(int val){
    if(val>=0)
        sign=true;
    else
    {
        sign=false;
        val*=-1;
    }
    do{
        digits.push_back((char)val%10);
        val/=10;
    }while(val!=0);
}

BigInteger::BigInteger(string& def){
    sign=true;
    for(string::reverse_iterator iter=def.rbegin();iter!=def.rend();++iter){
        char ch(*iter);
        cout<<"ch:"<<ch<<endl;
        if(iter==def.rend()-1){
            if(ch=='+')
                break;
            if(ch=='-')
            {
                sign=false;
                break;
            }
        }
        digits.push_back(ch-'0');
    }
    trim();
}

void BigInteger::trim(){
    vector<char>::reverse_iterator iter=digits.rbegin();
    while(!digits.empty()&&(*iter)==0){
        digits.pop_back();
        iter=digits.rbegin();
    }
    if(digits.size()==0){
        sign=true;
        digits.push_back(0);
    }
}

BigInteger::BigInteger(const BigInteger& rhs):sign(rhs.sign),digits(rhs.digits){} 
BigInteger& BigInteger::operator=(const BigInteger& rhs){
    if(this!=&rhs){
        sign=rhs.sign;
        digits=rhs.digits;
    }
    return *this;
}
BigInteger abs(const BigInteger& rhs){
    BigInteger res(rhs);
    res.sign=true;
    return res;
}
BigInteger pow(const BigInteger& rhs,int a){
    BigInteger res(1);
    for(int i=0;i<a;++i){
        res*=rhs;
    }
    return res;
}


bool BigInteger::g_than(const vector<char>&rhs)const{
    if(digits.size()>rhs.size())
        return true;
    else if(digits.size()<rhs.size())
        return false;
    else{
        vector<char>::const_reverse_iterator iter1;
        vector<char>::const_reverse_iterator iter2;
        iter1=digits.rbegin();
        iter2=rhs.rbegin();
        while(iter1!=digits.rend()){
            if(*iter1>*iter2)
                return true;
            else if(*iter1<*iter2)
                return false;
            else{
                ++iter1;
                ++iter2;
            }
        }
        return false;
    }
}
bool BigInteger::g_equal(const vector<char>&rhs)const{
    if(digits.size()>rhs.size())
        return true;
    else if(digits.size()<rhs.size())
        return false;
    else{
        vector<char>::const_reverse_iterator iter1;
        vector<char>::const_reverse_iterator iter2;
        iter1=digits.rbegin();
        iter2=rhs.rbegin();
        while(iter1!=digits.rend()){
            if(*iter1>*iter2)
                return true;
            else if(*iter1<*iter2)
                return false;
            else{
                ++iter1;
                ++iter2;
            }
        }
        return true;
    }
}
bool BigInteger::equal(const vector<char>& rhs)const{
    if(digits.size()!=rhs.size())
        return false;
    else{
        vector<char>::const_reverse_iterator iter1;
        vector<char>::const_reverse_iterator iter2;
        iter1=digits.rbegin();
        iter2=rhs.rbegin();
        while(iter1!=digits.rend()){
            if(*iter1!=*iter2)
                return false;
            else{
                ++iter1;
                ++iter2;
            }
        }
        return true;
    }

}
void BigInteger::add(const vector<char>& rhs){
    vector<char>::iterator iter1;
    vector<char>::const_iterator iter2;
    iter1=digits.begin();
    iter2=rhs.begin();
    char carry=0;
    while(iter1!=digits.end()&&iter2!=rhs.end()){
        *iter1+=(*iter2+carry);
        carry=(*iter1>9);
        *iter1%=10;
        ++iter1;
        ++iter2;
    }
    while(iter1!=digits.end()){
        (*iter1)+=carry;
        carry=(*iter1>9);
        *iter1%=10;
        ++iter1;
    }
    while(iter2!=rhs.end()){
        char val=(*iter2)+carry;
        carry=(val>9);
        val%=10;
        digits.push_back(val);
        ++iter2;
    }
    if(carry!=0)
        digits.push_back(carry);


}
void BigInteger::substract(const vector<char>& rhs){
    vector<char>::iterator iter1;
    vector<char>::const_iterator iter2;
    iter1=digits.begin();
    iter2=rhs.begin();
    char borrow=0;
    while(iter1!=digits.end()&&iter2!=rhs.end()){
        *iter1-=(*iter2+borrow);
        if(*iter1<0){
            borrow=1;
            *iter1+=10;
        }
        ++iter1;
        ++iter2;
    }
    while(iter1!=digits.end()){
        (*iter1)-=borrow;
        if(*iter1<0){
            borrow=1;
            *iter1+=10;
        }
        else
            break;
        ++iter1;
    }

}
void BigInteger::multiply(const vector<char>& rhs){

    vector<char> res(digits.size()+rhs.size()-1,0);
    vector<char>::iterator k;
    vector<char>::iterator iter1;
    vector<char>::const_iterator iter2;

    for(iter1=digits.begin();iter1!=digits.end();++iter1){
        k=res.begin()+(iter1-digits.begin());
        for(iter2=rhs.begin();iter2!=rhs.end();++iter2,++k){
            *k+=(*iter1)*(*iter2);
        }
    }
    for(k=res.begin();k!=res.end();++k){
        if(*k>9){
            if(k!=res.end()-1){
                *(k++)=*k/10;
                *k%=10;
            }
            else{
                char val=*k/10;
                *k%=10;
                res.push_back(val);
                break;
            }
        }
    }
    digits=res;
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs){
    if(sign==rhs.sign)
        add(rhs.digits);
    else{
        if(g_equal(rhs.digits)){
            substract(rhs.digits);
        }
        else{
            vector<char> tmp(digits);
            digits=rhs.digits;
            substract(tmp);
            sign=rhs.sign;
        }
        trim();
    }
    return *this;
}
BigInteger& BigInteger::operator-=(const BigInteger& rhs){
    if(sign==rhs.sign){
        if(sign){
            if(g_equal(rhs.digits)){

                substract(rhs.digits);

            }
            else{
                vector<char> tmp(digits);
                digits=rhs.digits;
                substract(tmp);
                sign=false;
            }
        }
        else{
            if(g_equal(rhs.digits)){
                substract(rhs.digits);
                sign=false;

            }
            else{
                vector<char> tmp(digits);
                digits=rhs.digits;
                substract(tmp);
                sign=true;
            }
        }
    }
    else{
        add(rhs.digits);
    }
    trim();
    return *this;
}
BigInteger& BigInteger::operator*=(const BigInteger& rhs){
    multiply(rhs.digits);
    sign=(sign==rhs.sign);
    return *this;
}
BigInteger& BigInteger::operator++(){
    *this+=(ONE);
    return *this;
}
BigInteger BigInteger::operator++(int){
    BigInteger res=*this;
    *this+=(ONE);
    return res;
}
BigInteger& BigInteger::operator--(){
    *this-=(ONE);
    return *this;
}
BigInteger BigInteger::operator--(int){

    BigInteger res=*this;
    *this-=(ONE);
    return res;
}

BigInteger operator-(const BigInteger& rhs){
    BigInteger res;
    res.sign=rhs.sign?false:true;
    res.digits=rhs.digits;
    return res;
}
BigInteger operator+(const BigInteger& rhs1,const BigInteger& rhs2){
    BigInteger res(rhs1);
    res+=rhs2;
    return res;
}
BigInteger operator-(const BigInteger& rhs1,const BigInteger& rhs2){

    BigInteger res(rhs1);
    res-=rhs2;
    return res;
}

BigInteger operator*(const BigInteger& rhs1,const BigInteger& rhs2){
    BigInteger res(rhs1);
    res*=rhs2;
    return res;
}
bool operator>(const BigInteger& rhs1,const BigInteger& rhs2){
    if(rhs1.sign==rhs2.sign){
        if(rhs1.sign)
            return rhs1.g_than(rhs2.digits);
        else
            return !rhs1.g_than(rhs2.digits);
    }
    else
    {
        return rhs1.sign;
    }
}
bool operator<(const BigInteger& rhs1,const BigInteger& rhs2){
    return !(rhs1>rhs2||rhs1==rhs2);
}
bool operator==(const BigInteger& rhs1,const BigInteger& rhs2){
    if(rhs1.sign==rhs2.sign){
        return rhs1.equal(rhs2.digits);
    }
    else
        return false;
}
bool operator!=(const BigInteger& rhs1,const BigInteger& rhs2){
    return !(rhs1==rhs2);
}
bool operator>=(const BigInteger& rhs1,const BigInteger& rhs2){
    return (rhs1>rhs2||rhs1==rhs2);
}
bool operator<=(const BigInteger& rhs1,const BigInteger& rhs2){
    return (rhs1<rhs2||rhs1==rhs2);
}
ostream& operator<<(ostream& os,const BigInteger& rhs){
    if(!rhs.sign)
        os<<"-";
    for(vector<char>::const_reverse_iterator iter=rhs.digits.rbegin();iter!=rhs.digits.rend();++iter){
        os<<(char)(*iter+'0');
    }

    return os;
}
istream& operator>>(istream& is,BigInteger& rhs){
    string str;
    is>>str;
    cout<<"str="<<str<<endl;
    rhs=BigInteger(str);

    return is;
}
int main(){
    int n=1;
    for(int i=0;i<n;++i){
        BigInteger A;
        BigInteger B(78);
//        BigInteger C=888;
        cin>>A;
        cout<<"A:"<<A<<" B:"<<B<<endl;
        cout<<"A+B:"<<A+B<<endl;
        cout<<"A-B:"<<A-B<<endl;
        cout<<"A*B:"<<A*B<<endl;
        cout<<"pow(A,5):"<<pow(A,5)<<endl;
        A++;
        cout<<"A++:"<<A<<endl;
        ++A;
        cout<<"++A:"<<A<<endl;
        A--;
        cout<<"A--:"<<A<<endl;
        --A;
        cout<<"--A:"<<A<<endl;
        A+=B;
        cout<<"A+=B:"<<A<<endl;
        A-=B;
        cout<<"A-=B:"<<A<<endl;
        A*=B;
        cout<<"A*=B:"<<A<<endl;


    }

}
#endif