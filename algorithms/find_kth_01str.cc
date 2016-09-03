/*
 * find_kth_01str.cc
 *
 *  Created on: 2016-5月-22 下午8:28:19
 *      Author: Felix
 */
/*
 * 找出第K个0和1组成的字符串
 * Time Limit: 10000ms
 * Case Time Limit: 1000ms
 * Memory Limit: 256MB
 *
 * Description
 *
 * Consider a string set that each of them consists of {0, 1} only. All strings in the set have the same number of 0s and 1s.
 * Write a program to find and output the K-th string according to the dictionary order. If s?uch a string doesn’t exist,
 * or the input is not valid, please output “Impossible”. For example, if we have two ‘0’s and two ‘1’s,
 * we will have a set with 6 different strings, {0011, 0101, 0110, 1001, 1010, 1100}, and the 4th string is 1001.
 *
 *
 * Input
 *
 * The first line of the input file contains a single integer t (1 ≤ t ≤ 10000),
 * the number of test cases, followed by the input data for each test case.
 * Each test case is 3 integers separated by blank space: N, M(2 <= N + M <= 33 and N , M >= 0),
 * K(1 <= K <= 1000000000). N stands for the number of ‘0’s, M stands for the number of ‘1’s,
 * and K stands for the K-th of string in the set that needs to be printed as output.
 *
 *
 * Output
 *
 * For each case, print exactly one line. If the string exists, please print it, otherwise print “Impossible”.
 *
 * Sample In
 *
 * 3
 * 2 2 2
 * 2 2 7
 * 4 7 47
 *
 * Sample Out
 *
 * 0101
 * Impossible
 * 01010111011
 *
 * 解答:
 * 这是N个0和M个1的组合问题，可以采用动态规划的方式解决。
 * dp[n][m]：n个0和m个1一共可能的组合数。
 * dp[n][m]递推公式： dp[n][m] = dp[n - 1][m] + dp[n][m - 1]。 表示：如果最高位是1的组合数为dp[n][m - 1]， 如果最高位为0， 组合数为dp[n - 1][m].
 *
 * 如何生成第K个数字：
 * 如果dp[n][m] < K， 返回"Impossible". （最大组合数都小于序号）。
 *
 * 如果最高位是0的组合数大于等于K（dp[n][m - 1] >= K), 那么最高为一定为0。
 * 如果最高位是0的组合数小于K， 那么最高位一定为1。 此时，因为最高位为0的组合有dp[n][m-1]个，问题变成：在N个0和（M-1)个1中需要为(K - dp[n][m-1]）的数字是多少。
 *
 * 下面的代码中包括了使用递归和非递归来实现的
 *
 */

#ifdef _FIND_KTH_01STR
#include <string>
#include <iostream>
#include <vector>
using namespace std;
string find_kth_str(int N,int M,int K){
    vector<vector<int> > dp(N+1,vector<int>(M+1));
    for(int n=0;n<=N;n++){
        for(int m=0;m<=M;m++){
            if(n==0||m==0){
                dp[n][m]=1;
            }
            else{
                dp[n][m]=dp[n][m-1]+dp[n-1][m];
            }
        }
    }
    /*
    for(int n=0;n<=N;n++){
        for(int m=0;m<=M;m++){
            cout<<dp[n][m]<<" ";
        }
        cout<<endl;
    }
    */
    if(dp[N][M]<K)
        return "Impossible";
    string res="";
    while(N>=1&&M>=0&&K>0){
        if(dp[N-1][M]>=K)//The first is "0"
        {
            res+="0";
            N--;//Find in the combinations of N-1 "0" and M "1"
        }
        else{ //The first is "1"
            res+="1";
            K-=dp[N-1][M];//No need to test the combinations of string started with 0
            M--;//Find in the combinations of N "0" and M-1 "1"
        }
    }
    //The remaining "1"
    while(M>0){
        res+="1";
        M--;
    }
    return res;
}
string xstr(vector<vector<int> >& dp,int N,int M,int K){
    if(N==0&&M==0){
        return "";
    }
    if(N==0){
        return "1"+xstr(dp,N,M-1,K);
    }
    if(dp[N-1][M]>=K){
        return "0"+xstr(dp,N-1,M,K);
    }
    else{
        return "1"+xstr(dp,N,M-1,K-dp[N-1][M]);
    }
}
string find_kth_str2(int N,int M,int K){

    vector<vector<int> > dp(N+1,vector<int>(M+1));
    for(int n=0;n<=N;n++){
        for(int m=0;m<=M;m++){
            if(n==0||m==0){
                dp[n][m]=1;
            }
            else{
                dp[n][m]=dp[n][m-1]+dp[n-1][m];
            }
        }
    }
    if(dp[N][M]<K)
        return "Impossible";
    else
        return xstr(dp,N,M,K);
}
int main(){
    int k;
    cin>>k;
    int N,M,K;
    for(int i=0;i<k;i++){
        cin>>N>>M>>K;
        cout<<find_kth_str2(N,M,K)<<endl;
    }
}

#endif

