/*
 * fx_fileops.c
 *
 *  Created on: 2016-05-21 6:05:55 pm
 *  Author: Felix
 */
#include <string.h>
#include "fx_fileops.h"

/*
 * get file name extension
 */
static inline char *fx_get_filename_extension( char *filename )
{
    char *ext = filename + strlen( filename );
    while( *ext != '.' && ext > filename )
        ext--;
    ext += *ext == '.';
    return ext;
}


#ifdef _WIN32
/* Functions for dealing with Unicode on Windows. */
FILE *fx_fopen( const char *filename, const char *mode )
{
    wchar_t filename_utf16[MAX_PATH];
    wchar_t mode_utf16[16];
    if( utf8_to_utf16( filename, filename_utf16 ) && utf8_to_utf16( mode, mode_utf16 ) )
        return _wfopen( filename_utf16, mode_utf16 );
    return NULL;
}

int fx_rename( const char *oldname, const char *newname )
{
    wchar_t oldname_utf16[MAX_PATH];
    wchar_t newname_utf16[MAX_PATH];
    if( utf8_to_utf16( oldname, oldname_utf16 ) && utf8_to_utf16( newname, newname_utf16 ) )
    {
        /* POSIX says that rename() removes the destination, but Win32 doesn't. */
        _wunlink( newname_utf16 );
        return _wrename( oldname_utf16, newname_utf16 );
    }
    return -1;
}

int fx_stat( const char *path, fx_struct_stat *buf )
{
    wchar_t path_utf16[MAX_PATH];
    if( utf8_to_utf16( path, path_utf16 ) )
        return _wstati64( path_utf16, buf );
    return -1;
}

int fx_vfprintf( FILE *stream, const char *format, va_list arg )
{
    HANDLE console = NULL;
    DWORD mode;

    if( stream == stdout )
        console = GetStdHandle( STD_OUTPUT_HANDLE );
    else if( stream == stderr )
        console = GetStdHandle( STD_ERROR_HANDLE );

    /* Only attempt to convert to UTF-16 when writing to a non-redirected console screen buffer. */
    if( GetConsoleMode( console, &mode ) )
    {
        char buf[4096];
        wchar_t buf_utf16[4096];

        int length = vsnprintf( buf, sizeof(buf), format, arg );
        if( length > 0 && length < sizeof(buf) )
        {
            /* WriteConsoleW is the most reliable way to output Unicode to a console. */
            int length_utf16 = MultiByteToWideChar( CP_UTF8, 0, buf, length, buf_utf16, sizeof(buf_utf16)/sizeof(wchar_t) );
            DWORD written;
            WriteConsoleW( console, buf_utf16, length_utf16, &written, NULL );
            return length;
        }
    }
    return vfprintf( stream, format, arg );
}

int fx_is_pipe( const char *path )
{
    wchar_t path_utf16[MAX_PATH];
    if( utf8_to_utf16( path, path_utf16 ) )
        return WaitNamedPipeW( path_utf16, 0 );
    return 0;
}
#endif

static inline int fx_is_regular_file( FILE *filehandle )
{
    fx_struct_stat file_stat;
    if( fx_fstat( fileno( filehandle ), &file_stat ) )
        return 1;
    return S_ISREG( file_stat.st_mode );
}

static inline int fx_is_regular_file_path( const char *filename )
{
    fx_struct_stat file_stat;
    if( fx_stat( filename, &file_stat ) )
        return !fx_is_pipe( filename );
    return S_ISREG( file_stat.st_mode );
}
//c获取文件的大小的两种方法和行读取
//1. fseek移动指针获取
#if 0
#include <stdio.h>
#include <stdlib.h>
long filesize( FILE *fp )
{
    long int save_pos;
    long size_of_file;

    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );

    return( size_of_file );
}

int main( void )
{
    FILE *fp;
    fp = fopen( "aa.txt", "r" );
    if( fp != NULL ) {
        printf( "File size=%ld\n", filesize( fp ) );
        fclose( fp );

        return EXIT_SUCCESS;
    }
     return EXIT_FAILURE;
}
#endif

//2 stat获取
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main( void )
{
    struct stat buf;

    if( stat( "file", &buf ) != -1 ) {
      printf( "File size = %d\n", buf.st_size );
    }
    return EXIT_SUCCESS;
  }
#endif

//但是是有区别的，如通过fseek()、ftell()两个函数，我们就可以随意访问文件的任何位置了，想了想好像操作文件就这么easy，实在也没有更多可说的了。
//对 了，fseek()和ftell()存在一个潜在的问题就是他们限制文件的大小只能在long类型的表示范围以内，
//也就是说通过这种方式，只能打开 2,000,000,000字节的文件，不过在绝大多数情况下似乎也已经够用了。如果需要打开更大的文件，你需要用到fgetpos()、 fsetpos()函数了.


