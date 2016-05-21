/*
 * realize the multiply of bigint
 */
#if 0
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

void multiply(vector<int> &a,vector<int> &b,vector<int> &result)
{
    int k;
    for(int i=0;i<a.size();++i){
        k=i;
        for(int j=0;j<b.size();++j){
            result[k++]+=a[i]*b[j];
        }
    }
    for(k=result.size()-1;k>=0;--k){
        if(result[k]>9){
            if(k!=0){
                result[k-1]+=result[k]/10;
                result[k]%=10;
            }
            else{
                int tmp=result[k]/10;
                result[k]%=10;
                result.insert(result.begin(),tmp);
            }
        }
    }

}
int main(int argc,char*argv[])
{
    if(argc<3){
        cout<<"Usage:"<<argv[0]<<" num1 num2"<<endl;
        return -1;
    }
    string s1(argv[1]),s2(argv[2]);
    vector<int> a,b;
    a.reserve(s1.size());
    b.reserve(s2.size());
    for(int i=0;i<s1.size();++i){
        a.push_back(s1[i]-'0');
    }
    for(int i=0;i<s2.size();++i){
        b.push_back(s2[i]-'0');
    }
    vector<int>c(a.size()+b.size()-1,0);
    multiply(a,b,c);
    for(int i=0;i<c.size();++i)
        cout<<c[i];
    cout<<endl;

    return 0;

    
}
#endif