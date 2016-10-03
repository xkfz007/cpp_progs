#ifndef _FX_MEMOPS_H
#define _FX_MEMOPS_H

#include <stdlib.h>
#include <string.h>
#include "fx_types.h"
#include "fx_macros.h"

//#ifdef _MSC_VER
//#define DECLARE_ALIGNED( var, n ) __declspec(align(n)) var
//#else
//#define DECLARE_ALIGNED( var, n ) var __attribute__((aligned(n)))
//#endif
//#define ALIGNED_32( var ) DECLARE_ALIGNED( var, 32 )
//#define ALIGNED_16( var ) DECLARE_ALIGNED( var, 16 )
//#define ALIGNED_8( var )  DECLARE_ALIGNED( var, 8 )
//#define ALIGNED_4( var )  DECLARE_ALIGNED( var, 4 )


///* For AVX2 */
//#if ARCH_X86 || ARCH_X86_64
//#define NATIVE_ALIGN 32
//#define ALIGNED_N ALIGNED_32
//#define ALIGNED_ARRAY_N ALIGNED_ARRAY_32
//#else
//#define NATIVE_ALIGN 16
//#define ALIGNED_N ALIGNED_16
//#define ALIGNED_ARRAY_N ALIGNED_ARRAY_16
//#endif
//
//
//static void *fx_malloc( int i_size )
//{
//    uint8_t *align_buf = NULL;
//    uint8_t *buf = malloc( i_size + (NATIVE_ALIGN-1) + sizeof(void **) );
//    if( buf )
//    {
//        align_buf = buf + (NATIVE_ALIGN-1) + sizeof(void **);
//        align_buf -= (intptr_t) align_buf & (NATIVE_ALIGN-1);
//        *( (void **) ( align_buf - sizeof(void **) ) ) = buf;
//    }
//    if( !align_buf )
//        fx_log( __FUNCTION__, FX_LOG_ERROR, "malloc of size %d failed\n", i_size );
//    return align_buf;
//}
//
//static void *fx_realloc(void *ptr, size_t size)
//{
//    return realloc(ptr, size + !size);
//}
//
//static void fx_free( void *p )
//{
//    if( p )
//    {
//        free( *( ( ( void **) p ) - 1 ) );
//    }
//}
#define CONFIG_MEMALIGN_HACK 1
#define HAVE_AVX 1
#define NATIVE_ALIGN (HAVE_AVX ? 32 : 16)

void fx_free(void *arg);
void fx_freep(void *arg);

static void *fx_malloc(size_t size)
{
    void *ptr = NULL;
#if CONFIG_MEMALIGN_HACK
    long diff;

    ptr = malloc(size + NATIVE_ALIGN);
    if (!ptr)
        return ptr;
    diff              = ((~(long)ptr)&(NATIVE_ALIGN - 1)) + 1;
    ptr               = (char *)ptr + diff;
    ((char *)ptr)[-1] = diff;
#else
    ptr = malloc(size);
#endif

    return ptr;
}

static void *fx_realloc(void *ptr, size_t size)
{
#if CONFIG_MEMALIGN_HACK
    int diff;

    //FIXME this isn't aligned correctly, though it probably isn't needed
    if (!ptr)
        return fx_malloc(size);
    diff = ((char *)ptr)[-1];
    ASSERT(diff>0 && diff<=NATIVE_ALIGN);
    ptr = realloc((char *)ptr - diff, size + diff);
    if (ptr)
        ptr = (char *)ptr + diff;
    return ptr;
#else
    return realloc(ptr, size + !size);
#endif
}

/**
 * Multiply two `size_t` values checking for overflow.
 *
 * @param[in]  a,b Operands of multiplication
 * @param[out] r   Pointer to the result of the operation
 * @return 0 on success, AVERROR(EINVAL) on overflow
 */
static int fx_size_mult(size_t a, size_t b, size_t *r)
{
    size_t t = a * b;
    /* Hack inspired from glibc: don't try the division if nelem and elsize
     * are both less than sqrt(SIZE_MAX). */
    if ((a | b) >= ((size_t)1 << (sizeof(size_t) * 4)) && a && t / a != b)
        return -1;
    *r = t;
    return 0;
}

static void *fx_realloc_f(void *ptr, size_t nelem, size_t elsize)
{
    size_t size;
    void *r;

    if (fx_size_mult(elsize, nelem, &size)) {
        fx_free(ptr);
        return NULL;
    }
    r = fx_realloc(ptr, size);
    if (!r)
        fx_free(ptr);
    return r;
}

static int fx_reallocp(void *ptr, size_t size)
{
    void *val;

    if (!size) {
        fx_freep(ptr);
        return 0;
    }

    memcpy(&val, ptr, sizeof(val));
    val = fx_realloc(val, size);

    if (!val) {
        fx_freep(ptr);
        return -1;
    }

    memcpy(ptr, &val, sizeof(val));
    return 0;
}

static void *fx_realloc_array(void *ptr, size_t nmemb, size_t size)
{
    if (!size || nmemb >= INT_MAX / size)
        return NULL;
    return fx_realloc(ptr, nmemb * size);
}

static int fx_reallocp_array(void *ptr, size_t nmemb, size_t size)
{
    void *val;

    memcpy(&val, ptr, sizeof(val));
    val = fx_realloc_f(val, nmemb, size);
    memcpy(ptr, &val, sizeof(val));
    if (!val && nmemb && size)
        return -1;

    return 0;
}

