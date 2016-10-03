/*
 * string_ops.cc
 *
 *  Created on: 2016-5月-21 下午2:46:03
 *      Author: Felix
 */
/*
 *  this program contains the basic string functions implementation
 */
#ifdef _STR_FUNC
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#define toupper_(a) ((a)>='a'&&(a)<='z'?(a)+'A'-'a':(a))
#define tolower_(a) ((a)>='A'&&(a)<='Z'?(a)+'a'-'A':(a))
#define isalnum_(c) ((toupper_((c))>='A')&&(toupper((c))<='Z')||((c)>='0')&&((c)<='9'))
#define isalpha_(c) ((toupper_((c))>='A')&&(toupper((c))<='Z'))
#define isdigit_(c) (((c)>='0')&&((c)<='9'))
#define isascii_(c) ((unsigned)(c)<128)
#define isgraph_(c) (((c)>=33)&&((c)<=127))
#define islower_(c) (((c)>='a')&&((c)<='z'))
#define isupper_(c) (((c)>='A')&&((c)<='Z'))
#define isprint_(c) (((c)>=32)&&((c)<=127))
#define ispunct_(c) (isgraph_((c))&&!isalnum_((c)))
#define isspace_(c) (((c)==32)||((c)==9)||((c)==13))
#define isxdigit_(c) (isdigit_((c))||(toupper_((c))>='A'&&toupper_((c))<='F'))
#define toascii_(c) ((c)&0x7f)


