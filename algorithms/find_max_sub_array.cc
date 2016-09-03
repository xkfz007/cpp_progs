#ifdef _FIND_MAX_SUB_ARRAY
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

/*
 * 方法一：暴力枚举法
此种方法最简单，我想应该也是每个人拿到题目想到的第一种解法了，学过一点编程的人都应该能编出此类程序。
记sum[i..j]为数组中第i个元素到第j个元素的和（其中0<=i<j<=n-1），通过遍历所有的组合之和，就能找到最大的一个和了。
伪代码如下：
int maxSubArray(int *A,int n) {
    int maxium = -INF; //保存最大子数组之和
    for i=0 to n-1 do
        sum = 0; //sum记录第i到j的元素之和
        for j=i to n-1 do
            sum += A[j];
        if sum>maxium do //更新最大值
            maxium = sum;
    return maxium;
}
此种方法的时间复杂度为O(n2)，显然不是一种很好的办法，也不是公司面试希望你写出这样的程序的。
 */
const int INF=0x7fffffff;
int max_sub_array_enum(int arr[],int n,int &left,int &right)
{
    int maxium=-INF;
    int sum;
    for(int i=0;i<n;i++){
        sum=0;
        for(int j=i;j<n;j++){
            sum+=arr[j];
            if(sum>maxium){
                maxium=sum;
                left=i;
                right=j;
            }
        }
    }

    return maxium;
}
/*
 * 方法二：分支界定
这里再介绍一种更高效的算法，时间复杂度为O(nlogn)。这是个分治的思想，解决复杂问题我们经常使用的一种思维方法——分而治之。
而对于此题，我们把数组A[1..n]分成两个相等大小的块：A[1..n/2]和A[n/2+1..n]，最大的子数组只可能出现在三种情况：
    A[1..n]的最大子数组和A[1..n/2]最大子数组相同；
    A[1..n]的最大子数组和A[n/2+1..n]最大子数组相同；
    A[1..n]的最大子数组跨过A[1..n/2]和A[n/2+1..n]
前两种情况的求法和整体的求法是一样的，因此递归求得。
第三种，我们可以采取的方法也比较简单，沿着第n/2向左搜索，直到左边界，找到最大的和maxleft，以及沿着第n/2+1向右搜索找到最大和maxright，那么总的最大和就是maxleft+maxright。
而数组A的最大子数组和就是这三种情况中最大的一个。
伪代码如下：
int maxSubArray(int *A,int l,int r) {
    if l<r do
        mid = (l+r)/2;
        ml = maxSubArray(A,l,mid); //分治
        mr = maxSubArray(A,mid+1,r);
        for i=mid downto l do
            search maxleft;
        for i=mid+1 to r do
            search maxright;
        return max(ml,mr,maxleft+maxright); //归并
        then //递归出口
            return A[l];
}
 */
int max_sub_array_binary(int arr[],int l,int r,int &left,int &right)
{
    if(l<r){
        int mid=(l+r)/2;
        int ll,lr;
        int suml=max_sub_array_binary(arr,l,mid,ll,lr);
        int rl,rr;
        int sumr=max_sub_array_binary(arr,mid+1,r,rl,rr);
        int sum_both=0;
        int max_left=-INF;
        int ml,mr;
        for(int i=mid;i>=l;i--)
        {
            sum_both+=arr[i];
            if(sum_both>max_left){
                max_left=sum_both;
                ml=i;
            }
        }
        int max_right=-INF;
        sum_both=0;
        for(int i=mid+1;i<=r;i++)
        {
            sum_both+=arr[i];
            if(sum_both>max_right){
                max_right=sum_both;
                mr=i;
            }
        }
        sum_both=max_left+max_right;
        if(suml<sumr) {
            if(sumr<sum_both) {
                left=ml;
                right=mr;
                return sum_both;
            }
            else {
                left=rl;
                right=rr;
                return sumr;
            }

        }
        else{
            if(suml<sum_both) {
                left=ml;
                right=mr;
                return sum_both;
            }
            else {
                left=ll;
                right=lr;
                return suml;
            }

        }
    }
    else {
        left=l;
        right=r;
        return arr[l];
    }
}

