//#define _PERMUTE
#ifdef _PERMUTE
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using std::string;
using std::endl;
using std::cout;
using std::vector;
using std::sort;
using std::swap;
using std::next_permutation;
vector<string> arr;
//template<class T>
//void swap(T& a,T& b){
//    T tmp;
//    tmp=a;
//    a=b;
//    b=tmp;
//}
//implemented using the string type
void permute(string str,int k){
    if(k==str.length())
    {
//        for(int i=0;i<str.length();i++)
//            cout<<str[i];
//        cout<<endl;
        arr.push_back(str);
    }
    else{
        for(int i=k;i<str.length();i++)
        {
            swap(str[i],str[k]);
            permute(str,k+1);
            //swap(str[i],str[k]);//this line is not needed, 'cause, the parameter is passed by value
                                  //each recursion the 'str' is a copy
        }
    }
}
//implemented using the char* type
void permute(char* str,int k,int n){
    if(k==n){
        //for(int i=0;i<n;i++)
        //    cout<<str[i];
        //cout<<endl;
        arr.push_back(string(str));
    }
    else{
        for(int i=k;i<n;i++){
            swap(str[i],str[k]);
            permute(str,k+1,n);
            swap(str[i],str[k]);
        }
    }
}
//implemented using the stl algorithm: next_permutation
void permute(string str){
    sort(str.begin(),str.end());
    arr.push_back(str);
    while(next_permutation(str.begin(),str.end())){
        arr.push_back(str);
    }
}
int  main()
{
    string str="ABCDE";
    permute(str,0);
    sort(arr.begin(),arr.end());
    vector<string> t1(arr);
    arr.clear();
    char* str2=new char[str.length()+1];
    memcpy(str2,str.c_str(),str.length()+1);
    permute(str2,0,str.length());
    sort(arr.begin(),arr.end());
    vector<string> t2(arr);
    arr.clear();

    permute(str);
    sort(arr.begin(),arr.end());
    vector<string> t3(arr);

    //int i;
    //for(i=1;i<arr.size();i++)
    //    if(arr[i]==arr[i-1])
    //        cout<<"error:"<<i<<endl;
    //if(i==arr.size())
    //    cout<<"ok"<<endl;
    
    for(int i=0;i<t1.size();i++){
        cout<<i<<':'<<t1[i]<<' '<<t2[i]<<' '<<t3[i]<<' ';
        if(t1[i]==t2[i]&&t2[i]==t3[i])
            cout<<"OK"<<endl;
        else
            cout<<"NO"<<endl;
    }

    delete[] str2;
    return 0;

}
#endif
