/* Fig. F.3: figF_03.c
   The merge sort algorithm. */
//#define _SORT_MERGE
#ifdef _SORT_MERGE
#define SIZE 10
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* function prototypes */
void mergeSort( int array[], int length );
void sortSubArray( int array[], int low, int high );
void merge( int array[], int left, int middle1, int middle2, int right );
void displayElements( int array[], int length );
void displaySubArray( int array[], int left, int right );

int main( void )
{
   int array[ SIZE ]; /* declare the array of ints to be sorted */
   int i; /* int used in for loop */
    
   srand( time( NULL ) ); /* seed the rand function */
    
   for ( i = 0; i < SIZE; i++ )
      array[ i ] = rand() % 90 + 10; /* give each element a value */
        
   printf( "Unsorted array:\n" );
   displayElements( array, SIZE ); /* print the array */
   printf( "\n\n" );
   mergeSort( array, SIZE ); /* merge sort the array */
   printf( "Sorted array:\n" );
   displayElements( array, SIZE ); /* print the array */
   return 0;
} /* end function main */

/* function that merge sorts the array */
void mergeSort( int array[], int length )
{
   sortSubArray( array, 0, length - 1 );
} /* end function mergeSort */

/* function that sorts a piece of the array */
void sortSubArray( int array[], int low, int high )
{
   int middle1, middle2; /* ints that record where the array is split */
    
   /* test base case: size of array is 1 */
   if ( ( high - low ) >= 1 ) { /* if not base case... */
      middle1 = ( low + high ) / 2;
      middle2 = middle1 + 1;

      /* output split step */
      printf( "split:   " );
      displaySubArray( array, low, high );
      printf( "\n         " );
      displaySubArray( array, low, middle1 );
      printf( "\n         " );
      displaySubArray( array, middle2, high );
      printf( "\n\n" );

      /* split array in half and sort each half recursively */
      sortSubArray( array, low, middle1 ); /* first half */
      sortSubArray( array, middle2, high ); /* second half */

      /* merge the two sorted arrays */
      merge( array, low, middle1, middle2, high );
   } /* end if */
} /* end function sortSubArray */

/* merge two sorted subarrays into one sorted subarray */
void merge( int array[], int left, int middle1, int middle2, int right )
{
   int leftIndex = left; /* index into left subarray */
   int rightIndex = middle2; /* index into right subarray */
   int combinedIndex = left; /* index into temporary array */
   int tempArray[ SIZE ]; /* temporary array */
   int i; /* int used in for loop */

   /* output two subarrays before merging */
   printf( "merge:   " );
   displaySubArray( array, left, middle1 );
   printf( "\n         " );
   displaySubArray( array, middle2, right );
   printf( "\n" );

   /* merge the subarrays until the end of one is reached */
   while ( leftIndex <= middle1 && rightIndex <= right ) {
      /* place the smaller of the two current elements in result */
      /* and move to the next space in the subarray */
      if ( array[ leftIndex ] <= array[ rightIndex ] )
         tempArray[ combinedIndex++ ] = array[ leftIndex++ ];
      else
         tempArray[ combinedIndex++ ] = array[ rightIndex++ ];
   } /* end while */
    
   if ( leftIndex == middle2 ) { /* if at end of left subarray ... */
      while ( rightIndex <= right ) /* copy the right subarray */
         tempArray[ combinedIndex++ ] = array[ rightIndex++ ];
   } /* end if */
   else { /* if at end of right subarray... */
      while ( leftIndex <= middle1 ) /* copy the left subarray */
         tempArray[ combinedIndex++ ] = array[ leftIndex++ ];
   } /* end else */
    
   /* copy values back into original array */
   for ( i = left; i <= right; i++ )
      array[ i ] = tempArray[ i ];
    
   /* output merged subarray */
   printf( "         " );
   displaySubArray( array, left, right );
   printf( "\n\n" );
} /* end function merge */

