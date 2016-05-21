/*
 *    Description:  获取某个域名的ip地址
 */
#ifdef _GETIP
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
int main()
{
    struct addrinfo *res,*pt;
    struct sockaddr_in *sinp;
    const char *addr;
    char abuf[INET_ADDRSTRLEN];
    int succ=0,i=0;
    char a[50];
    scanf("%s",a);
    succ=getaddrinfo(a,NULL,NULL,&res);
    if(!succ)
    {
        printf("ERROR!\n");
        exit(succ);

    }
    for(pt=res,i=0;pt!=NULL;pt=pt->ai_next,i++)
    {
        sinp=(struct sockaddr_in*)pt->ai_addr;
        addr=(const char*)inet_ntop(AF_INET,&sinp->sin_addr,abuf,INET_ADDRSTRLEN);
        printf("%2d.IP=%s\n",i,addr);
    }
    return EXIT_SUCCESS;
}
#endif