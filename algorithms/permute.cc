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
#if 0
/*
 * 1. 排列算法
 * 这个主要采用递归的方法来解决。
 */
void swap(int& a,int& b)
{
    if(a==b)
        return ;
    int t=a;
    a=b;
    b=t;
}
void perm(int arr[],int k,int m)
{
    if(k==m){
        copy(arr,arr+m+1,ostream_iterator<int>(cout," "));
        cout<<endl;
    }
    else{
        for(int i=k;i<=m;++i)
        {
            swap(arr[i],arr[k]);
            perm(arr,k+1,m);
            swap(arr[i],arr[k]);
        }
    }
}
/*
 * 2.组合算法
 * 组合问题就是从n中选m个数，也是采用递归的方式
 * a. 首先从n个数中选取编号最大的数，然后在剩下的n-1个数里面选取m-1个数，直到从n-(m-1)个数中选取1个数为止。
 * b. 从n个数中选取编号次小的一个数，继续执行1步，直到当前可选编号最大的数为m。
 * 下面是递归方法的实现：
/// 求从数组a[1..n]中任选m个元素的所有组合。
 /// a[1..n]表示候选集，n为候选集大小，n>=m>0。
 /// b[1..M]用来存储当前组合中的元素(这里存储的是元素下标)，
 /// 常量M表示满足条件的一个组合中元素的个数，M=m，这两个参数仅用来输出结果。
 */

void combine( int a[], int n, int m,  int b[], const int M )
{
  for(int i=n; i>=m; i--)   // 注意这里的循环范围
  {
    b[m-1] = i - 1;
     if (m > 1)
       combine(a,i-1,m-1,b,M);
     else                     // m == 1, 输出一个组合
     {
       for(int j=M-1; j>=0; j--)
       cout << a[b[j]] << " ";
       cout << endl;
     }
   }
 }


#endif
