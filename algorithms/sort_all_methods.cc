/*
 * sort_all_methods.cc
 *
 *  Created on: 2016-5月-22 下午11:11:05
 *      Author: Felix
 */
/*
 * 常见的排序算法
 * 总结一下常见的排序算法。
 * 排序分内排序和外排序。
 * 内排序:指在排序期间数据对象全部存放在内存的排序。
 * 外排序:指在排序期间全部对象个数太多,不能同时存放在内存,必须根据排序过程的要求,不断在内、外存之间移动的排序。
 * 内排序的方法有许多种,按所用策略不同,可归纳为五类:插入排序、选择排序、交换排序、归并排序、分配排序和计数排序。
 * 插入排序主要包括直接插入排序，折半插入排序和希尔排序两种;
 * 选择排序主要包括直接选择排序和堆排序;
 * 交换排序主要包括冒泡排序和快速排序;
 * 归并排序主要包括二路归并(常用的归并排序)和自然归并。
 * 分配排序主要包括箱排序和基数排序。
 * 计数排序就一种。
 * 稳定排序:假设在待排序的文件中,存在两个或两个以上的记录具有相同的关键字,在用某种排序法排序后,若这些相同关键字的元素的相对次序仍然不变,则这种排序方法是稳定的。
 * 其中冒泡,插入,基数,归并属于稳定排序;选择,快速,希尔,堆属于不稳定排序。
 */
#include <iostream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <list>
using namespace std;

#define _FUNC cout<<__FUNCTION__<<endl
#define print(arr,n) copy(arr,arr+n,ostream_iterator<int>(cout," "));\
    cout<<endl;
using namespace std;
const char *const red = "\033[0;40;31m";
const char *const green= "\033[0;40;32m";
const char *const normal = "\033[0m";
/*
 * 1. 直接插入排序
 * 直接插入排序(straight insertion sort)的作法是：每次从无序表中取出第一个元素，把它插入到有序表的合适位置，使有序表仍然有序。
 * 第一趟比较前两个数,然后把第二个数按大小插入到有序表中; 第二趟把第三个数据与前两个数从后向前扫描，把第三个数按大小插入到有序表中；依次进行下去，进行了(n-1)趟扫描以后就完成了整个排序过程。
 * 直接插入排序属于稳定的排序，时间复杂性为o(n^2)，空间复杂度为O(1)。
 * 直接插入排序是由两层嵌套循环组成的。外层循环标识并决定待比较的数值。 内层循环为待比较数值确定其最终位置。直接插入排序是将待比较的数值与它的前一个数值进行比较，所以外层循环是从第二个数值开始的。
 * 当前一数值比待比较数 值大的情况下继续循环比较，直到找到比待比较数值小的并将待比较数值置入其后一位置，结束该次循环。（从小到大）
 * 值得注意的是，我们必需用一个存储空间来保存当前待比较的数值，因为当一趟比较完成时，我们要将待比较数值置入比它小的数值的后一位。插入排序类似玩牌时整理手中纸牌的过程。
 */
void insert_sort(int a[],int n)
{
    _FUNC;
    for(int i=1;i<n;i++) {
        int t=a[i];
        int j;
        for(j=i-1;j>=0&&a[j]>t;j--) {
                a[j+1]=a[j];
        }
        a[j+1]=t;
        print(a,n);
    }

}
/*
 * 2. 折半插入排序
 * 折 半插入排序（binary insertion sort）是对插入排序算法的一种改进，由于排序算法过程中，就是不断的依次将元素插入前面已排好序的序列中。由于前半部分为已排好序的数列，这样我们不 用按顺序依次寻找插入点，可以采用折半查找的方法来加快寻找插入点的速度。
 * 折半插入排序算法的具体操作为：在将一个新元素插入已排好序的数组的过程中，寻找插入点时，将待插入区域的首元素设置为a[low],末元素设置为 a[high]，则轮比较时将待插入元素与a[m],其中m=(low+high)/2相比较,如果比参考元素小，则选择a[low]到a[m-1]为新 的插入区域(即high=m-1)，否则选择a[m+1]到a[high]为新的插入区域（即low=m+1），如此直至low<=high不成 立，即将此位置之后所有元素后移一位，并将新元素插入a[high+1]。
 * 折半插入排序算法是一种稳定的排序算法，比直接插入算法明显减少了关键字之间比较的次数，因此速度比直接插入排序算法快，但记录移动的次数没有变，所以折半插入排序算法的时间复杂度仍然为O(n^2)，与直接插入排序算法相同。
 */
