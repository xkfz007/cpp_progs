/*****************************************************************************
 * win32thread.c: windows threading
 *****************************************************************************
 * Copyright (C) 2010-2015 x264 project
 *
 * Authors: Steven Walters <kemuri9@gmail.com>
 *          Pegasys Inc. <http://www.pegasys-inc.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

/* Microsoft's way of supporting systems with >64 logical cpus can be found at
 * http://www.microsoft.com/whdc/system/Sysinternals/MoreThan64proc.mspx */

/* Based on the agreed standing that x264 does not need to utilize >64 logical cpus,
 * this API does not detect nor utilize more than 64 cpus for systems that have them. */

#if HAVE_WIN32THREAD
#include <process.h>

/* number of times to spin a thread about to block on a locked mutex before retrying and sleeping if still locked */
#define fx_SPIN_COUNT 0

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

int fx_pthread_create( fx_pthread_t *thread, const fx_pthread_attr_t *attr,
                         void *(*start_routine)( void* ), void *arg )
{
    thread->func   = start_routine;
    thread->arg    = arg;
    thread->p_ret  = &thread->ret;
    thread->ret    = NULL;
    thread->handle = (void*)_beginthreadex( NULL, 0, fx_win32thread_worker, thread, 0, NULL );
    return !thread->handle;
}

int fx_pthread_join( fx_pthread_t thread, void **value_ptr )
{
    DWORD ret = WaitForSingleObject( thread.handle, INFINITE );
    if( ret != WAIT_OBJECT_0 )
        return -1;
    if( value_ptr )
        *value_ptr = *thread.p_ret;
    CloseHandle( thread.handle );
    return 0;
}

int fx_pthread_mutex_init( fx_pthread_mutex_t *mutex, const fx_pthread_mutexattr_t *attr )
{
    return !InitializeCriticalSectionAndSpinCount( mutex, fx_SPIN_COUNT );
}

int fx_pthread_mutex_destroy( fx_pthread_mutex_t *mutex )
{
    DeleteCriticalSection( mutex );
    return 0;
}

int fx_pthread_mutex_lock( fx_pthread_mutex_t *mutex )
{
    static fx_pthread_mutex_t init = fx_PTHREAD_MUTEX_INITIALIZER;
    if( !memcmp( mutex, &init, sizeof(fx_pthread_mutex_t) ) )
        *mutex = thread_control.static_mutex;
    EnterCriticalSection( mutex );
    return 0;
}

int fx_pthread_mutex_unlock( fx_pthread_mutex_t *mutex )
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

int fx_pthread_cond_init( fx_pthread_cond_t *cond, const fx_pthread_condattr_t *attr )
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

int fx_pthread_cond_destroy( fx_pthread_cond_t *cond )
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

int fx_pthread_cond_broadcast( fx_pthread_cond_t *cond )
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

int fx_pthread_cond_signal( fx_pthread_cond_t *cond )
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

int fx_pthread_cond_wait( fx_pthread_cond_t *cond, fx_pthread_mutex_t *mutex )
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

int fx_win32_threading_init( void )
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

void fx_win32_threading_destroy( void )
{
    fx_pthread_mutex_destroy( &thread_control.static_mutex );
    memset( &thread_control, 0, sizeof(fx_win32thread_control_t) );
}

int fx_pthread_num_processors_np( void )
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

int fx_threading_init( void )
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