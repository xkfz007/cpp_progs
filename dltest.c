#ifdef _DLFCN
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
int main()
{
    void *handle;
    void (*errfcn)(const char*fmt,...);
    const char *errmsg;
    FILE* pf;
    handle=dlopen("./liberr.so",RTLD_NOW);
    if(handle==NULL){
        fprintf(stderr,"Failed to load liberr.so:%s\n",dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();
    errfcn=dlsym(handle,"err_ret");
    if((errmsg=dlerror())!=NULL){
        fprintf(stderr,"Didn't find err_ret:%s\n",errmsg);
        exit(EXIT_FAILURE);
    }
    if((pf=fopen("foobar","r"))==NULL){
        errfcn("Couldn't open foobar");
    }
    dlclose(handle);
    exit(EXIT_SUCCESS);

}
#endif