

/****************************************************************************
 * x264_malloc:
 ****************************************************************************/
void *x264_malloc( int i_size )
{
    uint8_t *align_buf = NULL;
#if HAVE_MALLOC_H
#if HAVE_THP
#define HUGE_PAGE_SIZE 2*1024*1024
#define HUGE_PAGE_THRESHOLD HUGE_PAGE_SIZE*7/8 /* FIXME: Is this optimal? */
    /* Attempt to allocate huge pages to reduce TLB misses. */
    if( i_size >= HUGE_PAGE_THRESHOLD )
    {
        align_buf = memalign( HUGE_PAGE_SIZE, i_size );
        if( align_buf )
        {
            /* Round up to the next huge page boundary if we are close enough. */
            size_t madv_size = (i_size + HUGE_PAGE_SIZE - HUGE_PAGE_THRESHOLD) & ~(HUGE_PAGE_SIZE-1);
            madvise( align_buf, madv_size, MADV_HUGEPAGE );
        }
    }
    else
#undef HUGE_PAGE_SIZE
#undef HUGE_PAGE_THRESHOLD
#endif
        align_buf = memalign( NATIVE_ALIGN, i_size );
#else
    uint8_t *buf = malloc( i_size + (NATIVE_ALIGN-1) + sizeof(void **) );
    if( buf )
    {
        align_buf = buf + (NATIVE_ALIGN-1) + sizeof(void **);
        align_buf -= (intptr_t) align_buf & (NATIVE_ALIGN-1);
        *( (void **) ( align_buf - sizeof(void **) ) ) = buf;
    }
#endif
    if( !align_buf )
        x264_log( NULL, X264_LOG_ERROR, "malloc of size %d failed\n", i_size );
    return align_buf;
}

/****************************************************************************
 * x264_free:
 ****************************************************************************/
void x264_free( void *p )
{
    if( p )
    {
#if HAVE_MALLOC_H
        free( p );
#else
        free( *( ( ( void **) p ) - 1 ) );
#endif
    }
}



/*
 *    Description: 检查当前系统类型是大端还是小端(intel)
 */
#if 0
#include <stdio.h>
int checkSystem()
{
    union check
    {
        int i;
        char ch;
    }c;
    c.i=1;
    return (c.ch==1);
}
int checkSystem2()
{
    int i=1;
    return ((i>>24&&15)==0);
}
int main(int argc,char*argv[])
{
    if(checkSystem2())
        printf("littleEndian\n");
    else
        printf("bigEndian\n");
    return 0;
}

#endif