static void fx_free(void *ptr)
{
#if CONFIG_MEMALIGN_HACK
    if (ptr) {
        int v= ((char *)ptr)[-1];
        ASSERT(v>0 && v<=NATIVE_ALIGN);
        free((char *)ptr - v);
    }
#else
    free(ptr);
#endif
}

static void fx_freep(void *arg)
{
    void *val;

    memcpy(&val, arg, sizeof(val));
    memcpy(arg, &(void *){ NULL }, sizeof(val));
    fx_free(val);
}

static void *fx_mallocz(size_t size)
{
    void *ptr = fx_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

static void *fx_calloc(size_t nmemb, size_t size)
{
    if (size <= 0 || nmemb >= INT_MAX / size)
        return NULL;
    return fx_mallocz(nmemb * size);
}
/**
 * Allocate a memory block for an array with fx_mallocz().
 *
 * The allocated memory will have size `size * nmemb` bytes.
 *
 * @param nmemb Number of elements
 * @param size  Size of the single element
 * @return Pointer to the allocated block, or `NULL` if the block cannot
 *         be allocated
 *
 * @see fx_mallocz()
 * @see fx_malloc_array()
 */
static void *fx_mallocz_array(size_t nmemb, size_t size)
{
    if (!size || nmemb >= INT_MAX / size)
        return NULL;
    return fx_mallocz(nmemb * size);
}

/**
 * Duplicate a string.
 *
 * @param s String to be duplicated
 * @return Pointer to a newly-allocated string containing a
 *         copy of `s` or `NULL` if the string cannot be allocated
 * @see fx_strndup()
 */
static char *fx_strdup(const char *s)
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

/**
 * Duplicate a substring of a string.
 *
 * @param s   String to be duplicated
 * @param len Maximum length of the resulting string (not counting the
 *            terminating byte)
 * @return Pointer to a newly-allocated string containing a
 *         substring of `s` or `NULL` if the string cannot be allocated
 */
static char *fx_strndup(const char *s, size_t len)
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

/**
 * Duplicate a buffer with fx_malloc().
 *
 * @param p    Buffer to be duplicated
 * @param size Size in bytes of the buffer copied
 * @return Pointer to a newly allocated buffer containing a
 *         copy of `p` or `NULL` if the buffer cannot be allocated
 */
static void *fx_memdup(const void *p, size_t size)
{
    void *ptr = NULL;
    if (p) {
        ptr = fx_malloc(size);
        if (ptr)
            memcpy(ptr, p, size);
    }
    return ptr;
}


/**
 * Add an element of to a dynamic array.
 *
 * The array is reallocated when its number of elements reaches powers of 2.
 * Therefore, the amortized cost of adding an element is constant.
 *
 * In case of success, the pointer to the array is updated in order to
 * point to the new grown array, and the size is incremented.
 *
 * @param fx_size_max  maximum size of the array, usually the MAX macro of
 *                     the type of the size
 * @param fx_elt_size  size of the elements in the array, in bytes
 * @param fx_array     pointer to the array, must be a lvalue
 * @param fx_size      size of the array, must be an integer lvalue
 * @param fx_success   statement to execute on success; at this point, the
 *                     size variable is not yet incremented
 * @param fx_failure   statement to execute on failure; if this happens, the
 *                     array and size are not changed; the statement can end
 *                     with a return or a goto
 */
#define FX_DYNARRAY_ADD(fx_size_max, fx_elt_size, fx_array, fx_size, \
                        fx_success, fx_failure) \
    do { \
        size_t fx_size_new = (fx_size); \
        if (!((fx_size) & ((fx_size) - 1))) { \
            fx_size_new = (fx_size) ? (fx_size) << 1 : 1; \
            if (fx_size_new > (fx_size_max) / (fx_elt_size)) { \
                fx_size_new = 0; \
            } else { \
                void *fx_array_new = \
                    fx_realloc((fx_array), fx_size_new * (fx_elt_size)); \
                if (!fx_array_new) \
                    fx_size_new = 0; \
                else \
                    (fx_array) = fx_array_new; \
            } \
        } \
        if (fx_size_new) { \
            { fx_success } \
            (fx_size)++; \
        } else { \
            fx_failure \
        } \
    } while (0)

static int fx_dynarray_add_nofree(void *tab_ptr, int *nb_ptr, void *elem)
{
    void **tab;
    memcpy(&tab, tab_ptr, sizeof(tab));

    FX_DYNARRAY_ADD(INT_MAX, sizeof(*tab), tab, *nb_ptr, {
        tab[*nb_ptr] = elem;
        memcpy(tab_ptr, &tab, sizeof(tab));
    }, {
        return -1;
    });
    return 0;
}

void fx_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem)
{
    void **tab;
    memcpy(&tab, tab_ptr, sizeof(tab));

    FX_DYNARRAY_ADD(INT_MAX, sizeof(*tab), tab, *nb_ptr, {
        tab[*nb_ptr] = elem;
        memcpy(tab_ptr, &tab, sizeof(tab));
    }, {
        *nb_ptr = 0;
        fx_freep(tab_ptr);
    });
}

void *fx_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size,
                       const uint8_t *elem_data)
{
    uint8_t *tab_elem_data = NULL;

    FX_DYNARRAY_ADD(INT_MAX, elem_size, *tab_ptr, *nb_ptr, {
        tab_elem_data = (uint8_t *)*tab_ptr + (*nb_ptr) * elem_size;
        if (elem_data)
            memcpy(tab_elem_data, elem_data, elem_size);
    }, {
        fx_freep(tab_ptr);
        *nb_ptr = 0;
    });
    return tab_elem_data;
}



#endif
