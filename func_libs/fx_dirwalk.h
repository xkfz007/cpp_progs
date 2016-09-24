#ifndef _FX_DIRWALK_H
#define _FX_DIRWALK_H

#include <stdio.h>
#include <string.h>
#include "fx_memops.h"

typedef struct dir_entry{
	int type;
	char name[1024];
}dir_entry_t;
static dir_entry_t* dent_list_new(int N){
	dir_entry_t* den_list;
	return den_list;
}

static dir_entry_t* dent_list_add(char* name,int type,dir_entry_t den_list[],int* idx,int *N){
	if(*idx>=*N){
		den_list=fx_realloc(den_list,*N*2);
		*N=*N*2;
	}
	den_list[*idx].type=type;
	strcpy(den_list[*idx].name,name);
	(*idx)++;

	return den_list;
}

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
  
static dir_entry_t* fx_dirwalk(char *dir,dir_entry_t den_list[], int* idx, int *N)
{  
    WIN32_FIND_DATA FindFileData;  
    HANDLE hFind=INVALID_HANDLE_VALUE;  
    int i,j;
    static int count=0;
    char DirSpec[MAX_PATH];
    TCHAR tmp[MAX_PATH];
	char buf[20];
	char *p;

	if(dir==NULL||den_list==NULL||idx==NULL||N==NULL){
		return NULL;
	}
    //DWORD dwError;  
    //strncpy(DirSpec,Dir,MAX_PATH);  
    //strncat(DirSpec,"\\*",MAX_PATH);
    sprintf(DirSpec,"%s\\*.*",dir);
    strncpy(tmp,DirSpec,MAX_PATH);
  
    hFind=FindFirstFile(tmp,&FindFileData);
  
    if(hFind==INVALID_HANDLE_VALUE)
    {  
        FindClose(hFind);   
		fx_log(NULL,FX_LOG_ERROR,"Cannot open directory: %s\n", dir);
        return NULL;    
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
				p=buf;
                while(i++<count)
                    p=sprintf(p,"+");
                fx_log(NULL,FX_LOG_INFO,"%s\e[32;44m%s\e[0m\n",p,FindFileData.cFileName);
                //StringCchCopy(DirAdd,MAX_PATH,Dir);  
                //StringCchCat(DirAdd,MAX_PATH,TEXT("\\"));  
                //StringCchCat(DirAdd,MAX_PATH,FindFileData.cFileName);
				den_list=dent_list_add(FindFileData.cFileName,FILE_ATTRIBUTE_DIRECTORY,den_list,idx,N);

                sprintf(DirAdd,"%s\\%s",Dir,FindFileData.cFileName);
                den_list=fx_dirwalk(DirAdd,den_list,idx,N);                                
            }  
            else if((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) 
            {  
				p=buf;
				while(j++<count+4)
					p=sprintf(p,"-");
				fx_log(NULL,FX_LOG_INFO,"\e[30;41m%s\e[0m\n",p,FindFileData.cFileName );
				den_list=dent_list_add(FindFileData.cFileName,FILE_ATTRIBUTE_NORMAL,den_list,idx,N);
                //wcout<<Dir<<"\\"<<FindFileData.cFileName<<endl;   
            }  
        }  
        FindClose(hFind);  
    }  
	return den_list;
}  

#else

#include <unistd.h>
#include <dirent.h>

static dir_entry_t* fx_dirwalk(char *dir,dir_entry_t den_list[], int* idx, int *N)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	char buf[20];
	char *p;
	static int count=0;
	int i=0,j=0;
	if(dir==NULL||den_list==NULL||idx==NULL||N==NULL){
		return NULL;
	}

	if((dp = opendir(dir)) == NULL) {
		fx_log(NULL,FX_LOG_ERROR,"Cannot open directory: %s\n", dir);
		return NULL;
	}
	count+=1;
	chdir(dir);
	while((entry = readdir(dp)) != NULL) {
		if(strcmp(entry->d_name,".")==0||
			strcmp(entry->d_name,"..")==0)
			continue;
		switch(entry->d_type){
				case DT_DIR:
					p=buf;
					while(i++<count)
						p=sprintf(p,"+");
					fx_log(NULL,FX_LOG_INFO,"%s\e[32;44m%s\e[0m\n",p,entry->d_name );
					den_list=dent_list_add(entry->name,entry->type,den_list,idx,N);
					den_list=fx_dirwalk(entry->d_name,den_list,idx,N);
					break;
				case DT_REG:
					den_list=dent_list_add(entry->name,entry->type,den_list,idx,N);
					p=buf;
					while(j++<count+4)
						p=sprintf(p,"-");
					fx_log(NULL,FX_LOG_INFO,"%s\e[30;41m%s\e[0m\n",p,entry->d_name );
					break;
				default:
					den_list=dent_list_add(entry->name,entry->type,den_list,idx,N);
					fx_log(NULL,FX_LOG_INFO,"%s\n",entry->d_name );
					break;
		}
	}
	count-=1;
	chdir("..");
	closedir(dir);
	
	return den_list;
}


#endif


#endif