void binary_insert_sort(int a[],int n){
    for(int i=1;i<n;i++){
        int low=0;
        int high=i-1;
        int t=a[i];
        int mid;
        while(low<=high){
            mid=(low+high)/2;
            if(t<a[mid])
                high=mid-1;
            else
                low=mid+1;
        }
        for(int j=i;j>mid;j--)
            a[j]=a[j-1];
        a[low]=t;

    }
}
/*
 * 3. 希尔排序
 * 希尔排序（Shell Sort）又叫做缩小增量排序（diminishing increment sort），是一种很优秀的排序法，算法本身不难理解，也很容易实现，而且它的速度很快。
 * 基本思想：
 * 先取一个小于n的整数d1作为第一个增量，把文件的全部记录分成d1个组。所有距离为dl的倍数的记录放在同一个组中。先在各组内进行直接插入 排序；然后，取第二个增量d2<d1重复上述的分组和排序，直至所取的增量dt=1(dt<dt-l<…<d2<d1)， 即所有记录放在同一组中进行直接插入排序为止。
 * 该方法实质上是一种分组插入方法。插 入排序（Insertion Sort）的一个重要的特点是，如果原始数据的大部分元素已经排序，那么插入排序的速度很快（因为需要移动的元素很少）。从这个事实我们可以想到，如果原 始数据只有很少元素，那么排序的速度也很快。－－希尔排序就是基于这两点对插入排序作出了改进。
 */

void shell_sort(int a[],int n)
{
    _FUNC;
    int gap=n/2;
    bool flag=true;
    while(gap>1||flag)
    {
        flag=false;
        for(int i=0;i+gap<n;i++)
            if(a[i]>a[i+gap])
            {
                swap(a[i],a[i+gap]);
                flag=true;
            }
        print(a,n);
        if(gap>1)
            gap/=2;
    }

}
void shell_sort2(int a[],int n){
//    _FUNC;
    int gap=n/2;
    while(gap>0){
        for(int i=gap;i<n;i++){
            int t=a[i];
            int j;
            for(j=i-gap;j>=0&&a[j]>t;j-=gap)
                a[j+gap]=a[j];
            a[j+gap]=t;
        }
        gap/=2;
    }
}
/*
 * 希尔排序shell
 * shell算法是D.L. Shell于1959年发明的，基本思想是：先比较近距离的元素，而不是像简单交换排序算法那样先比较相邻的元素。
 * 这样可以快速减少大量的无序情况，从而减轻后序的工作。被比较的元素之间的距离逐步减少，直到减少为1，这时编程了相邻元素的互换。
 */
void shell_sort3(int v[],int n){
    int gap,i,j,temp;
    for(gap=n/2;gap>0;gap/=2)
        for(i=gap;i<n;i++)
            for(j=i-gap;j>=0&&v[j]>v[j+gap];j-=gap){
                temp=v[j];
                v[j]=v[j+gap];
                v[j+gap]=temp;
            }
}

//void print_arr(int v[],int n){
//    int i;
//    for(i=0;i<n;i++)
//        printf("%4d",v[i]);
//    printf("\n");
//}
//int main(){
//    int i;
//    for(i=0;i<10;i++){
//        int arr[20],j,b[20];
//        for(j=0;j<20;j++)
//        {
//            int val=rand()%1000;
//            arr[j]=val;
//            b[j]=val;
//        }
//        printf("before:");
//        print_arr(b,20);
//        shellsort(arr,20);
//        printf("after :");
//        print_arr(arr,20);
//    }
//}

/*
 * 4. 直接选择排序
 * 排序是给每个位置选择当前元素最小的，比如给第一个位置选择最小的，在剩余元素里面给第二个元素选择第二小的，依次类推，直到第n-1个元素，第n个 元素不用选择了，
 * 因为只剩下它一个最大的元素了。那么，在一趟选择，如果当前元素比一个元素小，而该小的元素又出现在一个和当前元素相等的元素后面，那么 交换后稳定性就被破坏了。
 * 比较拗口，举个例子，序列5 8 5 2 9，我们知道第一遍选择第1个元素5会和2交换，那么原序列中2个5的相对前后顺序就被破坏了，所以选择排序不是一个稳定的排序算法。
 * 时间复杂度是 O(n^2)
 */

