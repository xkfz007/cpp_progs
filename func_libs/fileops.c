
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

