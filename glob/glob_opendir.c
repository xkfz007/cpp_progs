#include <stdio.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wchar.h>

static int OpenDir(const char *buf)
{
        int ret;
        char path[50] = {0};
        char temp[50] = {0};
        char *tp = NULL;
        glob_t globbuf;
        struct stat fileinfo;
        int i;
        char *ptr = NULL,*last_ptr = NULL;
        strcpy(path,buf);
        if(buf[strlen(buf)- 1] == '/')
                strcat(path,"*");
        else
                strcat(path,"/*");
        if((ret = glob(path,GLOB_NOSORT,NULL,&globbuf)) != 0){
                if(GLOB_NOMATCH == ret)
                        return 0;
                else
                        return -1;
        }
        strcpy(path,buf);
        if(buf[strlen(buf)- 1] == '/')
                strcat(path,".*");
        else
                strcat(path,"/.*");

        if((ret = glob(path,GLOB_APPEND,NULL,&globbuf)) != 0){
                if(GLOB_NOMATCH == ret)
                        return 0;
                else
                        return -1;
        }
        for(i = 0;i < globbuf.gl_pathc;i++){
                ret = lstat(globbuf.gl_pathv[i],&fileinfo);
                if(ret != 0){
                        perror("lstat()");
                        return -1;
                }
                if(1 == S_ISDIR(fileinfo.st_mode)){
                        printf("\n%s is directory!\n",globbuf.gl_pathv[i]);
                        strcpy(temp,globbuf.gl_pathv[i]);
                        tp = temp;
                        while((last_ptr = strsep(&tp,"/")) != NULL){
                                ptr = last_ptr;
                        }
                        if((strcmp(ptr,".") == 0) || (strcmp(ptr,"..") == 0))
                                continue;
                        ret = OpenDir(globbuf.gl_pathv[i]);
                        if(ret != 0){
                                printf("*****opendir() error!\n");
                        }
                }
                else
                {
                        printf("%s\n",globbuf.gl_pathv[i]);
                }
        }
        return 0;
}
int main(int argc, char *argv[])
{
        glob_t globbuf;
        int ret ;
        struct stat fileinfo;
        int i;
        if(argc != 2){
                printf("argument error!\n");
        }
        ret = OpenDir(argv[1]);
        if(ret != 0){
                printf("opendir() error!\n");
                return -1;
        }
        return 0;
}
