#ifndef _FX_THREAD_H
#define _FX_THREAD_H
#include "fx_funcproperties.h"
#define HAVE_WIN32THREAD  1
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

#if HAVE_WIN32THREAD
#include <process.h>

/* number of times to spin a thread about to block on a locked mutex before retrying and sleeping if still locked */
#define FX_SPIN_COUNT 0

/* GROUP_AFFINITY struct */
typedef struct
{
    ULONG_PTR mask; // KAFFINITY = ULONG_PTR
    USHORT group;
    USHORT reserved[3];
} fx_group_affinity_t;

typedef struct
{
    /* global mutex for replacing MUTEX_INITIALIZER instances */
    fx_pthread_mutex_t static_mutex;

    /* function pointers to conditional variable API on windows 6.0+ kernels */
    void (WINAPI *cond_broadcast)( fx_pthread_cond_t *cond );
    void (WINAPI *cond_init)( fx_pthread_cond_t *cond );
    void (WINAPI *cond_signal)( fx_pthread_cond_t *cond );
    BOOL (WINAPI *cond_wait)( fx_pthread_cond_t *cond, fx_pthread_mutex_t *mutex, DWORD milliseconds );
} fx_win32thread_control_t;

static fx_win32thread_control_t thread_control;

/* _beginthreadex requires that the start routine is __stdcall */
static unsigned __stdcall fx_win32thread_worker( void *arg )
{
    fx_pthread_t *h = arg;
    *h->p_ret = h->func( h->arg );
    return 0;
}

static int fx_pthread_create( fx_pthread_t *thread, const fx_pthread_attr_t *attr,
                         void *(*start_routine)( void* ), void *arg )
{
    thread->func   = start_routine;
    thread->arg    = arg;
    thread->p_ret  = &thread->ret;
    thread->ret    = NULL;
    thread->handle = (void*)_beginthreadex( NULL, 0, fx_win32thread_worker, thread, 0, NULL );
    return !thread->handle;
}

static int fx_pthread_join( fx_pthread_t thread, void **value_ptr )
{
    DWORD ret = WaitForSingleObject( thread.handle, INFINITE );
    if( ret != WAIT_OBJECT_0 )
        return -1;
    if( value_ptr )
        *value_ptr = *thread.p_ret;
    CloseHandle( thread.handle );
    return 0;
}

static int fx_pthread_mutex_init( fx_pthread_mutex_t *mutex, const fx_pthread_mutexattr_t *attr )
{
    return !InitializeCriticalSectionAndSpinCount( mutex, FX_SPIN_COUNT );
}

static int fx_pthread_mutex_destroy( fx_pthread_mutex_t *mutex )
{
    DeleteCriticalSection( mutex );
    return 0;
}

static int fx_pthread_mutex_lock( fx_pthread_mutex_t *mutex )
{
    static fx_pthread_mutex_t init = fx_PTHREAD_MUTEX_INITIALIZER;
    if( !memcmp( mutex, &init, sizeof(fx_pthread_mutex_t) ) )
        *mutex = thread_control.static_mutex;
    EnterCriticalSection( mutex );
    return 0;
}

static int fx_pthread_mutex_unlock( fx_pthread_mutex_t *mutex )
{
    LeaveCriticalSection( mutex );
    return 0;
}

/* for pre-Windows 6.0 platforms we need to define and use our own condition variable and api */
typedef struct
{
    fx_pthread_mutex_t mtx_broadcast;
    fx_pthread_mutex_t mtx_waiter_count;
    volatile int waiter_count;
    HANDLE semaphore;
    HANDLE waiters_done;
    volatile int is_broadcast;
} fx_win32_cond_t;

