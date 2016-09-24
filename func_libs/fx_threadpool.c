/*****************************************************************************
 * threadpool.c: thread pooling
 *****************************************************************************
 * Copyright (C) 2010-2015 x264 project
 *
 * Authors: Steven Walters <kemuri9@gmail.com>
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

#include "common.h"

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

int fx_threadpool_init( fx_threadpool_t **p_pool, int threads,
                          void (*init_func)(void *), void *init_arg )
{
    if( threads <= 0 )
        return -1;

    fx_threadpool_t *pool;
    CHECKED_MALLOCZERO( pool, sizeof(fx_threadpool_t) );
    *p_pool = pool;

    pool->init_func = init_func;
    pool->init_arg  = init_arg;
    pool->threads   = threads;

    CHECKED_MALLOC( pool->thread_handle, pool->threads * sizeof(fx_pthread_t) );

    if( fx_sync_frame_list_init( &pool->uninit, pool->threads ) ||
        fx_sync_frame_list_init( &pool->run, pool->threads ) ||
        fx_sync_frame_list_init( &pool->done, pool->threads ) )
        goto fail;

    for( int i = 0; i < pool->threads; i++ )
    {
       fx_threadpool_job_t *job;
       CHECKED_MALLOC( job, sizeof(fx_threadpool_job_t) );
       fx_sync_frame_list_push( &pool->uninit, (void*)job );
    }
    for( int i = 0; i < pool->threads; i++ )
        if( fx_pthread_create( pool->thread_handle+i, NULL, (void*)fx_threadpool_thread, pool ) )
            goto fail;

    return 0;
fail:
    return -1;
}

void fx_threadpool_run( fx_threadpool_t *pool, void *(*func)(void *), void *arg )
{
    fx_threadpool_job_t *job = (void*)fx_sync_frame_list_pop( &pool->uninit );
    job->func = func;
    job->arg  = arg;
    fx_sync_frame_list_push( &pool->run, (void*)job );
}

void *fx_threadpool_wait( fx_threadpool_t *pool, void *arg )
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

void fx_threadpool_delete( fx_threadpool_t *pool )
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