//判断字符串的长度
size_t strlen_(const char *str)
{
    size_t i=0;
    while(str[i])
        i++;
    return i;
}
//将一个串的内容追加到另一个串上
char *strcat_(char*tgt,const char*src)
{
    char*p=tgt;
    while(*tgt)
        tgt++;
    while(*tgt++=*src++)
        ;
    return p;
}
char *strncat_(char*tgt,const char*src,int n)
{
    int i=0;
    char*p=tgt;
    while(*tgt)
        tgt++;
    while((i++<n)&&(*tgt++=*src++))
        ;
    if(i>n)
        *tgt='\0';
    return p;
}
//将一个字符串的字符复制到另一个字符串中
char *strcpy_(char*dest,const char*src)
{
    while(*dest++=*src++)
        ;
    return dest-1;
}
char *strncpy_(char*dest,const char*src,int n)
{
    int i=0;
    while((i++<n)&&(*dest++=*src++))
        ;
    if(i>n)
        *dest='\0';
    return dest-1;
}
//判断两个字符串是否相同
int streql(const char*str1,const char*str2){
    while((*str1==*str2)&&(*str1))
    {
        str1++;
        str2++;
    }
    return (*str1=='\0')&&(*str2=='\0');
}
//比较字符串时忽略大小写
int strieql(const char*str1,const char*str2){
    while((toupper_(*str1)==toupper_(*str2))&&(*str1))
    {
        str1++;
        str2++;
    }
    return (*str1=='\0')&&(*str2=='\0');
}
//获取字符串中第一次出现的某个字符
char* strchr_(char*str,char c)
{
    while((*str!=c)&&(*str))
        str++;
    return str;
}
//返回索引到串的首次出现
int strchr_i(const char*str,char c)
{
    int i=0;
    while((str[i]!=c)&&(str[i]))
        i++;
    return (str[i]?i:-1);
}
//搜索字符在字符串中的末次出现
char* strrchr_(char*str,char c)
{
    char*p=NULL;
    while(*str)
    {
        if(*str==c)
            p=str;
        str++;
    }
    return p;
}
//返回指向字符中末次出现的索引
int strrchr_i(const char*str,char c)
{
    int p=-1;
    int i=0;
    while(str[i])
    {
        if(str[i]==c)
            p=i;
       i++;
    }
    return p;
}
//比较两个字符串
int strcmp_(const char*str1,const char*str2){
    while((*str1==*str2)&&(*str1))
    {
        str1++;
        str2++;
    }
    if((*str1==*str2)&&(!*str1))
        return 0;
    else if((*str1)&&(!*str2))
        return -1;
    else if((!*str1)&&(*str2))
        return 1;
    else
        return (*str1>*str2?-1:1);

}
//比较两个字符中的前N个字符
int strncmp_(const char*str1,const char*str2,int n){
    int i=0;
    while((i<n)&&(str1[i]==str2[i])&&(str1[i]))
    {
        i++;
    }
    if(i==n)
        return 0;
    else if(str1[i]==str2[i]&&!str1[i])
        return 0;
    else if((str1[i])&&(!str2[i]))
        return -1;
    else if((!str1[i])&&(str2[i]))
        return 1;
    else
        return (str1[i]>str2[i]?-1:1);
}
//从给定字符序列中查找字符的首次出现
size_t strspn_(const char*s1,const char*s2)
{
    int i=0;
    int j;
    int sz1=strlen(s1);
    int sz2=strlen(s2);
    while(i<sz1)
    {
        j=0;
        while(j<sz2)
        {
            if(s2[i]==s1[j])
                break;
            j++;
        }
        if(j==sz2)
            return i;
        i++;
    }
    return i;

}
//在字符串中查找子字符串
char* strstr_(const char*s1,const char *s2)
{
    while(*s1!='\0')
    {
        char*p=s1;
        char *q=s2;
        while(*q==*p&&*q)
        {
            p++;
            q++;
        }
        if(*q=='\0')
            return s1;
        s1++;
    }
    return NULL;

}
//计算子字符串出现的次数
int strstr_cnt(const char*s1,const char *s2)
{
    int count=0;
    while(*s1!='\0')
    {
        char*p=s1;
        char *q=s2;
        while(*q==*p&&*q)
        {
            p++;
            q++;
        }
        if(*q=='\0')
            count++;
        s1++;
    }
    return count;

}
//给子字符串获取索引
int  strstr_i(const char*s1,const char *s2)
{
    char*org=s1;
    while(*s1!='\0')
    {
        char*p=s1;
        char *q=s2;
        while(*q==*p&&*q)
        {
            p++;
            q++;
        }
        if(*q=='\0')
            return (s1-org);
        s1++;
    }
    return -1;

}
//'
//
char* strstr_rem(char*str,char*substr)
{
    char*org=str;
    char*p=NULL;
    while(*str!='\0')
    {
        p=str;
        char*q=substr;
        while(*q!='\0')
        {
            if(*p!=*q)
                break;
            p++;
            q++;
        }
        if(*q=='\0')
            break;
        str++;
    }
    if(*str!='\0')
        strcpy_(org,p);
    return str;
}
int main()
{
//    char* a="hello";
//   char b[]="world";
//    char dest[200];
//    strcat(strcpy(strcpy(dest,a)," k,"),b);
//    printf("%s\n",dest);
//    printf("%d\n",strlen(a));
// printf("%d\n",sizeof(b));
// printf("%d\n",strieql("abc","ABC"));
// printf("%d\n",streql("Abc","Abc"));
//char buf[100];
//strcpy(buf,a);
//int i;
//char *p;
//getcwd(buf,100);
//printf("%s\n",buf);
//char *buf2=get_current_dir_name();
//printf("%s\n",buf2);
//free(buf2);
//p=strchr_(buf,'/');
//printf("%s\n",p);
//i=strchr_i(buf,'/');
//printf("%d\n",i);
//p=strrchr_(buf,'/');
//printf("%s\n",p);
//i=strrchr_i(buf,'/');
//printf("%d\n",i);
//strrev_(buf);
//printf("%s\n",buf);
//
//printf("%d\n",strncmp_("abc","abcdef",3));
//printf("%d\n",strncmp_("abc","bcdef",3));
//printf("%d\n",strncmp_("abcdefg","abcdef",10));

//   printf("%s\n",strstr_("AbcDef","Abc"));
//   printf("%s\n",strstr_("AbcDef","Def"));
//   printf("%s\n",strstr_("AbcAbc","Def")?"over":"NULL");
char p[]="AbcDef";
char q[]="Abc";
printf("%s\n",p);
strstr_rem(p,q);
printf("%s\n",p);



    return 0;

}
#endif