// 这个是最基本的：从中找出最小的然后和第一个数交换，再从第2到n-1中找出最小的和第二个数交换
void select_sort(int a[],int n)
{
    for(int i=0;i<n-1;i++)
    {
        int min=a[i];
        int index=i;
        for(int j=i+1;j<n;j++)
            if(a[j]<min)
            {
                min=a[j];
                index=j;
            }
        swap(a[i],a[index]);
    }
}
// 这儿感觉形式上有点类似下面的冒泡排序。
void select_sort2(int a[],int n)
{
    _FUNC;
    for(int i=n-1;i>0;i--){
        for(int j=0;j<i;j++)
            if(a[j]>a[i])
                swap(a[j],a[i]);
    }

}
//这是对方法二的改进，判断过程中是否有交换发生，如果没有交换，说明已经完成排序了。
void select_sort3(int a[],int n)
{
    _FUNC;
    bool flag=true;
    for(int i=n-1;i>0&&flag;i--){
        flag=false;
        for(int j=0;j<i;j++)
            if(a[j]>a[i])
                swap(a[j],a[i]),flag=true;
        print(a,n);
    }

}
/*
 * 5. 堆排序
 * 我们知道堆的结构是节点i的孩子为2*i和2*i+1节点，大顶堆要求父节点大于等于其2个子节点，小顶堆要求父节点小于等于其2个子节点。
 * 在一个长为n 的序列，堆排序的过程是从第n/2开始和其子节点共3个值选择最大(大顶堆)或者最小(小顶堆),这3个元素之间的选择当然不会破坏稳定性。
 * 但当为n /2-1, n/2-2, ...1这些个父节点选择元素时，就会破坏稳定性。有可能第n/2个父节点交换把后面一个元素交换过去了，
 * 而第n/2-1个父节点把后面一个相同的元素没 有交换，那么这2个相同的元素之间的稳定性就被破坏了。所以，堆排序不是稳定的排序算法。
 * 需要注意的是，如果使用数组表示堆的话，要从下标1开始，而不是从0开始。所以，这儿采用了一个技巧，让int*b=a-1;这样的话b[1]就相当于对原数组从0开始似的,即a[0]。
 *
 */
void adjust(int b[],int m,int n){
//    int *b=a-1;
    int j=m;
    int k=2*m;
    while(k<=n){
        if(k<n&&b[k]<b[k+1])
            k++;
        if(b[j]<b[k])
            swap(b[j],b[k]);
        j=k;
        k*=2;
    }
}
void heap_sort(int a[],int n){
    _FUNC;
    int *b=a-1;
    for(int i=n/2;i>=1;i--)
        adjust(b,i,n);
    for(int i=n-1;i>=1;i--){
        swap(b[1],b[i+1]);
        adjust(b,1,i);
    }
}
/*
 * 6. 冒泡排序
 * 冒 泡排序就是把小的元素往前调或者把大的元素往后调。比较是相邻的两个元素比较，交换也发生在这两个元素之间。所以，如果两个元素相等，是不用交换的；
 * 如果 两个相等的元素没有相邻，那么即使通过前面的两两交换把两个相邻起来，这时候也不会交换，所以相同元素的前后顺序并没有改 变，
 * 所以冒泡排序是一种稳定排序算法。
 */
