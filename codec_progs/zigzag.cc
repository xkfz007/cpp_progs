/*
 * zigzag.cc
 *
 *  Created on: 2016-5月-23 下午11:26:30
 *      Author: Felix
 */

#include <iostream>
#include <iomanip>
using namespace std;
void zigzag(int n){
    const int N=n;
    int a[N][N];
    int squa=N*N;
    int b[N*N][2];
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++){
            int s=i+j;
            if(s<N) {
                a[i][j]=s*(s+1)/2+((i+j)%2==0?j:i);
            }
            else{
                s=(N-1-i)+(N-1-j);
                a[i][j]=squa-s*(s+1)/2-(N-((i+j)%2==0?j:i));
            }
            b[a[i][j]][0]=i;
            b[a[i][j]][1]=j;
        }
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            cout<<setw(2)<<a[i][j]<<" ";
        cout<<endl;
    }
    for(int i=0;i<N*N;i++) {
        for(int j=0;j<2;j++)
            cout<<setw(2)<<b[i][j]<<' ';
        cout<<endl;
    }
}
int main(){
    int n=4;
    zigzag(n);
}




