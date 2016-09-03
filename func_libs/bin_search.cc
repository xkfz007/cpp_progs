
//#define _BIN_SEARCH
#ifdef _BIN_SEARCH
int bin_search(int x,int v[],int n){
    int low,high,mid;
    low=0;
    high=n-1;
    while(low<=high){
        mid=(low+high)>>1;
        if(x<v[mid])
            high=mid-1;
        else if(x>v[mid])
            low=mid+1;
        else
            return mid;
    }
    return -1;
}

int bin_search_v2(int x,int v[],int n){
    int low,high,mid;
    low=0;
    high=n-1;
    mid=(low+high)/2;
    while(low<=high&&x!=v[mid]){
        if(x<v[mid])
            high=mid-1;
        else
            low=mid+1;
        mid=(low+high)/2;
    }
    if(x==v[mid])
        return mid;
    else
        return -1;                            
}
#endif
#define _BINSEARCH
#ifdef _BINSEARCH
#include <iostream>

//#include <algorithm>

#include <string>

using namespace std;

//this is the function from the C++ std lib
template<class T, class RandomAccessIterator>
RandomAccessIterator lower_bound (RandomAccessIterator first,
                                  RandomAccessIterator last,
                                  const T& value)
{
    if (first == last)
        return first;
    RandomAccessIterator middle = first + (last - first) / 2;
    if (*middle < value)
        return lower_bound (middle + 1, last, value);
    return lower_bound (first, middle, value);
} // lower_bound


template<class T>
bool binary_search (T* first,
                    T* last,
                    const T& value)
{
    if (first == last)
        return false;
    T* middle = first + (last - first) / 2;
    if (*middle < value)
        return binary_search (middle + 1, last, value);
    else if (*middle > value)
        return binary_search (first, middle, value);
    return true;
} // binary_search

/*
template<class T>
bool binary_search (T* a, int first, int last, const T& value)
{
   if (first == last)
       return false;
   int middle = first + (last - first) / 2;
   if (a [middle] < value)
      return binary_search (a, middle + 1, last, value);
   else if (value < a [middle])
      return binary_search (a, first, middle, value);
   return true;
}
*/
int main()
{
   const string CLOSE_WINDOW_PROMPT =
      "Please press the Enter key to close this output window.";

   int a [9] = {7, 22, 84, 106, 117, 200, 494, 555, 702};

   string b [10] = {"abe", "ben", "carol", "dave", "ed", "frank",
                    "gerri", "helen", "iggy", "joan"};

   cout << *lower_bound (a, a + 9, 111) << endl;
   if (*lower_bound (a, a + 9, 111) == 111)
      cout << "111 found" << endl;
   else
      cout << "111 not found" << endl;
   cout << *lower_bound (a, a + 9, 494) << endl;
   cout << binary_search (a, a + 9, 111) << endl;
   cout << binary_search (a, a + 9, 494) << endl;

 //  cout << binary_search (a, 0, 9, 111) << endl;
 //  cout << binary_search (a, 0, 9, 494) << endl;

   string* sPtr = lower_bound (b, b + 10, "joan");
   cout << (sPtr != (b + 10) && !("joan" < *sPtr)) << endl;
   cout << *lower_bound (b, b + 10, "aa") << endl;

   cout << "no cast " << binary_search (b, b + 10, string("ken")) << endl;
   cout << "cast " << binary_search (b, b + 10, (string)"ken") << endl;
   string* sPtr2 = lower_bound (b, b + 10, "ken");
   if (sPtr2 != (b + 10) && !("ken" < *sPtr2))
      cout << "ken found" << endl;
   else
      cout << "ken not found" << endl;
   cout << binary_search (b, b + 10, string("joan")) << endl;
   cout << binary_search (b, b + 10, string("jo")) << endl;

 //  cout << binary_search (b, 0, 10, (string)"joan") << endl;
 //  cout << binary_search (b, 0, 10, (string)"jo") << endl;



   cout << endl << endl << CLOSE_WINDOW_PROMPT;
   cin.get();
   return 0;
} // main

#endif
