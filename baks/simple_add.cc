/*
a simple program to add numbers
*/
#ifdef _SIMPLE_ADD
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
 
int main(){
    string str;
    int sum;
    while(getline(cin,str))
    {
    size_t pos,pos2;
    pos=str.find('+');
    string s1=str.substr(0,pos);
    pos2=str.find('=');
    string s2=str.substr(pos+1,pos2-pos-1);
    trim(s1);trim(s2);
    int a1=str2int(s1);
    int a2=str2int(s2);
    if(a1==0&&a2==0)
        break;
    sum=a1+a2;
        cout<<sum<<endl;
    }
    return 0;
}
#endif