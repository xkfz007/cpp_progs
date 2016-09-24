/*****************************************************************************
 * threadpool.h: thread pooling
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

#ifndef FX_THREADPOOL_H
#define FX_THREADPOOL_H

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

#endif
