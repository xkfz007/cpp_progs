/*
 * this is the usage of "strtok"
 */
#ifdef _STRTOK_USAGE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void strtok1()
{
    char *path="/etc/passwd";
    char buf[BUFSIZ];
    FILE* fp=fopen(path,"r");
    if(!fp)
    {
        printf("Can't open file:%s\n",path);
        exit(-1);
    }
    while(fgets(buf,BUFSIZ,fp))
    {
        char* token;
        printf("%s",buf);
        token=strtok(buf,":");
        printf("---->%s\n",token);
        while((token=strtok(NULL,":"))!=NULL)
            printf("---->%s\n",token);
    }
    fclose(fp);
}
int main()
{
    strtok1();
}
#endif