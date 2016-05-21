/*
 * this program contains the method of converting the hex to decimal. Four
 * methods are listed.
 */
//#define _HTOI
#ifdef _HTOI
#include <stdio.h>
#include <ctype.h>
#include <string.h>
long htoi(char*s);
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
//long htoi(char*s)
//{
//    long sum;
//    int digit;
//    while(isspace(*s))
//        ++s;
//
//    for(sum=0L;isxdigit(*s);++s)
//    {
//        if(isdigit(*s))
//            digit=*s-'0';
//        else
//            digit=toupper(*s)-'A'+10;
//        sum=sum*16L+digit;
//    }
//    return sum;
//}
//long htoi(char*s)
//{
//    static char xdigits[]="0123456789ABCDEF";
//    long sum;
//    int digit;
//
//    while(isspace(*s))
//        ++s;
//
//    for(sum=0L;isxdigit(*s);++s)
//    {
//        digit=strchr(xdigits,toupper(*s))-xdigits;
//        sum=sum*16L+digit;
//    }
//    return sum;
//}
//long htoi(char*s)
//{
//    int sum;
//    sscanf(s,"%x",&sum);
//    return sum;
//}
//long htoi(char*s)
//{
//    return strtol(s,NULL,16);
//}

int htoi(char s[]){
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
int main(){
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

//int atoi_(char s[]){
//    int i,n,sign;
//    for(i=0;isspace(s[i]);i++);
//    sign=(s[i]=='-')?-1:1;
//    if(s[i]=='+'||s[i]=='-')
//        i++;
//    for(n=0;isdigit(s[i]);i++)
//        n=10*n+(s[i]-'0');
//    return sign*n;
//}
//int main(){
//    int i;
//    char s[20];
//    for(i=0;i<10;i++){
//        int value=rand();
//        int d;
//        if(value&0x1)
//            value=-value;
//        sprintf(s,"%d",value);
//        d=atoi_(s);                          
//        printf("%12d %12s %12d ",value,s,d);
//        if(value==d)
//            printf("OK\n");
//        else
//            printf("NO\n");
//    }
//}

#endif