/*
 *    Description: 十进制转化为十六进制 
 */
#ifdef _DEC2HEX
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void print8(char x);
void print16(short x);
void print32(int x);
char hexdigit(int a);
void print8_bin(char x);
void print16_bin(short x);
void print32_bin(int x);

int main(int argc,char*argv[])
{
    int digit=32;
    if(argc<2)
    {
        printf("Usage:dec2hex number digits\n");
        exit(-1);
    }
    if(argc==2)
    {
        int x=atoi(argv[1]);
        print32(x);
        print32_bin(x);
    }
    if(argc>2)
    {
        digit=atoi(argv[2]);
        switch(digit)
        {
         case 8:
             {
        char x=atoi(argv[1]);
             print8(x);
             print8_bin(x);
             }
             break;
         case 16:
             {
        short x=atoi(argv[1]);
        print16(x);
        print16_bin(x);
             }
             break;
         default:
             {
             int x=atoi(argv[1]);
             print32(x);
             print32_bin(x);
             }
             break;
        }
    }
   return 0; 
}
char hexdigit(int a)
{
static char hexd[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    return hexd[a];
}
void print8(char x)
{
    int i;
    printf("0x");
    for(i=4;i>=0;i-=4)
        printf("%c",hexdigit(x>>i&15));
    printf("\n");
}
void print16(short x)
{
    int i;
    printf("0x");
    for(i=12;i>=0;i-=4)
        printf("%c",hexdigit(x>>i&15));
    printf("\n");
}
void print32(int x)
{
    int i;
    printf("0x");
    for(i=28;i>=0;i-=4)
        printf("%c",hexdigit(x>>i&15));
    printf("\n");
}

void print8_bin(char x)
{
    int i;
    for(i=7;i>=0;i--)
        printf("%d",x>>i&1);
    printf("\n");
}
void print16_bin(short x)
{
    int i;
    for(i=15;i>=0;i--)
        printf("%d",x>>i&1);
    printf("\n");
}
void print32_bin(int x)
{
    int i;
    for(i=31;i>=0;i--)
        printf("%d",x>>i&1);
    printf("\n");
}

#endif