#if 0
/*
*this program demonstrate how to modify some position of a file
* from internet:http://blog.csdn.net/dlutbrucezhang/article/details/9165105
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
int main()
{
  /*
  *linebuffer：读取文件中的一行存入的缓冲
  *buffer1：一行中第一个字段的存入缓冲
  *buffer2：一行中第二个字段的存入缓冲
  */
  char linebuffer[512] = {0};
  char buffer1[512] = {0};
  char buffer2[512] = {0};

  int line_len = 0;
  int len = 0;
  int res;

  /*
  * cc.cfg是文件名，r+代表可以读写文件
  */
  FILE *fp = fopen("cabac.cfg", "r+");
  if(fp == NULL)
  {
    printf("open error");
    return -1;
  }
  while(fgets(linebuffer, 512, fp))
  {
    line_len = strlen(linebuffer);
    len += line_len;
    /*
    * buffer1=wireless.1.current_state
    * buffer2=1
    */
    //sscanf(linebuffer, "%[^=]=%[^=]", buffer1,buffer2);
    //sscanf(linebuffer, "%[^(](%[^)])%[^)]", buffer1,buffer2);
    //if(!strcmp("wireless.1.current_state", buffer1))
    if(strstr(linebuffer,"num_entry_point_offsets")!=NULL)
    {
      //read line "offset_len_minus1"
      fgets(linebuffer, 512, fp);
      // line_len = strlen(linebuffer);
      // len += line_len;
      char *pos=strchr(linebuffer,':');
      int temp_len=pos-linebuffer+1;
      //   len -= strlen(linebuffer);
      //len+=(temp_len+2+2);
      int temp_len2=temp_len+len+3;
      res = fseek(fp, temp_len2, SEEK_SET);
      //strcpy(buffer2, "=0");
      /*strcat(buffer1, "=");*/
      //strcat(buffer1, buffer2);
      //printf("%d",strlen(buffer1));
      int num=5;
      fprintf(fp, "%-2d", num);
      line_len = strlen(linebuffer)+1;
      len += line_len+2;
      res = fseek(fp, len, SEEK_SET);
      //read line "entry_point_offset_minus1"
      fgets(linebuffer, 512, fp);
      line_len = strlen(linebuffer)+1;
      pos=strchr(linebuffer,'(');
      temp_len=pos-linebuffer+1;
      fclose(fp);

      for(int i=0;i<3;i++){
        fp=fopen("cabac.cfg", "r+");
        temp_len2=temp_len+len;
        res = fseek(fp, temp_len2, SEEK_SET);
        num=33;
        fprintf(fp, "%-2d) : %-5d", num,7777);
        fclose(fp);
        len+=line_len;

      }

      break;
    }
  }
  return 0;
}
#endif


#ifdef _MODIFY_FILE
/*
*this program demonstrate how to modify some position of a file
* from internet:http://blog.csdn.net/dlutbrucezhang/article/details/9165105
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
int main()
{
  /*
  *linebuffer：读取文件中的一行存入的缓冲
  *buffer1：一行中第一个字段的存入缓冲
  *buffer2：一行中第二个字段的存入缓冲
  */
  char linebuffer[512] = {0};
  char buffer1[512] = {0};
  char buffer2[512] = {0};

  int line_len = 0;
  int len = 0;
  int res;

  /*
  * cc.cfg是文件名，r+代表可以读写文件
  */
  FILE *fp = fopen("cabac.cfg", "r+");
  if(fp == NULL)
  {
    printf("open error");
    return -1;
  }
  while(fgets(linebuffer, 512, fp))
  {
    line_len = strlen(linebuffer);
    len += line_len;
    /*
    * buffer1=wireless.1.current_state
    * buffer2=1
    */
    //sscanf(linebuffer, "%[^=]=%[^=]", buffer1,buffer2);
    //sscanf(linebuffer, "%[^(](%[^)])%[^)]", buffer1,buffer2);
    //if(!strcmp("wireless.1.current_state", buffer1))
    if(strstr(linebuffer,"num_entry_point_offsets")!=NULL)
    {
      //read line "offset_len_minus1"
      fgets(linebuffer, 512, fp);
      // line_len = strlen(linebuffer);
      // len += line_len;
      char *pos=strchr(linebuffer,':');
      int temp_len=pos-linebuffer+1;
      //   len -= strlen(linebuffer);
      //len+=(temp_len+2+2);
      int temp_len2=temp_len+len+3;
      res = fseek(fp, temp_len2, SEEK_SET);
      //strcpy(buffer2, "=0");
      /*strcat(buffer1, "=");*/
      //strcat(buffer1, buffer2);
      //printf("%d",strlen(buffer1));
      int num=5;
      fprintf(fp, "%-2d", num);
      line_len = strlen(linebuffer)+1;
      len += line_len+2;
      res = fseek(fp, len, SEEK_SET);
      //read line "entry_point_offset_minus1"
      fgets(linebuffer, 512, fp);
      line_len = strlen(linebuffer)+1;
      pos=strchr(linebuffer,'(');
      temp_len=pos-linebuffer+1;
      fclose(fp);

      for(int i=0;i<3;i++){
        fp=fopen("cabac.cfg", "r+");
        temp_len2=temp_len+len;
        res = fseek(fp, temp_len2, SEEK_SET);
        num=33;
        fprintf(fp, "%-2d) : %-5d", num,7777);
        fclose(fp);
        len+=line_len;

      }

      break;
    }
  }
  return 0;
}
#endif
#ifdef _PARSE_NUM
/*
 * read numbers from file
示例文件：
读入数据为：
5 34  324w4 23w 33
323 23f 23 wer 234
; ?
112
12345

*/
#include <string>
#include <iostream>
#include <fstream>
using namespace std ;

int main (int argc ,char **argv ){
    if( argc<2){
        cerr<< "Invaild input"<<endl ;
        return -1;
    }
    ifstream fin( argv[1]);
    if(! fin){
        cerr<< "Invaild file:"<<argv [1]<<endl;
        return -2;
    }
    string line;
    string digits("0123456789" );
    while( getline(fin ,line )){
        cout<< "Read from file: "<<line <<endl;
        cout<< ">>>>:";
        string word ;
        string::size_type pos =0,pos2 =0;
        while((pos2 =line .find_first_of (digits ,pos ))!=string ::npos ){
            pos= line. find_first_not_of(digits ,pos2 );
            if( pos!= string::npos ){
                word= line. substr(pos2 ,pos -pos2 );
            }
            else{
                word= line. substr(pos2 );
            }
            cout<< word<< ' ';
        }
        cout<< endl;
    }

    return 0;
}


#endif