/*
 * 方法三：动态规划
这算是一个经典的动态规划的题目了，如果不知道动态规划可以先不去理解这个名词。用通俗点的语言描述这个算法就是：
令cursum(i)表示数组下标以i为起点的最大连续下标最大的和，而maxsum(i)表示前i个元素的最大子数组之和。那么我们就可以推出下一个maxsum(i+1)应该为cursum(i+1)和maxsum(i)中选取一个最大值。递推式为：
cursum(i) = max{A[i],cursum(i-1)+A[i]};
maxsum(i) = max{maxsum(i-1),cursum(i+1)};
伪代码为：
int maxSubArray(int *A,int n) {
    cursum = A[0];
    maxsum = A[0];
    for i=1 to n-1 do
        //当我们加上一个正数时，和会增加；当我们加上一个负数时，和会减少。如果当前得到的和是个负数，那么这个和在接下来的累加中应该抛弃并重新清零，
        //不然的话这个负数将会减少接下来的和。
        if cursum<0 do
            cursum = 0;
        cursum += A[i];
        if cursum>maxsum do
            maxsum = cursum;
    return maxsum;
}
这种算法时间复杂度只是O(n)，效果非常好！
 */
int max_sub_array_dp(int arr[],int n,int& left,int&right)
{
    int cursum=arr[0];
    int maxsum=arr[0];
    int pos=0;
     for(int i=1;i<n;i++) {
        cursum+=arr[i];
        if(cursum>maxsum)
        {
            maxsum=cursum;
            left=pos;
            right=i;
        }
        if(cursum<arr[i])
        {
            pos=i;
            cursum=arr[i];
        }
    }
    return maxsum;

}
void test1()
{
    int arr[]={-2,5,3,-6,4,-8,6};
    int len=sizeof(arr)/sizeof(arr[0]);
    int left,right;
    int sum;
    cout<<"arr:";
    copy(arr,arr+len,ostream_iterator<int>(cout, " "));
    cout<<endl;
    sum=max_sub_array_enum(arr,len,left,right);
    cout<<"method1:("<<left<<","<<right<<")  ";
    cout<<"sum="<<sum<<endl;
    sum=max_sub_array_binary(arr,0,len-1,left,right);
    cout<<"method2:("<<left<<","<<right<<")  ";
    cout<<"sum="<<sum<<endl;
    sum=max_sub_array_dp(arr,len,left,right);
    cout<<"method3:("<<left<<","<<right<<")  ";
    cout<<"sum="<<sum<<endl;

}
void test2()
{
    const int LEN=10;
    int arr[LEN];
    int sign[LEN];
    srand(time(0));
    for(int i=0;i<LEN;i++){
        int val=rand()%1000;
        if(val%2==0)
            sign[i]=1;
        else
            sign[i]=-1;
    }
    for(int i=0;i<LEN;i++){
        int val=rand()%100;
        arr[i]=val*sign[i];
    }
    int left,right;
    int sum;
    int len=LEN;
    cout<<"arr:";
    copy(arr,arr+len,ostream_iterator<int>(cout, " "));
    cout<<endl;
    sum=max_sub_array_enum(arr,len,left,right);
    cout<<"method1:("<<left<<","<<right<<")  ";
    cout<<"sum="<<sum<<endl;
    sum=max_sub_array_binary(arr,0,len-1,left,right);
    cout<<"method2:("<<left<<","<<right<<")  ";
    cout<<"sum="<<sum<<endl;
    sum=max_sub_array_dp(arr,len,left,right);
    cout<<"method3:("<<left<<","<<right<<")  ";
    cout<<"sum="<<sum<<endl;


}
int main()
{
        test2();
}
#endif
