/*
 *    Description: 检查当前系统类型是大端还是小端(intel) 
 */
#if 0
#include <stdio.h>
int checkSystem()
{
    union check
    {
        int i;
        char ch;
    }c;
    c.i=1;
    return (c.ch==1);
}
int checkSystem2()
{
    int i=1;
    return ((i>>24&&15)==0);
}
int main(int argc,char*argv[])
{
    if(checkSystem2())
        printf("littleEndian\n");
    else
        printf("bigEndian\n");
    return 0;
}

#endif