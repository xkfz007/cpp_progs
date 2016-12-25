#ifndef _FX_NETWORK_H
#define _FX_INETATON_H

#include   <stdio.h>
#include   <string.h>
#if _WIN32
#else 
#include   <sys/ioctl.h>
#include   <sys/socket.h>
#include   <netinet/in.h>
#include   <net/if.h>
#endif

struct in_addr  
{  
    unsigned int s_addr; // network byte order( big-endian)  
};  
// convert ip to number
// "192.168.33.123"=> 1100 0000 1010 1000 0010 0001 0111 1011
int fx_inet_aton(const char *cp, struct in_addr *ap)  
{  
    int dots = 0;  
    register u_long acc = 0, addr = 0;  
    do {  
        register char cc = *cp;  
        switch (cc)   
        {  
        case '0':  
        case '1':  
        case '2':  
        case '3':  
        case '4':  
        case '5':  
        case '6':  
        case '7':  
        case '8':  
        case '9':  
            acc = acc * 10 + (cc - '0');  
            break;  
        case '.':  
            if (++dots > 3)  
            {  
                return 0;  
            }  
            // Fall through  
        case '/0':  
            if (acc > 255)   
            {  
                return 0;  
            }  
            addr = addr << 8 | acc; 
            acc = 0;  
            break;  
        default:  
            return 0;  
        }  
    } while (*cp++) ;  
    // Normalize the address   
    if (dots < 3)  
    {  
        addr <<= 8 * (3 - dots) ;  
    }  
    //Store it if requested   
    if (ap)   
    {  
        ap->s_addr = htonl(addr);  
    }  
    return 1;      
}
/**
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 */
int fx_inet_aton_bsd(const char *cp, struct in_addr *addr)
{
    register unsigned int val;
    register int base, n;
    register char c;
    unsigned int parts[4];
    register unsigned int *pp = parts;
    c = *cp;
    for (;;) {
        /**
         * Collect number up to ``.''.
         * Values are specified as for C:
         * 0x=hex, 0=octal, isdigit=decimal.
         */
        if (!isdigit(c))
            return (0);
        val = 0; base = 10;
        if (c == '0') {
            c = *++cp;
            if (c == 'x' || c == 'X')
                base = 16, c = *++cp;
            else
                base = 8;
        }
        for (;;) {
            if (isascii(c) && isdigit(c)) {
                val = (val * base) + (c - '0');
                c = *++cp;
            } else if (base == 16 && isascii(c) && isxdigit(c)) {
                val = (val << 4) |
                    (c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++cp;
            } else
                break;
        }
        if (c == '.') {
            /**
             * Internet format:
             *  a.b.c.d
             *  a.b.c   (with c treated as 16 bits)
             *  a.b (with b treated as 24 bits)
             */
            if (pp >= parts + 3)
                return (0);
            *pp++ = val;
            c = *++cp;
        } else
            break;
    }
    /**
     * Check for trailing characters.
     */
    if (c != '\0' && (!isascii(c) || !isspace(c)))
        return (0);
    /**
     * Concoct the address according to
     * the number of parts specified.
     */
    n = pp - parts + 1;
    switch (n) {
    case 0:
        return (0);     /** initial nondigit */
    case 1:             /** a -- 32 bits */
        break;
    case 2:             /** a.b -- 8.24 bits */
        if ((val > 0xffffff) || (parts[0] > 0xff))
            return (0);
        val |= parts[0] << 24;
        break;
    case 3:             /** a.b.c -- 8.8.16 bits */
        if ((val > 0xffff) || (parts[0] > 0xff) || (parts[1] > 0xff))
            return (0);
        val |= (parts[0] << 24) | (parts[1] << 16);
        break;
    case 4:             /** a.b.c.d -- 8.8.8.8 bits */
        if ((val > 0xff) || (parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff))
            return (0);
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
        break;
    }
    if (addr)
        addr->s_addr = htonl(val);
    return (1);
}

//check if network card address is valid
//like:12-02-B5-2D-2F-8F or 00:0c:29:76:fa:88
int check_physical_address(const char* s){
    char tmp[6][4];
    int i;
    int ret;
    ret=sscanf(s,"%2s%*[-:]%2s%*[-:]%2s%*[-:]%2s%*[-:]%2s%*[-:]%2s",
            tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
#if _DEBUG
    {
        for(i=0;i<6;i++){
            fprintf(stdout,"value=%s\n",tmp[i]);
        }
    }
#endif
    if(ret!=6)
        return 0;
    for(i=0;i<6;i++){
        if(strlen(tmp[i])!=2)
            return 0;
        if(!isxdigit(tmp[i][0])||!isxdigit(tmp[i][0]))
            return 0;
    }
    
    return 1;
}

#if _WIN32

int check_ipv4_addr(const char *s) {
    char tmp[4][5];
    int i;
    int ret;
    ret=sscanf(s,"%3s%*[.]%3s%*[.]%3s%*[.]%3s",
            tmp[0],tmp[1],tmp[2],tmp[3]);
    if(ret!=4)
        return 0;
    for(i=0;i<4;i++){
        int value;
        if(strlen(tmp[i])<1)
            return 0;
        value=atoi(tmp[i]);
        if(value<0||value>255)
            return 0;
    }
    return 1;
    
}

#else
int get_inet_mac_addr(const char * ethname,char* addr)
{
    struct ifreq ifc;
    int sock;

    if((sock=socket(AF_INET,SOCK_STREAM,0)) <0)
    {
        fprintf(stderr,"could not open socket\n");
        return   -1;
    }
    strcpy(ifc.ifr_name,ethname);
    if(ioctl(sock,SIOCGIFHWADDR,&ifc) <0)
    {
        fprintf(stderr, "ioctl");
        return   -1;
    }
    sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned   char)ifc.ifr_hwaddr.sa_data[0],
            (unsigned   char)ifc.ifr_hwaddr.sa_data[1],
            (unsigned   char)ifc.ifr_hwaddr.sa_data[2],
            (unsigned   char)ifc.ifr_hwaddr.sa_data[3],
            (unsigned   char)ifc.ifr_hwaddr.sa_data[4],
            (unsigned   char)ifc.ifr_hwaddr.sa_data[5]);
    return   0;
}

int check_ipv4_addr(const char *s)
{
    struct in_addr addr;
    int ret;

    ret = inet_pton(AF_INET, s, &addr);
    if (ret > 0)
        return 1;
    else
        return 0;
}
#endif
#endif