/*
 * this is a great program to traverse the directory.
 */
#ifdef _DIRWALK
#include "fx_log.h"
#include "fx_dirwalk.h"

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

/*  We start with the appropriate headers and then a function, printdir,
    which prints out the current directory.
    It will recurse for subdirectories, using the depth parameter is used for indentation.  */
void printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 || 
                strcmp("..",entry->d_name) == 0)
                continue;
            printf("%*s%s/\n",depth,"",entry->d_name);
            /* Recurse at a new indent level */
            printdir(entry->d_name,depth+4);
        }
        else printf("%*s%s\n",depth,"",entry->d_name);
    }
    chdir("..");
    closedir(dp);
}

/*  Now we move onto the main function.  */

int main(int argc, char* argv[])
{
    char *topdir, pwd[2]=".";
    if (argc != 2)
        topdir=pwd;
    else
        topdir=argv[1];

    printf("Directory scan of %s:\n",topdir);
    printdir(topdir,0);
    printf("------done-------\n");

    return (0);
}



#endif

#if 0
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <tchar.h>
  
void dirwalk_win(char* Dir)      
{  
    WIN32_FIND_DATA FindFileData;  
    HANDLE hFind=INVALID_HANDLE_VALUE;  
    int i=0,j=0;
    static int count=0;
    char DirSpec[MAX_PATH];
    TCHAR tmp[MAX_PATH];
    //DWORD dwError;  
    //strncpy(DirSpec,Dir,MAX_PATH);  
    //strncat(DirSpec,"\\*",MAX_PATH);
    sprintf(DirSpec,"%s\\*.*",Dir);
    strncpy(tmp,DirSpec,MAX_PATH);
  
    hFind=FindFirstFile(tmp,&FindFileData);
  
    if(hFind==INVALID_HANDLE_VALUE)
    {  
        FindClose(hFind);   
        return;    
    }  
    else   
    {  
        while(FindNextFile(hFind,&FindFileData)!=0)
        {  
            i=0;
            j=0;
            if((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0)
            {  
                char DirAdd[MAX_PATH];  
                if(strcmp(FindFileData.cFileName,".")==0||strcmp(FindFileData.cFileName,"..")==0)
                    continue;
                while(i++<count)
                    printf("+");
                printf("\e[32;44m%s\e[0m\n",FindFileData.cFileName);
                //StringCchCopy(DirAdd,MAX_PATH,Dir);  
                //StringCchCat(DirAdd,MAX_PATH,TEXT("\\"));  
                //StringCchCat(DirAdd,MAX_PATH,FindFileData.cFileName);
                sprintf(DirAdd,"%s\\%s",Dir,FindFileData.cFileName);
                dirwalk_win(DirAdd);                                
            }  
            else if((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) 
            {  
                    while(j++<count+4)
                        printf("-");
                    printf("\e[30;41m%s\e[0m\n",FindFileData.cFileName );
                //wcout<<Dir<<"\\"<<FindFileData.cFileName<<endl;   
            }  
        }  
        FindClose(hFind);  
    }  
}  
  
int main( int argc, char *argv[])  
{  
    dirwalk_win("F:\\QQDownload"); 
    return 0;
}  
#endif