static int fx_pthread_cond_init( fx_pthread_cond_t *cond, const fx_pthread_condattr_t *attr )
{
    if( thread_control.cond_init )
    {
        thread_control.cond_init( cond );
        return 0;
    }

    /* non native condition variables */
    fx_win32_cond_t *win32_cond = calloc( 1, sizeof(fx_win32_cond_t) );
    if( !win32_cond )
        return -1;
    cond->ptr = win32_cond;
    win32_cond->semaphore = CreateSemaphore( NULL, 0, 0x7fffffff, NULL );
    if( !win32_cond->semaphore )
        return -1;

    if( fx_pthread_mutex_init( &win32_cond->mtx_waiter_count, NULL ) )
        return -1;
    if( fx_pthread_mutex_init( &win32_cond->mtx_broadcast, NULL ) )
        return -1;

    win32_cond->waiters_done = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( !win32_cond->waiters_done )
        return -1;

    return 0;
}

static int fx_pthread_cond_destroy( fx_pthread_cond_t *cond )
{
    /* native condition variables do not destroy */
    if( thread_control.cond_init )
        return 0;

    /* non native condition variables */
    fx_win32_cond_t *win32_cond = cond->ptr;
    CloseHandle( win32_cond->semaphore );
    CloseHandle( win32_cond->waiters_done );
    fx_pthread_mutex_destroy( &win32_cond->mtx_broadcast );
    fx_pthread_mutex_destroy( &win32_cond->mtx_waiter_count );
    free( win32_cond );

    return 0;
}

static int fx_pthread_cond_broadcast( fx_pthread_cond_t *cond )
{
    if( thread_control.cond_broadcast )
    {
        thread_control.cond_broadcast( cond );
        return 0;
    }

    /* non native condition variables */
    fx_win32_cond_t *win32_cond = cond->ptr;
    fx_pthread_mutex_lock( &win32_cond->mtx_broadcast );
    fx_pthread_mutex_lock( &win32_cond->mtx_waiter_count );
    int have_waiter = 0;

    if( win32_cond->waiter_count )
    {
        win32_cond->is_broadcast = 1;
        have_waiter = 1;
    }

    if( have_waiter )
    {
        ReleaseSemaphore( win32_cond->semaphore, win32_cond->waiter_count, NULL );
        fx_pthread_mutex_unlock( &win32_cond->mtx_waiter_count );
        WaitForSingleObject( win32_cond->waiters_done, INFINITE );
        win32_cond->is_broadcast = 0;
    }
    else
        fx_pthread_mutex_unlock( &win32_cond->mtx_waiter_count );
    return fx_pthread_mutex_unlock( &win32_cond->mtx_broadcast );
}

static int fx_pthread_cond_signal( fx_pthread_cond_t *cond )
{
    if( thread_control.cond_signal )
    {
        thread_control.cond_signal( cond );
        return 0;
    }

    /* non-native condition variables */
    fx_win32_cond_t *win32_cond = cond->ptr;

    fx_pthread_mutex_lock( &win32_cond->mtx_broadcast );
    fx_pthread_mutex_lock( &win32_cond->mtx_waiter_count );
    int have_waiter = win32_cond->waiter_count;
    fx_pthread_mutex_unlock( &win32_cond->mtx_waiter_count );

    if( have_waiter )
    {
        ReleaseSemaphore( win32_cond->semaphore, 1, NULL );
        WaitForSingleObject( win32_cond->waiters_done, INFINITE );
    }

    return fx_pthread_mutex_unlock( &win32_cond->mtx_broadcast );
}

static int fx_pthread_cond_wait( fx_pthread_cond_t *cond, fx_pthread_mutex_t *mutex )
{
    if( thread_control.cond_wait )
        return !thread_control.cond_wait( cond, mutex, INFINITE );

    /* non native condition variables */
    fx_win32_cond_t *win32_cond = cond->ptr;

    fx_pthread_mutex_lock( &win32_cond->mtx_broadcast );
    fx_pthread_mutex_lock( &win32_cond->mtx_waiter_count );
    win32_cond->waiter_count++;
    fx_pthread_mutex_unlock( &win32_cond->mtx_waiter_count );
    fx_pthread_mutex_unlock( &win32_cond->mtx_broadcast );

    // unlock the external mutex
    fx_pthread_mutex_unlock( mutex );
    WaitForSingleObject( win32_cond->semaphore, INFINITE );

    fx_pthread_mutex_lock( &win32_cond->mtx_waiter_count );
    win32_cond->waiter_count--;
    int last_waiter = !win32_cond->waiter_count || !win32_cond->is_broadcast;
    fx_pthread_mutex_unlock( &win32_cond->mtx_waiter_count );

    if( last_waiter )
        SetEvent( win32_cond->waiters_done );

    // lock the external mutex
    return fx_pthread_mutex_lock( mutex );
}

