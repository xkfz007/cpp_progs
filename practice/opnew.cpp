/*
 * this program is used to overload the operator new and delete
 */
#if 0
#include <cstdlib>
#include <new>
using namespace std;
void *operator new(size_t siz)
{
    void (*new_handler)()=set_new_handler(0);
    set_new_handler(new_handler);
    for(;;)
    {
        void *p=malloc(siz);
        if(p)
            return p;
        if(new_handler)
            new_handler();
        else
            return 0;
    }
}
void operator delete(void*p)
{
    if(p)
        free(p);

}
main()
{
    double *dp=new double;
    delete dp;
}
#endif