void bubble_sort(int a[],int n)
{
    _FUNC;
    for(int i=n-1;i>0;i--)
        for(int j=0;j<i;j++)
            if(a[j]>a[j+1])
                swap(a[j],a[j+1]);

}
//下面的方法是加入了是否已经排好序的判断。
void bubble_sort2(int a[],int n)
{
    bool flag=true;
    for(int i=n-1;i>0&&flag;i--){
        flag=false;
        for(int j=0;j<i;j++)
            if(a[j]>a[j+1])
                swap(a[j],a[j+1]),flag=true;
    }

}
/*
 * 7. 快速排序
 * 快 速排序（Quicksort）是对冒泡排序的一种改进。由C. A. R. Hoare在1962年提出。
 * 它的基本思想是：通过一趟排序将要排序的数据分割成独立的两部分，其中一部分的所有数据都比另外一部分的所有数据都要小，
 * 然 后再按此方法对这两部分数据分别进行快速排序，整个排序过程可以递归进行，以此达到整个数据变成有序序列。
 * 快速排序有两个方向，左边的i下标一直往右走，当a[i] <= a[center_index]，其中center_index是中枢元素的数组下标，
 * 一般取为数组第0个元素。而右边的j下标一直往左走，当a[j] > a[center_index]。如果i和j都走不动了，i <= j,
 * 交换a[i]和a[j],重复上面的过程，直到i>j。交换a[j]和a[center_index]，完成一趟快速排序。在中枢元素和a[j]交 换的时候，
 * 很有可能把前面的元素的稳定性打乱，比如序列为 5 3 3 4 3 8 9 10 11，现在中枢元素5和3(第5个元素，下标从1开始计)交换就会把元素3的稳定性打乱，
 * 所以快速排序是一个不稳定的排序算法，不稳定发生在中枢元素和 a[j] 交换的时刻。
 */

//下面的代码中中枢元素采用的中间的元素：
void qsort(int a[],int l,int r){
    int pvt=a[(l+r)/2];
    int i=l,j=r;
    while(i<=j){
        while(a[i]<pvt)
            i++;
        while(a[j]>pvt)
            j--;
        if(i<=j){
            if(i!=j)
                swap(a[i],a[j]);
            i++;
            j--;
        }
    }
    if(j>l)
        qsort(a,l,j);
    if(i<r)
        qsort(a,i,r);
}
void quick_sort(int a[],int n){
    qsort(a,0,n-1);
}

/*
 * 8. 二路归并排序
 * 归并排序是把序列递归地分成短序列，递归出口是短序列只有1个元素(认为直接有序)或者2个序列(1次比较和交换),然后把各个有序的段序列合并成一个
 * 有 序的长序列，不断合并直到原序列全部排好序。可以发现，在1个或2个元素时，1个元素不会交换，2个元素如果大小相等也没有人故意交换，这不会破坏稳定 性。
 * 那么，在短的有序序列合并的过程中，稳定是是否受到破坏？没有，合并过程中我们可以保证如果两个当前元素相等时，我们把处在前面的序列的元素保存在结 果序
 * 列的前面，这样就保证了稳定性。所以，归并排序也是稳定的排序算法。
 */

//方法一：递归形式的归并排序
void merge(int a[],int b[],int l,int m,int r){
//    int *b=new int[r-l+1];
    int i,j,k;
    i=l;
    j=m+1;
    k=l;
    while(i<=m&&j<=r){
        if(a[i]<a[j])
            b[k++]=a[i++];
        else
            b[k++]=a[j++];
    }
    while(i<=m)
        b[k++]=a[i++];
    while(j<=r)
        b[k++]=a[j++];
    for(int s=l;s<=r;s++)
        a[s]=b[s];
//    delete[] b;
}
void msort(int a[],int b[],int l,int r){
    if(l<r){
        int m=(l+r)/2;
        msort(a,b,l,m);
        msort(a,b,m+1,r);
        merge(a,b,l,m,r);
    }
}
void merge_sort(int a[],int n){
    _FUNC;
    int *b=new int[n];
    msort(a,b,0,n-1);
    delete[] b;
}

//方法二：去除递归的方法
void merge_pass(int x[],int y[],int s,int n){
    int i=0;
    while(i+2*s-1<n){
        merge(x,y,i,i+s-1,i+2*s-1);
        i+=2*s;
    }
    if(i+s<n)
        merge(x,y,i,i+s-1,n-1);
    else
        for(int j=i;j<=n-1;j++)
            y[j]=x[j];
}
void merge_sort2(int a[],int n){
    _FUNC;
    int *b=new int [n];
    int s=1;
    while(s<n){
        merge_pass(a,b,s,n);
        s+=s;
        merge_pass(b,a,s,n);
        s+=s;
    }
    delete[] b;
}
/*
 * 9. 自然归并排序
 * 下面的两种形式是一样的，开始我先采用的vector来记录子序列的位置，后来发现其实采用一个数组就可以了。两种代码都放在这儿吧。
 */

