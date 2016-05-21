/*
 * implementation of getline
 */
/*
 * getline函数是一个比较常见的函数,根据它的名字我们就可以知道这个函数是来完成读入一行数据的。
 */
//#define _GETLINE
#ifdef _GETLINE_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * 在标准C语言中，getline函数是不存在的, 此处我们列出了不同的实现方式
 */

//first version: stops at blank line
static int getline1(char s[],int lim,FILE*fp){
    int ch,i=0;
    while((ch=fgetc(fp))!=EOF&&ch!='\n'&&i<lim){
        s[i++]=ch;
    }
    s[i]='\0';
    return i;//the length of line
}
//second version: can read the blank line
//maybe this is the perfect version
static int getline2(char s[],int lim,FILE*fp){
    int ch,i=0;
    while((ch=fgetc(fp))!=EOF&&ch!='\n'&&i<lim){
        s[i++]=ch;
    }
    if(ch==EOF&&i==0)//end of file
        return -1;
    s[i]='\0';
    return i;//the length of line
}

//third version: include newline character LF
//this is same with getline of glibc
static int getline3(char s[],int lim,FILE*fp){
    int ch,i=0;
    while((ch=fgetc(fp))!=EOF&&ch!='\n'&&i<lim){
        s[i++]=ch;
    }
    if(ch==EOF&&i==0)//end of file
        return -1;
    if(ch=='\n')//add \n like glibc getline
        s[i++]=ch;
    s[i]='\0';
    return i;//the length of line
}

//in standard c, fgets actually is getline
//for fgets, newline is included in the buffer s
static int getline4(char* s,int lim,FILE* fp ){
	char *p;
	p=fgets(s,lim,fp);
	if(!p)
		return -1;
	return strlen(s);
}

#ifdef _GNU_SOURCE
/*
 * this is the version of glibc
 * ssize_t getline(char **lineptr, size_t *n, FILE *stream);
 * If *lineptr is NULL, then getline() will allocate a buffer for storing the line,
 * which should be freed by the user program.
 */
//getline allocate buffer itself
static void test_gnu_getline(void)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("/etc/motd", "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	while ((read = getline(&line, &len, fp)) != -1) {
		printf("Retrieved line of length %zu :\n", read);
		printf("%s", line);
	}

	free(line);//free memory allocated by getline
	exit(EXIT_SUCCESS);
}
//we allocate buffer,
static void test_gnu_getline2(){
    int read;
    int len=100;
    char *line=NULL;
    if((line=malloc((len+1)))==NULL){
        printf("Can't get memory\n");
        exit(-1);
    }
    while((read=getline(&line,&len,stdin))!=-1)
        printf("%s\n",line);
    free(line);
}

#endif

#define getline getline2
void simple_test(){
    char line[200];
    int i=0;
    while(getline(line,200,stdin)>-1)
        printf("%d:%s\n",i++,line);
}
void find_the_longest_line(){
    char cur_line[200];
    char longest_line[200];
    int len;
    int max_len=-1;
    while((len=getline(cur_line,200,stdin))>-1){
        if(len>max_len) {
            strcpy(longest_line,cur_line);
            max_len=len;
        }
    }
    printf("MAX:%d:%s\n",max_len,longest_line);
}
int main(){
    find_the_longest_line();
}

#endif


#ifdef _GETLINE_CPP
/*
 * In cpp, the standard lib contains the getline function
 * Actually, the inputstream objects have a member function called getline,
 * and also there is global getline fucntion
 */

//getline member function
/*
 * istream::getline
 * istream& getline (char* s, streamsize n );
 * istream& getline (char* s, streamsize n, char delim );
 */
void test_istream_getline(){
    char line[100];
    while(cin.getline(line,100))
        cout<<line<<endl;
}

//global getline
/*
 * istream& getline ( istream& is, string& str, char delim );
 * istream& getline ( istream& is, string& str );
 */
void test_global_getline(){
    string line;
    while(getline(cin,line))
        cout<<line<<endl;
}

#endif
