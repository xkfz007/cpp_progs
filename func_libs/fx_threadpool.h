/*****************************************************************************
 * threadpool.h: thread pooling
 *****************************************************************************/

#ifndef FX_THREADPOOL_H
#define FX_THREADPOOL_H
#include "fx_thread.h"
#include "fx_memops.h"
#define HAVE_THREAD 1
typedef struct fx_threadpool_t fx_threadpool_t;

#if HAVE_THREAD
int   fx_threadpool_init( fx_threadpool_t **p_pool, int threads,
                            void (*init_func)(void *), void *init_arg );
void  fx_threadpool_run( fx_threadpool_t *pool, void *(*func)(void *), void *arg );
void *fx_threadpool_wait( fx_threadpool_t *pool, void *arg );
void  fx_threadpool_delete( fx_threadpool_t *pool );
#else
#define fx_threadpool_init(p,t,f,a) -1
#define fx_threadpool_run(p,f,a)
#define fx_threadpool_wait(p,a)     NULL
#define fx_threadpool_delete(p)
#endif

#define fx_stack_align(func,...) func(__VA_ARGS__)


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

typedef struct fx_frame{
	int i_frame;//frame no
	void *data;
	void(*frame_new)();
	void(*frame_delete)();
} fx_frame_t;

/* synchronized frame list */
typedef struct
{
   fx_frame_t **list;
   int i_max_size;
   int i_size;
   fx_pthread_mutex_t     mutex;
   fx_pthread_cond_t      cv_fill;  /* event signaling that the list became fuller */
   fx_pthread_cond_t      cv_empty; /* event signaling that the list became emptier */
} fx_sync_frame_list_t;


typedef struct
{
    void *(*func)(void *);
    void *arg;
    void *ret;
} fx_threadpool_job_t;

struct fx_threadpool_t
{
    int            exit;
    int            threads;
    fx_pthread_t *thread_handle;
    void           (*init_func)(void *);
    void           *init_arg;

    /* requires a synchronized list structure and associated methods,
       so use what is already implemented for frames */
    fx_sync_frame_list_t uninit; /* list of jobs that are awaiting use */
    fx_sync_frame_list_t run;    /* list of jobs that are queued for processing by the pool */
    fx_sync_frame_list_t done;   /* list of jobs that have finished processing */
};

static fx_frame_t *fx_frame_shift( fx_frame_t **list )
{
    fx_frame_t *frame = list[0];
    int i;
    for( i = 0; list[i]; i++ )
        list[i] = list[i+1];
    assert(frame);
    return frame;
}

static void fx_frame_delete_list( fx_frame_t **list )
{
    int i = 0;
    if( !list )
        return;
    while( list[i] )
        list[i++]->frame_delete();
    fx_free( list );
}

static int fx_sync_frame_list_init( fx_sync_frame_list_t *slist, int max_size )
{
    if( max_size < 0 )
        return -1;
    slist->i_max_size = max_size;
    slist->i_size = 0;
    FX_CHECKED_MALLOCZERO( slist->list, (max_size+1) * sizeof(fx_frame_t*) );
    if( fx_pthread_mutex_init( &slist->mutex, NULL ) ||
        fx_pthread_cond_init( &slist->cv_fill, NULL ) ||
        fx_pthread_cond_init( &slist->cv_empty, NULL ) )
        return -1;
    return 0;
fail:
    return -1;
}

static void fx_sync_frame_list_delete( fx_sync_frame_list_t *slist )
{
    fx_pthread_mutex_destroy( &slist->mutex );
    fx_pthread_cond_destroy( &slist->cv_fill );
    fx_pthread_cond_destroy( &slist->cv_empty );
    fx_frame_delete_list( slist->list );
}

static void fx_sync_frame_list_push( fx_sync_frame_list_t *slist, fx_frame_t *frame )
{
    fx_pthread_mutex_lock( &slist->mutex );
    while( slist->i_size == slist->i_max_size )
        fx_pthread_cond_wait( &slist->cv_empty, &slist->mutex );
    slist->list[ slist->i_size++ ] = frame;
    fx_pthread_mutex_unlock( &slist->mutex );
    fx_pthread_cond_broadcast( &slist->cv_fill );
}