//形式1：
void merge_sort3(int a[],int n){
    vector<int> st;
    for(int i=0;i<n-1;i++){
        if(a[i]>a[i+1])
            st.push_back(i);
    }
    st.push_back(n-1);
//    copy(st.begin(),st.end(),ostream_iterator<int>(cout," "));
//    cout<<endl;
    int *b=new int [n];
    int l,m,r;
    l=0;
    if(!st.empty())
    {
        m=st.front();
        st.erase(st.begin());
    }
    while(!st.empty()){
        r=st.front();
        st.erase(st.begin());
        merge(a,b,l,m,r);
//        print(a,n);
//        copy(st.begin(),st.end(),ostream_iterator<int>(cout," "));
//        cout<<endl;
        m=r;
    }
//    print(a,n);
    delete [] b;

}

//形式2：
void merge_sort4(int a[],int n){
    _FUNC;
    int *pos=new int[n];
    int k=0;
    for(int i=0;i<n-1;i++){
        if(a[i]>a[i+1])
            pos[k++]=i;
    }
    pos[k++]=n-1;
    int *b=new int [n];
    int l,m,r;
    l=0;
    int p=0;
    if(p<k)
        m=pos[p++];
    while(p<k){
        r=pos[p++];
        merge(a,b,l,m,r);
        m=r;
    }
    delete [] b;

}
/*
 * 10. 箱排序
 *
 */
#if 0
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <cstdlib>
#include <map>
using namespace std;
//下 面的程序采用一个简单的Node类来描述学生的姓名和成绩，采用STL中的list来实现箱子排序。同样进行随机生成了多个实例来测试程序的正确性。
//而所 采用的标准是STL中的multimap容器。因为这个容器可以自动根据关键字进行排序。本来想使用map容器，但是map容器不允许重复，而我们的测试 实例中有很多的重复元素。
class Node{
    friend ostream& operator<<(ostream& ,const Node &);
    public:
    Node(int s=0,string n="A"):score(s),name(n){}
    int operator!=(const Node & x)const{
        return score!=x.score||name!=x.name;
    }
    operator int()const{
        return score;
    }
    private:
    int score;
    string name;
};
ostream& operator<<(ostream& out,const Node& x){
    out<<"<"<<x.score<<","<<x.name<<">";
    return out;
}
void BinSort(list<Node>& x,int range){
    vector<list<Node> > bin(range+1);
    list<Node> y;
    list<Node>::const_iterator ite;
    for(ite=x.begin();ite!=x.end();ite++){
        bin[int(*ite)].push_back(*ite);
    }
    vector<list<Node> >::const_iterator ite2;
    for(ite2=bin.begin();ite2!=bin.end();++ite2){
        for(ite=ite2->begin();ite!=ite2->end();++ite)
            y.push_back(*ite);
    }
    x=y;
}
void bin_test(){
    int n=10;
    int range=10;
    for(int j=0;j<18;j++){
        list<Node> x;
        multimap<int,string> mp;
        for(int i=0;i<n;i++) {
            int score=rand()%(range+1);
            string name(1,'A'+i);
            Node nod(score,name);
            x.push_back(nod);
            mp.insert(pair<int,string>(score,name));
        }
        list<Node>::const_iterator ite;
        cout<<"orig:";
        for(ite=x.begin();ite!=x.end();ite++)
            cout<<*ite<<" ";
        cout<<endl;
        BinSort(x,range);
        cout<<"aftr:";
        for(ite=x.begin();ite!=x.end();++ite)
            cout<<*ite<<" ";
        cout<<endl;
        multimap<int,string>::const_iterator ite2;
        cout<<"map :";
        for(ite2=mp.begin();ite2!=mp.end();++ite2)
            cout<<"<"<<ite2->first<<","<<ite2->second<<"> ";
        cout<<endl;
        cout<<endl;
    }
}
int main(){
    bin_test();
}
#endif

/*
 * 11. 基数排序
 * 基数排序是按照低位先排序，然后收集；再按照高位排序，然后再收集；依次类推，直到最高位。有时候有些属性是有优先级顺序的，先按低优先级排序，
 * 再按高优 先级排序，最后的次序就是高优先级高的在前，高优先级相同的低优先级高的在前。基数排序基于分别排序，分别收集，所以其是稳定的排序算法。
 */

