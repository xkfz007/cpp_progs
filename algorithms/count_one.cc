/*
 * count_one.cc
 *
 *  Created on: 2016-5月-22 下午10:47:00
 *      Author: Felix
 */
/*
 * 如何判断整数x的二进制中含有多少个1
 * 主要是利用位操作来判断每一位是否为1
 */
//清除整数a最右边的1可以使用这个操作：a&(a-1)
// 所以可以利用这个方法来得到1的个数：
int count_one(unsigned x){
    int count=0;
    while(x){
        count++;
        x=x&(x-1);
    }
    return count;
}
//对整数进行移位，然后判断最后一位是否为1
int count_one2(unsigned x){
    int count=0;
    int i=0;
    while(i<32){
        if((x>>i)&0x1)
            count++;
        i++;
    }
    return count;
}
//这个方法和方法二类似，只是每次移动4位。
int count_one3(unsigned x){
    static const int hex[]={0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
    int count=0;
    int i=0;
    while(i<8){
        count+=hex[(x>>(i*4))&0xf];
        i++;
    }
    return count;

}
#include <iostream>
using namespace std;
int main(){
    int n=20;
    for(int i=0;i<n;i++){
        unsigned val=rand()%100;
        int c1=count_one(val);
        int c2=count_one2(val);
        int c3=count_one3(val);
        cout<<val<<" "<<c1<<" "<<c2<<" "<<c3<<endl;
    }
}





