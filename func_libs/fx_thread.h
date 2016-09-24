#ifndef _FX_THREAD_H
#define _FX_THREAD_H
#include "fx_funcproperties.h"
/* threads */
#if HAVE_BEOSTHREAD
#include <kernel/OS.h>
#define fx_pthread_t               thread_id
static inline int fx_pthread_create( fx_pthread_t *t, void *a, void *(*f)(void *), void *d )
{
     *t = spawn_thread( f, "", 10, d );
     if( *t < B_NO_ERROR )
         return -1;
     resume_thread( *t );
     return 0;
}
#define fx_pthread_join(t,s)       { long tmp; \
                                       wait_for_thread(t,(s)?(long*)(s):&tmp); }

#elif HAVE_POSIXTHREAD
#include <pthread.h>
#define fx_pthread_t               pthread_t
#define fx_pthread_create          pthread_create
#define fx_pthread_join            pthread_join
#define fx_pthread_mutex_t         pthread_mutex_t
#define fx_pthread_mutex_init      pthread_mutex_init
#define fx_pthread_mutex_destroy   pthread_mutex_destroy
#define fx_pthread_mutex_lock      pthread_mutex_lock
#define fx_pthread_mutex_unlock    pthread_mutex_unlock
#define fx_pthread_cond_t          pthread_cond_t
#define fx_pthread_cond_init       pthread_cond_init
#define fx_pthread_cond_destroy    pthread_cond_destroy
#define fx_pthread_cond_broadcast  pthread_cond_broadcast
#define fx_pthread_cond_wait       pthread_cond_wait
#define fx_pthread_attr_t          pthread_attr_t
#define fx_pthread_attr_init       pthread_attr_init
#define fx_pthread_attr_destroy    pthread_attr_destroy
#define fx_pthread_num_processors_np pthread_num_processors_np
#define fx_PTHREAD_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

#elif HAVE_WIN32THREAD
//#include "win32thread.h"
#include <windows.h>
/* the following macro is used within x264 */
#undef ERROR

typedef struct
{
    void *handle;
    void *(*func)( void* arg );
    void *arg;
    void **p_ret;
    void *ret;
} fx_pthread_t;
#define fx_pthread_attr_t int

/* the conditional variable api for windows 6.0+ uses critical sections and not mutexes */
typedef CRITICAL_SECTION fx_pthread_mutex_t;
#define fx_PTHREAD_MUTEX_INITIALIZER {0}
#define fx_pthread_mutexattr_t int

/* This is the CONDITIONAL_VARIABLE typedef for using Window's native conditional variables on kernels 6.0+.
 * MinGW does not currently have this typedef. */
typedef struct
{
    void *ptr;
} fx_pthread_cond_t;
#define fx_pthread_condattr_t int

int fx_pthread_create( fx_pthread_t *thread, const fx_pthread_attr_t *attr,
                         void *(*start_routine)( void* ), void *arg );
int fx_pthread_join( fx_pthread_t thread, void **value_ptr );

int fx_pthread_mutex_init( fx_pthread_mutex_t *mutex, const fx_pthread_mutexattr_t *attr );
int fx_pthread_mutex_destroy( fx_pthread_mutex_t *mutex );
int fx_pthread_mutex_lock( fx_pthread_mutex_t *mutex );
int fx_pthread_mutex_unlock( fx_pthread_mutex_t *mutex );

int fx_pthread_cond_init( fx_pthread_cond_t *cond, const fx_pthread_condattr_t *attr );
int fx_pthread_cond_destroy( fx_pthread_cond_t *cond );
int fx_pthread_cond_broadcast( fx_pthread_cond_t *cond );
int fx_pthread_cond_wait( fx_pthread_cond_t *cond, fx_pthread_mutex_t *mutex );
int fx_pthread_cond_signal( fx_pthread_cond_t *cond );

#define fx_pthread_attr_init(a) 0
#define fx_pthread_attr_destroy(a) 0

int  fx_win32_threading_init( void );
void fx_win32_threading_destroy( void );

int fx_pthread_num_processors_np( void );
#else
#define fx_pthread_t               int
#define fx_pthread_create(t,u,f,d) 0
#define fx_pthread_join(t,s)
#endif //HAVE_*THREAD

#if !HAVE_POSIXTHREAD && !HAVE_WIN32THREAD
#define fx_pthread_mutex_t         int
#define fx_pthread_mutex_init(m,f) 0
#define fx_pthread_mutex_destroy(m)
#define fx_pthread_mutex_lock(m)
#define fx_pthread_mutex_unlock(m)
#define fx_pthread_cond_t          int
#define fx_pthread_cond_init(c,f)  0
#define fx_pthread_cond_destroy(c)
#define fx_pthread_cond_broadcast(c)
#define fx_pthread_cond_wait(c,m)
#define fx_pthread_attr_t          int
#define fx_pthread_attr_init(a)    0
#define fx_pthread_attr_destroy(a)
#define fx_PTHREAD_MUTEX_INITIALIZER 0
#endif

#if HAVE_WIN32THREAD || PTW32_STATIC_LIB
int fx_threading_init( void );
#else
#define fx_threading_init() 0
#endif


static ALWAYS_INLINE int fx_pthread_fetch_and_add( int *val, int add, fx_pthread_mutex_t *mutex )
{
#if HAVE_THREAD
#if defined(__GNUC__) && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ > 0) && ARCH_X86
    return __sync_fetch_and_add( val, add );
#else
    fx_pthread_mutex_lock( mutex );
    int res = *val;
    *val += add;
    fx_pthread_mutex_unlock( mutex );
    return res;
#endif
#else
    int res = *val;
    *val += add;
    return res;
#endif
}

#if HAVE_POSIXTHREAD
#if SYS_WINDOWS
#define fx_lower_thread_priority(p)\
{\
    fx_pthread_t handle = pthread_self();\
    struct sched_param sp;\
    int policy = SCHED_OTHER;\
    pthread_getschedparam( handle, &policy, &sp );\
    sp.sched_priority -= p;\
    pthread_setschedparam( handle, policy, &sp );\
}
#else
#include <unistd.h>
#define fx_lower_thread_priority(p) { UNUSED int nice_ret = nice(p); }
#endif /* SYS_WINDOWS */
#elif HAVE_WIN32THREAD
#define fx_lower_thread_priority(p) SetThreadPriority( GetCurrentThread(), fx_MAX( -2, -p ) )
#else
#define fx_lower_thread_priority(p)
#endif


#endif