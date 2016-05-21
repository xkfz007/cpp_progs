/*
 * this is an very simple dict implementation.
 */
#ifdef _SIMPLE_DICT
#include <stdio.h>
#include <string.h>
#include <ctype.h>
char *dic[]={
    "atlas","A voluem of maps.",
    "car","A motorized vechicle.",
    "telephone","A communication device.",
    "airplane","A flying machine.",
    "","",
};
int main()
{
    char word[80];
    char **p;
    do{
        puts("Enter the word:");
        scanf("%s",word);
        p=(char**)dic;
        do{
            if(!strcmp(*p,word))
            {
                puts("Meaning:");
                puts(*(p+1));
                break;
            }
            p=p+2;
        }while(*p);
        if(!*p)
            puts("Not found.");
        puts("Another word?");
        scanf("%s",word);
    }while(word[0]=='Y'||word[0]=='y');

    return 0;
}
#endif