// 下面的一种方法是采用STL的链表容器list来实现的，这种实现比较直观：
void radix_sort2(int a[],int n){
    int bits=maxbits(a,n);
    list<int> x(a,a+n);
    int range=10;
    vector<list<int> > bin(range);
    list<int> y;
    list<int>::iterator ite;
    int adix=1;
    for(int i=0;i<bits;i++){
        for(ite=x.begin();ite!=x.end();ite++){
            int d=(*ite/adix)%10;
            bin[d].push_back(*ite);
        }
        vector<list<int> >::iterator ite2;
        y.clear();
        for(ite2=bin.begin();ite2!=bin.end();++ite2){
            for(ite=ite2->begin();ite!=ite2->end();++ite)
                y.push_back(*ite);
            ite2->clear();
        }
        x=y;
        adix*=10;
    }
    int i=0;
    for(ite=x.begin();ite!=x.end();ite++)
        a[i++]=*ite;
}

//另一种方法是采用多个数组来实现，不是很容易理解，这是参考的网上的代码。具体代码如下：
int maxbits(int a[],int n){
    int d=0;
    for(int i=0;i<n;i++){
        int b=1;
        int r=a[i];
        while(r/10>0){
            b++;
            r/=10;
        }
        if(d<b)
            d=b;
    }
    return d;
}
void radix_sort(int a[],int n){
    _FUNC;
    int d=maxbits(a,n);
    int *temp=new int[n];
    int *count=new int[10];
    int adix=1;
    for(int b=1;b<=d;b++){
        for(int i=0;i<10;i++)
            count[i]=0;
        for(int i=0;i<n;i++){
            int k=(a[i]/adix)%10;
            count[k]++;
        }
        for(int i=1;i<10;i++)
            count[i]+=count[i-1];
        for(int i=n-1;i>=0;i--){
            int k=(a[i]/adix)%10;
            count[k]--;
            temp[count[k]]=a[i];
        }
        for(int i=0;i<n;i++)
            a[i]=temp[i];
        adix*=10;
    }
    delete[] temp;
    delete[] count;
}


/*
 * 12.计数排序
 */
void rank(int arr[],int n,int r[])
{
    for(int i=0;i<n;i++)
        r[i]=0;
    for(int i=1;i<n;i++){
        for(int j=0;j<i;j++)
        {
            if(arr[j]<=arr[i])
                r[i]++;
            else
                r[j]++;
        }
    }
}

void rank_sort2(int a[],int n){
    int *r=new int[n];
    rank(a,n,r);
    int *u=new int[n];
    for(int i=0;i<n;i++)
        u[r[i]]=a[i];
    for(int i=0;i<n;i++)
        a[i]=u[i];
    delete[] r;
    delete[] u;
}
void rank_sort(int arr[],int n)
{
    int *r=new int[n];
    rank(arr,n,r);
    for(int i=0;i<n;i++)
    {
        while(r[i]!=i)
        {
            int t=r[i];
            swap(arr[i],arr[t]);
            swap(r[i],r[t]);
        }
    }
    delete[] r;
}

