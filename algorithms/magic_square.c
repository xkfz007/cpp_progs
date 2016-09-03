/*
output the magic square
this is from the internet
 */
#ifdef _MAGIC_SQUARE
#include<stdio.h>  
#include<math.h>  
#define MAX 30  
  
int a[MAX][MAX];    // 幻方矩阵  
int n,s;    // n:阶数，s:幻方数  
int x,y;  
int i,j,k;  
int total,m;  
int ox,oy;  
  
void main()  
{  
    void odd(int m, int index);  
    void singleEven();  
    void FourXFour();  
    void doubleEven();  
      
    do  
    {  
        printf("Please input n(3<=n[<=17]):\t");    // 屏幕可显示的最大阶数为17  
        scanf("%d",&n);  
        if(n<3) continue;   // 幻方最小阶数  
  
        s=n*(pow(n,2)+1)/2; // 幻方数  
        printf("s=%d\n",s);  
  
        if(n%2==1){  
            // 奇阶幻方  
            ox=oy=0;  
            odd(n,0);   // 从1开始填写n阶幻方  
        }  
        else if(n%4==0)  
        {  
            // 双偶阶幻方  
            doubleEven();  
              
        }  
        else if(n%2==0)  
        {  
            // 单偶阶幻方  
            singleEven();  
              
        }  
        // 输出制作好的n阶幻方  
        for(i=0;i<n;i++)  
        {  
            s=0;  
            for(j=0;j<n;j++)  
                s+=a[i][j],printf("%4d",a[i][j]);  
            printf("\t=%d\n",s);  
        }  
  
        fflush(stdin);  // 清除多余或无效的输入  
    }while(1);  
}  
  
/* 奇数阶幻方 
最经典的填法是罗伯特法(楼梯法)，填写方法是这样： 
把1(或最小的数)放在第一行正中；按以下规律排列剩下的n×n-1个数：  
(1)每一个数放在前一个数的右上一格； 
(2)如果这个数所要放的格已经超出了顶行那么就把它放在底行，仍然要放在右一列； 
(3)如果这个数所要放的格已经超出了最右列那么就把它放在最左列，仍然要放在上一行； 
(4)如果这个数所要放的格已经超出了顶行且超出了最右列，那么就把它放在前一个数的下一行同一列的格内； 
(5)如果这个数所要放的格已经有数填入，处理方法同(4)。 
这种写法总是先向“右上”的方向，象是在爬楼梯。 
 
三阶幻方： 
 
   8   1   6 
   3   5   7 
   4   9   2    */  
  
// 解奇阶幻方的通用模块  
// m 为阶数  
// index 为起始标识  
void odd(int m, int index)  
{  
    x=m/2;  
    y=0;  
    for(i=index+1;i<=index+pow(m,2);i++)  
    {  
        a[oy+y][ox+x]=i;  
        if(i%m==0) y++;  
        else x++,y--;  
        // else x++,y+=2; Hourse法  
        x=(x%m+m)%m;  
        y=(y%m+m)%m;  
    }  
}  
  
/* 单偶阶幻方 
n为偶数，且不能被4整除 (n=6，10，14，18，22……；n=4k+2，k=1，2，3，4，5……) 
以n=10为例。这时，k=2 
(1) 把方阵分为A，B，C，D四个象限，这样每一个象限肯定是奇数阶。 
用楼梯法，依次在A象限，D象限，B象限，C象限按奇数阶幻方的填法填数。 
 
6阶幻方第一步： 
 
   8   1   6 | 26  19  24 
   3   5   7 | 21  23  25 
   4   9   2 | 22  27  20 
------------------------- 
  35  28  33 | 17  10  15 
  30  32  34 | 12  14  16 
  31  36  29 | 13  18  11 
 
(2) 在A象限的中间行、中间格开始，按自左向右的方向，标出k格。 
A象限的其它行则标出最左边的k格。 
将这些格，和C象限相对位置上的数，互换位置。 
 
6阶幻方第二步： 
 
  35*  1   6 | 26  19  24 
   3  32*  7 | 21  23  25 
  31*  9   2 | 22  27  20 
------------------------- 
   8* 28  33 | 17  10  15 
  30   5* 34 | 12  14  16 
   4* 36  29 | 13  18  11 
 
(3) 在B象限任一行的中间格，自右向左，标出k-1列。 
(注：6阶幻方由于k-1=0，所以不用再作B、D象限的数据交换) 
将B象限标出的这些数，和D象限相对位置上的数进行交换，就形成幻方。 
 
6阶幻方： 
 
  35   1   6 | 26  19* 24 
   3  32   7 | 21  23* 25 
  31   9   2 | 22  27* 20 
------------------------- 
   8  28  33 | 17  10* 15 
  30   5  34 | 12  14* 16 
   4  36  29 | 13  18* 11   */  
  
void singleEven()  
{  
    int temp;  
    // 步骤一  
    // A象限  
    ox=oy=0;  
    odd(n/2,pow(n/2,2)*0);  
    // D象限  
    ox=oy=n/2;  
    odd(n/2,pow(n/2,2)*1);  
    // B象限  
    ox=n/2,oy=0;  
    odd(n/2,pow(n/2,2)*2);  
    // C象限  
    ox=0,oy=n/2;  
    odd(n/2,pow(n/2,2)*3);  
    // 对已经按ADBC象限以奇阶方式填充的幻方做处理  
    m=(n-2)/4;  
    for(i=0;i<n/2;i++)  
    {     
        // 步骤二  
        for(j=0;j<m;j++)  
        {  
            k=(i==n/4)?n/4+j:j;  
            temp=a[i][k];  
            a[i][k]=a[i+n/2][k];  
            a[i+n/2][k]=temp;  
  
        }  
        // 步骤三  
        for(j=0;j<m-1;j++)  
        {  
            k=n/2+n/4+j;  
            temp=a[i][k];  
            a[i][k]=a[i+n/2][k];  
            a[i+n/2][k]=temp;  
        }  
    }  
}  
  
