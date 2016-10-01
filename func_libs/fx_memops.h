#ifndef _FX_MEMOPS_H
#define _FX_MEMOPS_H


#ifdef _MSC_VER
#define DECLARE_ALIGNED( var, n ) __declspec(align(n)) var
#else
#define DECLARE_ALIGNED( var, n ) var __attribute__((aligned(n)))
#endif
#define ALIGNED_32( var ) DECLARE_ALIGNED( var, 32 )
#define ALIGNED_16( var ) DECLARE_ALIGNED( var, 16 )
#define ALIGNED_8( var )  DECLARE_ALIGNED( var, 8 )
#define ALIGNED_4( var )  DECLARE_ALIGNED( var, 4 )


/* For AVX2 */
#if ARCH_X86 || ARCH_X86_64
#define NATIVE_ALIGN 32
#define ALIGNED_N ALIGNED_32
#define ALIGNED_ARRAY_N ALIGNED_ARRAY_32
#else
#define NATIVE_ALIGN 16
#define ALIGNED_N ALIGNED_16
#define ALIGNED_ARRAY_N ALIGNED_ARRAY_16
#endif


#include "fx_types.h"
#include "fx_log.h"
/****************************************************************************
 * fx_malloc:
 ****************************************************************************/
static void *fx_malloc( int i_size )
{
    uint8_t *align_buf = NULL;
    uint8_t *buf = malloc( i_size + (NATIVE_ALIGN-1) + sizeof(void **) );
    if( buf )
    {
        align_buf = buf + (NATIVE_ALIGN-1) + sizeof(void **);
        align_buf -= (intptr_t) align_buf & (NATIVE_ALIGN-1);
        *( (void **) ( align_buf - sizeof(void **) ) ) = buf;
    }
    if( !align_buf )
        fx_log( __FUNCTION__, FX_LOG_ERROR, "malloc of size %d failed\n", i_size );
    return align_buf;
}

static void *fx_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size + !size);
}

/****************************************************************************
 * fx_free:
 ****************************************************************************/
static void fx_free( void *p )
{
    if( p )
    {
        free( *( ( ( void **) p ) - 1 ) );
    }
}



#define FX_CHECKED_MALLOC( var, size )\
do {\
    var = fx_malloc( size );\
    if( !var )\
        goto fail;\
} while( 0 )

#define FX_CHECKED_MALLOCZERO( var, size )\
do {\
    FX_CHECKED_MALLOC( var, size );\
    memset( var, 0, size );\
} while( 0 )


void *fx_mallocz(size_t size)
{
    void *ptr = fx_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void *fx_calloc(size_t nmemb, size_t size)
{
    if (size <= 0 || nmemb >= INT_MAX / size)
        return NULL;
    return fx_mallocz(nmemb * size);
}

char *fx_strdup(const char *s)
{
    char *ptr = NULL;
    if (s) {
        size_t len = strlen(s) + 1;
        ptr = fx_realloc(NULL, len);
        if (ptr)
            memcpy(ptr, s, len);
    }
    return ptr;
}

char *fx_strndup(const char *s, size_t len)
{
    char *ret = NULL, *end;

    if (!s)
        return NULL;

    end = memchr(s, 0, len);
    if (end)
        len = end - s;

    ret = fx_realloc(NULL, len + 1);
    if (!ret)
        return NULL;

    memcpy(ret, s, len);
    ret[len] = 0;
    return ret;
}

void *fx_memdup(const void *p, size_t size)
{
    void *ptr = NULL;
    if (p) {
        ptr = fx_malloc(size);
        if (ptr)
            memcpy(ptr, p, size);
    }
    return ptr;
}


#endif