//排序算法复杂度：
//按平均时间将排序分为四类：
//（1）平方阶(O(n2))排序
//    　一般称为简单排序，例如直接插入、直接选择和冒泡排序；
//（2）线性对数阶(O(nlgn))排序
//    　如快速、堆和归并排序；
//（3）O(n1+￡)阶排序
//    　￡是介于0和1之间的常数，即0<￡<1，如希尔排序；
//（4）线性阶(O(n))排序
//    　如桶、箱和基数排序。
//不同条件下，排序方法的选择
//(1)若n较小(如n≤50)，可采用直接插入或直接选择排序。
//    　当记录规模较小时，直接插入排序较好；否则因为直接选择移动的记录数少于直接插人，应选直接选择排序为宜。
//(2)若文件初始状态基本有序(指正序)，则应选用直接插人、冒泡或随机的快速排序为宜；
//(3)若n较大，则应采用时间复杂度为O(nlgn)的排序方法：快速排序、堆排序或归并排序。
//    　快速排序是目前基于比较的内部排序中被认为是最好的方法，当待排序的关键字是随机分布时，快速排序的平均时间最短；
//    　堆排序所需的辅助空间少于快速排序，并且不会出现快速排序可能出现的最坏情况。这两种排序都是不稳定的。
//    　若要求排序稳定，则可选用归并排序。但本章介绍的从单个记录起进行两两归并的  排序算法并不值得提倡，通常可以将它和直接插入排序结合在一起使用。先利用直接插入排序求得较长的有序子文件，然后再两两归并之。因为直接插入排序是稳定 的，所以改进后的归并排序仍是稳定的。4)在基于比较的排序方法中，每次比较两个关键字的大小之后，仅仅出现两种可能的转移，因此可以用一棵二叉树来描述比较判定过程。
//    　当文件的n个关键字随机分布时，任何借助于"比较"的排序算法，至少需要O(nlgn)的时间。
//    　箱排序和基数排序只需一步就会引起m种可能的转移，即把一个记录装入m个箱子之一，因此在一般情况下，箱排序和基数排序可能在O(n)时间内完成对n个 记录的排序。但是，箱排序和基数排序只适用于像字符串和整数这类有明显结构特征的关键字，而当关键字的取值范围属于某个无穷集合(例如实数型关键字)时， 无法使用箱排序和基数排序，这时只有借助于"比较"的方法来排序。
//    　若n很大，记录的关键字位数较少且可以分解时，采用基数排序较好。虽然桶排序对关键字的结构无要求，但它也只有在关键字是随机分布时才能使平均时间达到 线性阶，否则为平方阶。同时要注意，箱、桶、基数这三种分配排序均假定了关键字若为数字时，则其值均是非负的，否则将其映射到箱(桶)号时，又要增加相应 的时间。
//(5)有的语言(如Fortran，Cobol或Basic等)没有提供指针及递归，导致实现归并、快速(它们用递归实现较简单)和基数(使用了指针)等排序算法变得复杂。此时可考虑用其它排序。
//(6)本章给出的排序算法，输人数据均是存储在一个向量中。当记录的规模较大时，为避免耗费大量的时间去移动记录，可以用链表作为存储结构。譬如插入排 序、归并排序、基数排序都易于在链表上实现，使之减少记录的移动次数。但有的排序方法，如快速排序和堆排序，在链表上却难于实现，在这种情况下，可以提取 关键字建立索引表，然后对索引表进行排序。然而更为简单的方法是：引人一个整型向量t作为辅助表，排序前令t[i]=i(0≤i<n)，若排序算法 中要求交换R[i]和R[j]，则只需交换t[i]和t[j]即可；排序结束后，向量t就指示了记录之间的顺序关系：
//        R[t[0]].key≤R[t[1]].key≤…≤R[t[n-1]].key
//  若要求最终结果是：
//       R[0].key≤R[1].key≤…≤R[n-1].key
//则可以在排序结束后，再按辅助表所规定的次序重排各记录，完成这种重排的时间是O(n)。