static fx_frame_t *fx_sync_frame_list_pop( fx_sync_frame_list_t *slist )
{
    fx_frame_t *frame;
    fx_pthread_mutex_lock( &slist->mutex );
    while( !slist->i_size )
        fx_pthread_cond_wait( &slist->cv_fill, &slist->mutex );
    frame = slist->list[ --slist->i_size ];
    slist->list[ slist->i_size ] = NULL;
    fx_pthread_cond_broadcast( &slist->cv_empty );
    fx_pthread_mutex_unlock( &slist->mutex );
    return frame;
}
static void *fx_threadpool_thread( fx_threadpool_t *pool )
{
    if( pool->init_func )
        pool->init_func( pool->init_arg );

    while( !pool->exit )
    {
        fx_threadpool_job_t *job = NULL;
        fx_pthread_mutex_lock( &pool->run.mutex );
        while( !pool->exit && !pool->run.i_size )
            fx_pthread_cond_wait( &pool->run.cv_fill, &pool->run.mutex );
        if( pool->run.i_size )
        {
            job = (void*)fx_frame_shift( pool->run.list );
            pool->run.i_size--;
        }
        fx_pthread_mutex_unlock( &pool->run.mutex );
        if( !job )
            continue;
        job->ret = (void*)fx_stack_align( job->func, job->arg ); /* execute the function */
        fx_sync_frame_list_push( &pool->done, (void*)job );
    }
    return NULL;
}

static int fx_threadpool_init( fx_threadpool_t **p_pool, int threads,
                          void (*init_func)(void *), void *init_arg )
{
    if( threads <= 0 )
        return -1;

    fx_threadpool_t *pool;
    FX_CHECKED_MALLOCZERO( pool, sizeof(fx_threadpool_t) );
    *p_pool = pool;

    pool->init_func = init_func;
    pool->init_arg  = init_arg;
    pool->threads   = threads;

    FX_CHECKED_MALLOC( pool->thread_handle, pool->threads * sizeof(fx_pthread_t) );

    if( fx_sync_frame_list_init( &pool->uninit, pool->threads ) ||
        fx_sync_frame_list_init( &pool->run, pool->threads ) ||
        fx_sync_frame_list_init( &pool->done, pool->threads ) )
        goto fail;

    for( int i = 0; i < pool->threads; i++ )
    {
       fx_threadpool_job_t *job;
       FX_CHECKED_MALLOC( job, sizeof(fx_threadpool_job_t) );
       fx_sync_frame_list_push( &pool->uninit, (void*)job );
    }
    for( int i = 0; i < pool->threads; i++ )
        if( fx_pthread_create( pool->thread_handle+i, NULL, (void*)fx_threadpool_thread, pool ) )
            goto fail;

    return 0;
fail:
    return -1;
}

static void fx_threadpool_run( fx_threadpool_t *pool, void *(*func)(void *), void *arg )
{
    fx_threadpool_job_t *job = (void*)fx_sync_frame_list_pop( &pool->uninit );
    job->func = func;
    job->arg  = arg;
    fx_sync_frame_list_push( &pool->run, (void*)job );
}

static void *fx_threadpool_wait( fx_threadpool_t *pool, void *arg )
{
    fx_threadpool_job_t *job = NULL;

    fx_pthread_mutex_lock( &pool->done.mutex );
    while( !job )
    {
        for( int i = 0; i < pool->done.i_size; i++ )
        {
            fx_threadpool_job_t *t = (void*)pool->done.list[i];
            if( t->arg == arg )
            {
                job = (void*)fx_frame_shift( pool->done.list+i );
                pool->done.i_size--;
            }
        }
        if( !job )
            fx_pthread_cond_wait( &pool->done.cv_fill, &pool->done.mutex );
    }
    fx_pthread_mutex_unlock( &pool->done.mutex );

    void *ret = job->ret;
    fx_sync_frame_list_push( &pool->uninit, (void*)job );
    return ret;
}

static void fx_threadpool_list_delete( fx_sync_frame_list_t *slist )
{
    for( int i = 0; slist->list[i]; i++ )
    {
        fx_free( slist->list[i] );
        slist->list[i] = NULL;
    }
    fx_sync_frame_list_delete( slist );
}

static void fx_threadpool_delete( fx_threadpool_t *pool )
{
    fx_pthread_mutex_lock( &pool->run.mutex );
    pool->exit = 1;
    fx_pthread_cond_broadcast( &pool->run.cv_fill );
    fx_pthread_mutex_unlock( &pool->run.mutex );
    for( int i = 0; i < pool->threads; i++ )
        fx_pthread_join( pool->thread_handle[i], NULL );

    fx_threadpool_list_delete( &pool->uninit );
    fx_threadpool_list_delete( &pool->run );
    fx_threadpool_list_delete( &pool->done );
    fx_free( pool->thread_handle );
    fx_free( pool );
}

#endif