static int fx_win32_threading_init( void )
{
    /* find function pointers to API functions, if they exist */
    HANDLE kernel_dll = GetModuleHandleW( L"kernel32.dll" );
    thread_control.cond_init = (void*)GetProcAddress( kernel_dll, "InitializeConditionVariable" );
    if( thread_control.cond_init )
    {
        /* we're on a windows 6.0+ kernel, acquire the rest of the functions */
        thread_control.cond_broadcast = (void*)GetProcAddress( kernel_dll, "WakeAllConditionVariable" );
        thread_control.cond_signal = (void*)GetProcAddress( kernel_dll, "WakeConditionVariable" );
        thread_control.cond_wait = (void*)GetProcAddress( kernel_dll, "SleepConditionVariableCS" );
    }
    return fx_pthread_mutex_init( &thread_control.static_mutex, NULL );
}

static void fx_win32_threading_destroy( void )
{
    fx_pthread_mutex_destroy( &thread_control.static_mutex );
    memset( &thread_control, 0, sizeof(fx_win32thread_control_t) );
}

static int fx_pthread_num_processors_np( void )
{
    DWORD_PTR system_cpus, process_cpus = 0;
    int cpus = 0;

    /* GetProcessAffinityMask returns affinities of 0 when the process has threads in multiple processor groups.
     * On platforms that support processor grouping, use GetThreadGroupAffinity to get the current thread's affinity instead. */
#if ARCH_X86_64
    /* find function pointers to API functions specific to x86_64 platforms, if they exist */
    HANDLE kernel_dll = GetModuleHandleW( L"kernel32.dll" );
    BOOL (*get_thread_affinity)( HANDLE thread, fx_group_affinity_t *group_affinity ) = (void*)GetProcAddress( kernel_dll, "GetThreadGroupAffinity" );
    if( get_thread_affinity )
    {
        /* running on a platform that supports >64 logical cpus */
        fx_group_affinity_t thread_affinity;
        if( get_thread_affinity( GetCurrentThread(), &thread_affinity ) )
            process_cpus = thread_affinity.mask;
    }
#endif
    if( !process_cpus )
        GetProcessAffinityMask( GetCurrentProcess(), &process_cpus, &system_cpus );
    for( DWORD_PTR bit = 1; bit; bit <<= 1 )
        cpus += !!(process_cpus & bit);

    return cpus ? cpus : 1;
}
#endif

#if HAVE_WIN32THREAD || PTW32_STATIC_LIB
/* state of the threading library being initialized */
static volatile LONG fx_threading_is_init = 0;

static void fx_threading_destroy( void )
{
#if PTW32_STATIC_LIB
    pthread_win32_thread_detach_np();
    pthread_win32_process_detach_np();
#else
    fx_win32_threading_destroy();
#endif
}

static int fx_threading_init( void )
{
    /* if already init, then do nothing */
    if( InterlockedCompareExchange( &fx_threading_is_init, 1, 0 ) )
        return 0;
#if PTW32_STATIC_LIB
    /* if static pthread-win32 is already initialized, then do nothing */
    if( ptw32_processInitialized )
        return 0;
    if( !pthread_win32_process_attach_np() )
        return -1;
#else
    if( fx_win32_threading_init() )
        return -1;
#endif
    /* register cleanup to run at process termination */
    atexit( fx_threading_destroy );

    return 0;
}
#endif

#endif