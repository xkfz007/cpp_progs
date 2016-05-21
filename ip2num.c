#ifdef _IP2NUM
// ip��ַ�ṹ��  
struct in_addr  
{  
    unsigned int s_addr; // network byte order( big-endian)  
};  
// inet_aton�� ���ʮ���ƴ�cp ת��Ϊһ�������ֽ�˳���32λ���� IP��ַ  
// ���罫��cp "192.168.33.123 "  
// תΪ 1100 0000   1010 1000    0010 0001     0111 1011   
// �ɹ�����1��������0  
// ת���ɹ�����������ڽṹ��ָ��ap��  
int inet_aton(const char *cp, struct in_addr *ap)  
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
            addr = addr << 8 | acc; // ����Ǿ���,ÿ�ν���ǰֵ���ư�λ���Ϻ����ֵ  
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
int inet_aton_bsd(const char *cp, struct in_addr *addr)
{
    register u_int32_t val;
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



#endif