//未了测试代码的正确性，我们采取了随机生成10个序列，
//然后先使用C++STL中给出的排序算法sort来得到一个正确的排序，然后再使用我们的方法进行排序得到结果，通过对比这两者的结果来验证我们代码的正确性。
// 其中参数是要测试的方法，void (*_sort)(int*,int)是排序方法的指针，我们所有的排序方法都写成这种形式。
//void sort_test(void (*_sort)(int*,int)){
//    const int N=10;
//    int orig[N];
//    int standard[N];
//    int arr[N];
//    srand(time(0));
//    for(int j=0;j<15;j++){
//        for(int i=0;i<N;i++)
//            orig[i]=rand()%100;//随机生成序列
//        cout<<"bef:";
//        print(orig,N);
//
//        copy(orig,orig+N,standard);
//        sort(standard,standard+N);//利用sort函数进行排序
//        cout<<"std:";
//        print(standard,N);
//
//        copy(orig,orig+N,arr);
//        _sort(arr,N);//采用我们的方法进行排序
//        cout<<"aft:";
//        print(arr,N);
//        if(equal(standard,standard+N,arr))//测试我们的方法是否正确
//            printf("%sOK%s\n",green,normal);
//        else
//            printf("%sNO%s\n",red,normal);
//    }
//}
template <class T>
void sort_test(void (*_sort)(T*,int)){
    const int N=10;
    T orig[N];
    T standard[N];
    T arr[N];
    srand(time(0));
    for(int j=0;j<18;j++){
        for(int i=0;i<N;i++)
            orig[i]=rand()%100;
        cout<<"bef:";
        print(orig,N);

        copy(orig,orig+N,standard);
        sort(standard,standard+N);
        cout<<"std:";
        print(standard,N);

        copy(orig,orig+N,arr);
        _sort(arr,N);
        cout<<"aft:";
        print(arr,N);
        if(equal(standard,standard+N,arr))
            printf("%sOK%s\n",green,normal);
        else
            printf("%sNO%s\n",red,normal);
    }

}
//void clock_test()
//{
//    clock_t start,finish;
//    int step=100;
//    cout<<CLOCKS_PER_SEC<<endl;
//    for(int k=0;k<=1000;k+=step)
//    {
//
//        start=clock();
//    const int N=10000;
//    int arr[N];
//    srand(time(0));
//    for(int i=0;i<N;i++)
//        arr[i]=rand()%1000;
//    bubble_sort2(arr,N);
//    finish=clock();
//    cout<<k<<" "<<finish-start<<" "<<(finish-start)*1.0/CLOCKS_PER_SEC<<endl;
//    }
//}
template <class T>
void clock_test()
{
    void(*_sort[])(T*,int)={insert_sort,binary_insert_sort,shell_sort,shell_sort2,select_sort,
                           select_sort2,select_sort3,heap_sort,bubble_sort,bubble_sort2,quick_sort,
                           merge_sort,merge_sort2,merge_sort3,merge_sort4,rank_sort,rank_sort2};
    int len=sizeof(_sort)/sizeof(_sort[0]);
    clock_t start,finish;
    //    cout<<CLOCKS_PER_SEC<<endl;
    cout<<"鐩存帴鎻掑叆\t鎶樺崐鎻掑叆\t甯屽皵鎺掑簭\t"<<endl;
    int step=100;
    for(int k=100;k<=100000;k+=step)
    {
        const int N=k;
        T arr[N];
        double tm[N];
        //    srand(time(0));
        for(int i=0;i<N;i++)
            arr[i]=rand()%1000;
        for(int i=0;i<len;i++) {
            start=clock();
            _sort[i](arr,N);
            finish=clock();
            tm[i]=(finish-start)*1.0/CLOCKS_PER_SEC;
        }
        cout<<k<<'\t';
        for(int i=0;i<len;i++)
            cout<<tm[i]<<'\t';
        cout<<endl;
    }
}
template <class T>
void time_test()
{
    void(*_sort[])(T*,int)={insert_sort,binary_insert_sort,shell_sort,shell_sort2,select_sort,
                           select_sort2,select_sort3,heap_sort,bubble_sort,bubble_sort2,quick_sort,
                           merge_sort,merge_sort2,merge_sort3,merge_sort4,rank_sort,rank_sort2};
    int len=sizeof(_sort)/sizeof(_sort[0]);
    struct TIMEB ts1,ts2;
    time_t t_sec,t_ms,ti;
    cout<<"鐩存帴鎻掑叆\t鎶樺崐鎻掑叆\t甯屽皵鎺掑簭\t"<<endl;
    int step=100;
    for(int k=100;k<=100000;k+=step)
    {
        const int N=k;
        T arr[N];
        double tm[N];
        for(int i=0;i<N;i++)
            arr[i]=rand()%1000;
        cout<<k<<'\t';
        for(int i=0;i<len;i++) {
            ftime(&ts1);
            _sort[i](arr,N);
            ftime(&ts2);
            t_sec=ts2.time-ts1.time;
            t_ms=ts2.millitm-ts1.millitm;
            ti=t_sec*1000+t_ms;
            cout<<ti<<'\t';
        }
        cout<<endl;
    }
}

int test()
{
    const int N=10;
    int arr[N];
//    srand(time(0));
    for(int i=0;i<N;i++)
        arr[i]=rand()%100;
//    arr[0]=90;
    cout<<"bef:";
    print(arr,N);
//    rank_sort(arr,N);
//    select_sort2(arr,N);
//    bubble_sort2(arr,N);
//    insert_sort(arr,N);

//    radix_sort(arr,N);

//    int *b=arr-1;
//    for(int i=N/2;i>=1;i--) {
//        adjust(b,i,N);
//        print(arr,N);
//    }
//    cout<<"aft:";
//    print(arr,N);
    merge_sort3(arr,N);

    return 0;

}
int main()
{
   // clock_test();
//      test();
//    sort_test(radix_sort2);
    time_test<int>();

}


