/*
 * joseph_ring.cc
 *
 *  Created on: 2016-5月-23 上午12:23:39
 *      Author: Felix
 */

/*
 * 约瑟夫环问题
 *  约瑟夫环问题：有编号从1到N的N个人坐成一圈报数，报到M的人出局，下一位再从1开始， 如此持续，直止剩下一位为止，报告此人的编号X。输入N,M，求出X。
 *  对于这个问题，有很多解法。包括一些基本的解法和一些优化的算法。
 */

// 基本解法一：
// 这个方法比较直观，就是使用循环列表来模拟约瑟夫环。但是复杂度比较高，为O(m*n)
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
typedef struct _node_t
{
    int num;
    struct _node_t *next;
}node_t;

node_t *list_create(int n)
{
    node_t *p_ret=NULL;
    if(0!=n)
    {
        int i;
        node_t *p_node=(node_t*)malloc(n*sizeof(node_t));
        if(p_node==NULL)
        {
            printf("no enough memorry\n");
            exit(-1);
        }
        i=1;
        p_ret=p_node;
        for(;i<n;i++)
        {
            p_node->num=i;
            p_node->next=p_node+1;
            p_node=p_node->next;
        }
        p_node->num=n;
        p_node->next=p_ret;
    }

    return p_ret;
}
int main()
{
    int n,m;
    int i;
    node_t *p_list,*p_iter;
    printf("Enter n and m:");
    scanf("%d %d",&n,&m);
    p_list=list_create(n);
    if(p_list==NULL)
    {
        printf("can't create linklist\n");
        return -1;
    }
    p_iter=p_list;
    while(p_iter->next!=p_list)
    {
        printf("%d ",p_iter->num);
        p_iter=p_iter->next;
    }
    printf("%d \n",p_iter->num);

    p_iter=p_list;
    while(p_iter!=p_iter->next)
    {
        for(i=1;i<m-1;i++)
            p_iter=p_iter->next;
        printf("%d kicked\n",p_iter->next->num);
        p_iter->next=p_iter->next->next;
        p_iter=p_iter->next;
    }
    printf("%d left\n",p_iter->num);

    free(p_list);
}

//基本解法二：
//这个方法也是比较直观的，使用数组进行模拟。
#if 0
#include <stdio.h>
#define NMAX 50
int main()
{
    int n,m;
    int arr[NMAX];
    int i,count,k;
    printf("Enter n and m:");
    scanf("%d %d",&n,&m);
    //初始化编号
    for(i=0;i<n;i++)
        arr[i]=i+1;

    count=0;
    i=0;
    k=0;//
//不断的循环，直到剩余一个人
    while(count<n-1)
    {
        if(arr[i]!=0)
            k++;
        if(k==m)//数到m的人要被剔除
        {
            printf("%d kicked\n",arr[i]);
            arr[i]=0;//设为0表示被剔除
            k=0;//重新开始数
            count++;//计数剔除人的数目
        }
//数组不断的循环
        i++;
        if(i==n)
            i=0;
    }
//找到编号不为0的即为最后剩下的
    for(i=0;i<n;i++)
        if(arr[i]!=0)
            break;
    printf("%d left\n",arr[i]);


}
#endif

#if 0
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
using namespace std;
int main(){
    int n,m;
    cin>>n>>m;
    vector<int> vr(n,0);
    for(int i=1,pos=-1;i<=n;i++){
        int j=0;
        while(j<m) {
            pos=(pos+1)%n;
            if(vr[pos]==0)
                j++;
        }
        vr[pos]=i;
    }
    copy(vr.begin(),vr.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
}
#endif

//下面的一个算法是优化算法，特别巧妙：
int f1(int n, int m)
{
    int i, r = 0;
    for (i = 2; i <= n; i++)
        r = (r + m) % i;
    return r+1;
}
//这是一个递归的
int f2(int n, int m)
{
  if (n > 1)
    return (m + f(n - 1, m)) % m;
  else
    return 0;
}
// 其实这个两个看起来比较简单的算法是经过一些推导的。
//这儿（http://baike.baidu.com/view/717633.htm和http://wenwen.soso.com/z/q32613561.htm）有一些讲解，但是我现在还没有看明白，等以后明白之后再补上吧。




