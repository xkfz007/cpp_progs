/*
 *    Description: 利用多次反转字符串来达到对字符串的循环移位。 
 */
#ifdef _STR_SHIFT
#include <stdio.h>  
#include <string.h>  
//反转字符串，把st与ed所指向的中间的内容反转（包含st不包含ed）  
void str_rev(char* st, char *ed)  
{  
for (--ed; st < ed; ++st, --ed)  
{  
char c;  
c = *st;
*st = *ed;
*ed = c;  
}  
}  
//用三反转等效左移字符串（st与ed之间，包含st不包含ed的内容）  
//n是进行循环移位的位数
char* str_shl(char* st, char* ed, int n)  
{  
str_rev(st, &st[n]);  
str_rev( &st[n], ed);  
str_rev(st, ed);  
return st;  
}  
int main()  
{  
char str[] = "abcdefghijklmnopqrstuvwxyz";  

puts( str_shl(str, str + strlen(str), 6) );  
return 0;  
}  
#endif