/* 双偶阶幻方 
n为偶数，且能被4整除 (n=4，8，12，16，20……；n=4k，k=1，2，3，4，5……) 
互补：如果两个数字的和，等于幻方最大数和最小数的和，即 n*n+1，称为互补。*/  
  
/* 四阶幻方 
将数字从左到右、从上到下按顺序填写： 
 
   1   2   3   4 
   5   6   7   8 
   9  10  11  12 
  13  14  15  16 
 
将对角线上的数字，换成与它互补的数字。 
这里，n×n+1 = 4×4+1 = 17； 
把1换成17-1 = 16； 
把6换成17-6 = 11； 
把11换成17-11 = 6； 
把16换成17-16 = 1； 
…… 
换完后就是一个四阶幻方。 
 
  16*  2   3  13* 
   5  11* 10*  8 
   9   7*  6* 12 
   4* 14  15   1* */  
  
void FourXFour()  
{  
    // 对已填写数字的4阶幻方进行对角线互补替换  
    for(i=0;i<4;i++)  
    {  
        a[oy+i][ox+i]=total-a[oy+i][ox+i];  
        a[oy+i][ox+(4-i-1)]=total-a[oy+i][ox+(4-i-1)];  
    }  
}  
  
/* 对于n=4k阶幻方，我们先把数字按顺序填写。 
 
   1   2   3   4   5   6   7   8 
   9  10  11  12  13  14  15  16 
  17  18  19  20  21  22  23  24 
  25  26  27  28  29  30  31  32 
  33  34  35  36  37  38  39  40 
  41  42  43  44  45  46  47  48 
  49  50  51  52  53  54  55  56 
  57  58  59  60  61  62  63  64 
 
写好后，按4*4把它划分成k*k个方阵。 
因为n是4的倍数，一定能用4*4的小方阵分割。 
 
   1   2   3   4 |  5   6   7   8 
   9  10  11  12 | 13  14  15  16 
  17  18  19  20 | 21  22  23  24 
  25  26  27  28 | 29  30  31  32 
--------------------------------- 
  33  34  35  36 | 37  38  39  40 
  41  42  43  44 | 45  46  47  48 
  49  50  51  52 | 53  54  55  56 
  57  58  59  60 | 61  62  63  64 
 
然后把每个小方阵的对角线上的数字换成互补的数字，就构成幻方。 
 
  64*  2   3  61*| 60*  6   7  57* 
   9  55* 54* 12 | 13  51* 50* 16 
  17  47* 46* 20 | 21  43* 42* 24 
  40* 26  27  37*| 36* 30  31  33* 
--------------------------------- 
  32* 34  35  29*| 28* 38  39  25* 
  41  23* 22* 44 | 45  19* 18* 48 
  49  15* 14* 52 | 53  11* 10* 56  
   8* 58  59   5*|  4* 62  63   1* */  
  
void doubleEven()  
{  
    // 填写数字  
    x=y=0;  
    for(i=1;i<=pow(n,2);i++)  
    {  
        a[y][x]=i;  
        if(i%n==0) x=0,y++;  
        else x++;  
    }  
  
    total=1+pow(n,2);   // 最大数和最小数的和  
  
    // 以 4x4 大小分割幻方  
    m=n/4;  
    x=y=0;  
    ox=oy=0;  
    for(k=1;k<=pow(m,2);k++)  
    {  
        // 对每个 4x4 幻方做对角互补替换  
        FourXFour();  
        if(k%m==0) ox=0,oy+=4;  
        else ox=k%m*4;  // 转移到下一个 4x4 幻方  
    }  
}  

//
//在matlab中可以通过magic函数得到魔方阵，比如magic(5)就是5阶魔方阵。看matlab中magic.m代码可以发现，它的实现也是分为三种情况的。
//
//function M = magic(n)
//%MAGIC  Magic square.
//%   MAGIC(N) is an N-by-N matrix constructed from the integers
//%   1 through N^2 with equal row, column, and diagonal sums.
//%   Produces valid magic squares for all N > 0 except N = 2.
//
//%   Copyright 1984-2002 The MathWorks, Inc.
//%   $Revision: 5.15 $  $Date: 2002/04/15 03:44:23 $
//
//% Historically, MATLAB's magic was a built-in function.
//% This M-file uses a new algorithm to generate the same matrices.
//
//n = floor(real(double(n(1))));
//
//% Odd order.
//if mod(n,2) == 1
//   [J,I] = meshgrid(1:n);
//   A = mod(I+J-(n+3)/2,n);
//   B = mod(I+2*J-2,n);
//   M = n*A + B + 1;
//
//% Doubly even order.
//elseif mod(n,4) == 0
//   [J,I] = meshgrid(1:n);
//   K = fix(mod(I,4)/2) == fix(mod(J,4)/2);
//   M = reshape(1:n*n,n,n)';
//   M(K) = n*n+1 - M(K);
//
//% Singly even order.
//else
//   p = n/2;
//   M = magic(p);
//   M = [M M+2*p^2; M+3*p^2 M+p^2];
//   if n == 2, return, end
//   i = (1:p)';
//   k = (n-2)/4;
//   j = [1:k (n-k+2):n];
//   M([i; i+p],j) = M([i+p; i],j);
//   i = k+1;
//   j = [1 i];
//   M([i; i+p],j) = M([i+p; i],j);
//end


#endif