/* display elements in array */
void displayElements( int array[], int length )
{
   displaySubArray( array, 0, length - 1 );
} /* end function displayElements */

/* display certain elements in array */
void displaySubArray( int array[], int left, int right )
{
   int i; /* int used in for loop */
    
   /* output spaces for alignment */
   for ( i = 0; i < left; i++ )
      printf( "   " );
    
   /* output elements left in array */
   for ( i = left; i <= right; i++ )
      printf( " %d", array[ i ] );
} /* end function displaySubArray */
#endif
#if MERGE_CC
/*
 * 原地归并算法
 * 归并排序算法(mergesort)是将一个序列划分为同样大小的两个子序列,然后对两个子序列分别进行排序,最后进行合并操作,将两个子序列合成有序的序列.
 * 在合成的过程中,一般的实现都需要开辟一块与原序列大小相同的空间,以进行合并操作。
 * 这儿有比较全的排序算法：http://www.cnblogs.com/xkfz007/archive/2012/07/01/2572017.html
 * 之前使用过的归并排序，都是需要重新开辟新空间的。一般需要开辟的空间为n(数组大小），所以空间复杂度是O(n).
 * 其实可以对于归并可以不需要开辟新的空间就可以直接进行归并的方法，即原地归并。
 * 其 主要思想就是，因为需要归并的部分是有序的，所以需要找到两个序列中第二个序列小于第一个序列的部分，
 * 比如对序列[<1, 3, 5, 7>, <2, 4, 6, 8>]进行归并操作。2就是第二个序列中小于第一个序列中的<3,5,7>部分，需要调节<3,5,7>，与2的位置。
 * 然 后不断的进行扫描。其中进行调整的时候，用到的是对序列的循环移动，循环左移或循环右移。这样就不需要新的空间了。
 */

void rotate_right(int *v,int sz,int n){
    reverse(v,v+sz);
    reverse(v,v+n);
    reverse(v+n,v+sz);
}
void merge(int *v,int sz,int pos){
    int first=0,second=pos;
    while(first<second&&second<sz){
        while(first<second&&v[first]<=v[second])
            first++;
        int n=0;
        while(second<sz&&v[first]>v[second])
            n++,second++;
        rotate_right(v+first,second-first,n);
        first+=n;
    }
}


其中rotate_right是利用STL中的算法reserve实现的，其实也可以自己实现，很简单的。
之前总结了归并排序的三种方法：递归，非递归，自然归并。现在重新利用新的merge算法将原来的代码重新写一遍。
C++代码如下：
void merge_sort1(int *v,int sz){
    if(sz<=1)
        return ;
    merge_sort1(v,sz/2);
    merge_sort1(v+sz/2,sz-sz/2);
    merge(v,sz,sz/2);
}
void merge_pass(int* v,int s,int n){
    int i=0;
    while(i+2*s-1<n){
        merge(v+i,2*s,s);
        i+=2*s;
    }
    if(i+s<n)
        merge(v+i,n-i,i+s);
}
void merge_sort2(int*v ,int n){
    int s=1;
    while(s<n){
        merge_pass(v,s,n);
        s+=s;
    }
}
void merge_sort3(int *v,int n){
    int *pos=new int[n];
    int ct=0;
    for(int i=0;i<n-1;i++)
        if(v[i]>v[i+1])
            pos[ct++]=i;
    pos[ct++]=n-1;
    print(pos,n);
    if(ct>1){
        int k=0;
        int m=pos[k++];
        int r;
        while(k<ct){
            r=pos[k++];
            merge(v,r+1,m+1);
            m=r;
        }
    }
    delete[] pos;
}

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
using namespace std;
void print(int *v,int sz){
    copy(v,v+sz,ostream_iterator<int>(cout," "));
    cout<<endl;

}
int main(){
    const int N=20;
    int a[N];
//    srand(time(0));
    for(int i=0;i<N;i++)
        a[i]=rand()%100;
    print(a,N);
    merge_sort3(a,N);
    print(a,N);
}

#endif
