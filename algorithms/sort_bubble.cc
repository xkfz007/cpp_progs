/*
 * sort_bubble.cc
 *
 *  Created on: 2016-5月-22 下午10:40:31
 *      Author: Felix
 */

/*
 * 鸡尾酒排序, 是冒泡排序的改进版, 从两边分别排序, 每次是将未排序序列中的最大和最小的数移到正确的位置.
 */

template <class T>
void cocktail(T arr[],int n){
    int left=0,right=n-1;
    while(left<right){
        for(int i=left;i<right;i++){
            if(arr[i]>arr[i+1])
                swap(arr[i],arr[i+1]);
        }
        right--;
        for(int i=right;i>left;i--)
            if(arr[i]<arr[i-1])
                swap(arr[i],arr[i-1]);
        left++;
    }
}



