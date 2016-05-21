#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <iostream>
using namespace std;
// 将char类型转化为wchar13 // locale: 环境变量的值，mbstowcs依赖此值来判断src的编码方式
int ToWchar(char* &src, wchar_t* &dest, const char *locale = "zh_CN.utf8"){
    if (src == NULL) {
        dest = NULL;
        return 0;

    }
    //根据环境变量设置locale
    setlocale(LC_CTYPE, locale);
    //得到转化为需要的宽字符大小
    int w_size = mbstowcs(NULL, src, 0) + 1;
    //w_size=0说明mbstowcs返回值为-1。即在运行过程中遇到了非法字符(很有可能使locale没有设置正确)
    if (w_size == 0) {
        dest = NULL;
        return -1;

    }
    wcout << "w_size" << w_size << endl;
    dest = new wchar_t[w_size];
    if (!dest) return -1;
    int ret = mbstowcs(dest, src, strlen(src)+1);
    if (ret <= 0)return -1;
    return 0;

}
int main(){
    char* str = "中国123";
    wchar_t *w_str ;
    ToWchar(str,w_str);
    wcout << w_str[0] << "--" << w_str[1] << "--" << w_str[2];
    delete(w_str);
    return 0;
}
