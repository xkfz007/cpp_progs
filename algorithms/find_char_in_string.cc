/*
 * find_char_in_string.cc
 *
 *  Created on: 2016-5月-22 下午8:16:28
 *      Author: Felix
 */
/*
 * 判断一个字符串中的字符是否都在另一个中出现
 * 假设这有一个各种字母组成的字符串，假设这还有另外一个字符串，而且这个字符串里的字母数相对少一些。从算法是讲，什么方法能最快的查出所有小字符串里的字母在大字符串里都有？
 * 解答：
 * （1）把其中的每个字母都放入一个Hashtable里(成本是O(n)或16次操作)。然后轮询第二个字串，在Hashtable里查询每个字母，看能否找到。如果找不到，说明没有匹配成功。
 * （2） 假设我们有一个一定个数的字母组成字串 —— 我给每个字母分配一个素数，从2开始，往后类推。这样A将会是2，B将会是3，C将会是5，等等。现在我遍历第一个字串，把每个字母代表的素数相乘。你最 终会得到一个很大的整数，对吧？然后 —— 轮询第二个字符串，用每个字母除它。如果除的结果有余数，这说明有不匹配的字母。如果整个过程中没有余数，你应该知道它是第一个字串恰好的子集了。
 * （3）共有26个英文字母，如果在字符串中该字母出现，就为1，否则为0，则用两个整型数（ int）可以表示 这两个字符串。
 *
 * 假设 int A,B;
 * C=A|B(按位或）
 * D=A^C(按位异或）
 * 如果D为0那么说明B中的字符串在A中都出现，如果D不为1，那么说明B中有A中未出现的字符串。
 */
#ifdef _FIND_CHAR_IN_STRING
#include <stdio.h>
#include <stdlib.h>
int convert(char *str)
{
    int A=0;
    char s;
    int pos=0;
    while(*str!='\0')
    {
        s=tolower(*str);
        pos=s-'a';
        A|=(1<<pos);
        str++;
    }
    return A;
}
int main(int argc,char*argv[])
{
    int A,B,C,D;
    char *str1,*str2;
    if(argc<2)
    {
        printf("Usage:test_str str1 str2\n");
        exit(0);
    }
    str1=argv[1];
    str2=argv[2];
    A=convert(str1);
    B=convert(str2);
    printf("A=0x%x  B=0x%x\n",A,B);
    C=A|B;
    printf("C=0x%x\n",C);
    D=A^C;
    printf("D=0x%x\n",D);
    D==0?printf("%s all in %s\n",str2,str1):printf("%s not all in %s\n",str2,str1);

    return 0;
}

#endif
