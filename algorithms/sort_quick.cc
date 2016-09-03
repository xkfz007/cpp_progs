/*
 * sort_quick.cc
 *
 *  Created on: 2016-5月-22 下午10:35:57
 *      Author: Felix
 */
/*
 * 快速排序(递归与非递归形式)
 * 首先说明一下快速排序是对冒泡排序的改进。为什么这么说呢？想一下冒泡排序，它把序列分成了两部分，前半部分无序，后半部分升序排列，并且后半部分的数都大于前半部的数。
 * 由此可得到快速排序和冒泡排序的一些共同点：
 *     都要经历n趟排序
 *     每趟排序要经历O(n)次比较
 *     都是后半部分元素比前半部大
 * 而不同之处就在于冒泡排序的交换操作发生相邻的元素之间，即一趟排序可以要经过多次交换操作；快速排序的交换操作发生在间隔比较远的两个元素之间，一趟排序要经过交换操作次数会少一些。
 * 一般的快速排序采用的是递归的方式进行实现的，而且一般只要是递归的问题，都可以转化为非递归的实现，而实现的时候一般采用的是堆栈数据结构。
 * 快 排的主要思想就是在一个区间中选中一个中枢元素pivot，然后在该区间内为该中枢元素pivot找到正确的位置Mid，Mid左面的是比pivot小的 元素，Mid右面的是比pivot大的元素。然后再对分别对左面和右面的元素进行同样的操作。这其实是一个典型的分治法的思想。
 * 其实每次进行操作的时候，都可以找到一个区间的边界，然后对该区间进行递归调用，现在我们不是进行递归调用，而是将区间的边界存入到栈中，然后再不断的从栈中取出区间进行操作即可。直到，堆栈为空了，说明我们已经排序完毕。
 * 下面是递归和非递归的程序。其中partition方法完成了对pivot元素的位置的查找。而由这个函数返回的pivot的位置，我们就知道其他要操作的区间位置。
 * 注意这儿的partition写法不错，可以作为以后快排的标准写法。
 */
#include <stack>
#include <iostream>

template <class T>
int partition(T a[],int low,int high){
    T pivot=a[low];
    int i=low,j=high+1;
    while(true){
        do
            i++;
        while(a[i]<pivot);
        do
            j--;
        while(a[j]>pivot);
        if(i>=j)
            break;
        swap(a[i],a[j]);
    }
    swap(a[low],a[j]);
    return j;
}
template <class T>
void qsort2(T a[],int low,int high){
    if(low<high){
        int mid=partition(a,low,high);
        qsort2(a,low,mid-1);
        qsort2(a,mid+1,high);
    }
}
template <class T>
void quick_sort3(T a[],int n){
    stack<int> st;
    int low=0,high=n-1;
    int mid;
    st.push(low);
    st.push(high);
    while(st.size()>1){
        high=st.top();st.pop();
        low=st.top();st.pop();
        mid=partition(a,low,high);
        if(low<mid-1){
            st.push(low);
            st.push(mid-1);
        }
        if(mid+1<high){
            st.push(mid+1);
            st.push(high);
        }
    }
}




