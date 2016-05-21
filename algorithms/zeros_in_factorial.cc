/*
Compute the number of trailing zeros in factorial(N) for any 32-bit integer N > 0. 
The num_zeros() function, below, is easily adaptable to support computation 
of # of trailing zeros factorial of any 64-bit integer. Some results are shown below:
Fac(1) = 1 has 0 trailing zeros
Fac(2) = 2 has 0 trailing zeros
Fac(5) = 120 has 1 trailing zeros
Fac(9) = 362880 has 1 trailing zeros
Fac(10) = 3628800 has 2 trailing zeros
Fac(14) = 87178291200 has 2 trailing zeros
Fac(15) = 1307674368000 has 3 trailing zeros
Fac(19) = 121645100408832000 has 3 trailing zeros
Fac(20) = 2432902008176640000 has 4 trailing zeros

Fac(1000) has 249 trailing zeros
Fac(1000000) has 249998 trailing zeros
*/

//#define _ZEROS_IN_FACTORIAL
#ifdef _ZEROS_IN_FACTORIAL
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

int count_fac(int factor, int num)
{
    int count_fac = 0;
    while (num / factor == num / float(factor))
    {
        num = num / factor;
        count_fac ++;
    }
    return count_fac;
}

int num_zeros(int num)
{
    int count_fac_2 = 0;
    int count_fac_5 = 0;
    for (int x=1; x<=num; x++)
    {
        count_fac_5 += count_fac(5, x);
        count_fac_2 += count_fac(2, x);
    }
    
    return min(count_fac_2, count_fac_5);
}

uint64_t fac(int num)
{
    if (num > 1)
        return num * fac(num - 1);
    return num;
}

int main()
{
    for (int num=1; num<=20; num++)
        cout << "Fac(" << num << ") = " 
        << fac(num) << " has " << num_zeros(num) 
             << " trailing zeros" << endl;

    vector<int> large_nums = {1000, 1000000, 1000000000};
    for (int large_num: large_nums)
        cout << "Fac(" << 
        large_num << ") has " << num_zeros(large_num) 
             << " trailing zeros" << endl;
}
#endif