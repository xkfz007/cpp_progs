/*
 * hex_dec.cc
 *
 *  Created on: 2016-5月-21 下午2:58:10
 *      Author: Felix
 */

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
        printf("Usage:dec2hex <number> <digits>\n");
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

/*
 *
 * this program contains the method of converting the hex to decimal. Four
 * methods are listed.
 */
//#define _HTOI
#ifdef _HEX2DEC
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
long htoi(char*s)
{
    long sum;
    int digit;
    while(isspace(*s))
        ++s;

    for(sum=0L;isxdigit(*s);++s)
    {
        if(isdigit(*s))
            digit=*s-'0';
        else
            digit=toupper(*s)-'A'+10;
        sum=sum*16L+digit;
    }
    return sum;
}
long htoi2(char*s)
{
    static char xdigits[]="0123456789ABCDEF";
    long sum;
    int digit;

    while(isspace(*s))
        ++s;

    for(sum=0L;isxdigit(*s);++s)
    {
        digit=strchr(xdigits,toupper(*s))-xdigits;
        sum=sum*16L+digit;
    }
    return sum;
}
long htoi3(char*s)
{
    int sum;
    sscanf(s,"%x",&sum);
    return sum;
}
long htoi4(char*s)
{
    return strtol(s,NULL,16);
}

long htoi5(char s[]){
    int hexd,i,inhex,n;
    i=0;
    if(s[i]=='0'){
        ++i;
        if(s[i]=='x'||s[i]=='X')
            ++i;
    }
    n=0;
    inhex=1;
    while(inhex){
        if(s[i]>='0'&&s[i]<='9')
            hexd=s[i]-'0';
        else if(s[i]>='a'&&s[i]<='f')
            hexd=s[i]-'a'+10;
        else if(s[i]>='A'&&s[i]<='F')
            hexd=s[i]-'A'+10;
        else
            inhex=0;
        if(inhex==1)
            n=16*n+hexd;
        i++;
    }
    return n;
}
void test(){
    int count=10;
    int i;
    char s[100];
    for(i=0;i<10;i++){
        int num=rand()%10000;
        int n;
        sprintf(s,"%#0x",num);//
        n=htoi(s);
        printf("%#0x %s %d %d \n",num,s,num,n);

    }

}
//int main(int argc,char*argv[])
//{
//    char *s;
//    int value;
//    s=argv[1];
//    value=htoi(s);
//    printf("%s %d\n",s,value);
//
//    return 0;
//}
int main(int argc,char*argv[]){
	int n;
	if(argc<2)
	{
		fprintf(stdout,"Usage:hex2dec <hex>\n");
		exit(1);
	}
	n=htoi(argv[1]);
	fprintf(stdout,"%s : %d",argv[1],n);
}

#endif


