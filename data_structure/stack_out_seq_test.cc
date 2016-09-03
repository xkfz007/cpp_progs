/*
 * stack_out_seq_test.cc
 *
 *  Created on: 2016-5月-23 上午12:18:36
 *      Author: Felix
 */

/*
 * 判断出栈序列是否合法
 * 【问题描述】对于一个栈，已知元素的进栈序列，判断一个由栈中所有元素组成的排列是否是可能的出栈序列。
 * 比如，进栈序列为1 2 3 4，则可能的出栈序列有4 3 2 1，1 4 3 2等。而1 4 2 3就不是。
 * 【输入形式】从标准输入读取第一行是一个整数N（3≤N≤10），代表有N个元素，其进栈序列是1 2 3 …… N。
 * 第二行是空格分隔的1~N的数字的一个排列。
 * 【输出形式】向标准输出打印结果。如果该排列是可能的出栈序列，则打印“YES”，
 * 否则打印“NO”。在行末要输出一个回车符。
 * 【输入样例】
 * 3
 * 3 1 2
 * 【输出样例】
 * NO
 * 【样例说明】进栈序列为1 2 3的出栈序列里没有3 1 2
 * 判断标准：出栈序列中，元素i之后所有比i小的元素间必须是降序排列的。
 */

#include <stdio.h>
#include <stdlib.h>
int main1()
{
    int i,j,k,N;
    int temp;//存放s[i]之后可能存在的第一个比s[i]小的
    int flag1;//标识s[i]之后是否存在比s[i]小的
    int flag2;//标识出栈序列是否非法
    int *s;
    scanf("%d",&N);
    s=malloc(N*sizeof(int));//动态开辟长度为N的数组
    for(i=0;i<N;i++)
        scanf("%d",&s[i]);
    for(i=0;i<N;i++)
    {
        flag1=0;
        flag2=0;
        for(j=i+1;j<N;j++)//寻找s[i]之后第一个比s[i]小的
            if(s[j]<s[i])//s[i]之后存在比s[i]小的
            {
                temp=s[j];//s[j]是s[i]之后第一个比s[i]小的
                flag1=1;
                break;
            }
        if(flag1==1)
            for(k=j+1;k<N;k++)
                if(s[k]<s[i])//s[j]之后如果还有比s[i]小的数
                    if(s[k]<temp)//都必须小于它之前的一个比s[i]小的数
                        temp=s[k];//并更新temp
                    else//否则，出栈序列非法
                    {
                        flag2=1;
                        break;
                    }
        if(flag2==1)
        {
            printf("NO");
            goto EXIT;
        }
    }
    printf("YES");
EXIT:
    free(s);
    exit(0);//退出程序
}
#define LEN 20
int main2()
{
    int N;
    int arr[LEN];
    int arr2[LEN];
    int i,j,k,m,n;
    int count;
    int flag;
    printf("Enter the Integer N:");
    scanf("%d",&N);
    printf("Enter the Serial:");
    for(i=0;i<N;i++)
        scanf("%d",&arr[i]);

    for(i=0;i<N-1;i++)
    {
        k=0;
        //找到arr[i]之后的比其小的元素
        for(j=i+1;j<N;j++)
        {
            if(arr[j]<arr[i])
                arr2[k++]=arr[j];
        }
        count=k;
        //判断arr[i]之后的比其小的元素是否按降序排列
        for(j=0;j<count-1;j++)
        {
            for(k=j+1;k<count;k++)
                if(arr2[k]>arr2[j])
                {
                    printf("No\n");
                    return  0;
                }
        }
    }
    printf("Yes\n");
    return 0;
}



