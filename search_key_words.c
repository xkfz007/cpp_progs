/*
 * this program demenstrates how to search key words
 */
#ifdef _SEARCH_KEY_WORDS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct person
{
    char last[16];
    char first[11];
    char phone[13];
    int age;
};
static int comp(const void *,const void *);

main()
{
    int i;
    struct person *p;
    static struct person people []={
        {"sfsf","fddd","1341234",28},
        {"dfsf","fsfad","341234",28},
        {"wer","fdhd","41234",28},
        {"pppwer","sdfafddd","1334",28},
        {"oonn","asfdd","13414",28},
        {"mfam","dsdf","13234",28},
    };
    static struct person  key;
    strcpy(key.phone,"41234");

    qsort(people,6,sizeof people[0],comp);
    p=(struct person *)bsearch(&key,people,6,sizeof people[0],comp);
    if(p!=NULL)
    {
        printf("%s,%s,%s,%d\n",p->last,p->first,p->phone,p->age);
    }
    else
        puts("Not found");
    return 0;
}
static int comp(const void *p1,const void *p2)
{
    const struct person *ps1=(const struct person *)p1;
    const struct person *ps2=(const struct person *)p2;
    return strcmp(ps1->phone,ps2->phone);
}

#endif