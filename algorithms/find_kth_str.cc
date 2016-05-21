#ifdef _FIND_KTH_STR
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
