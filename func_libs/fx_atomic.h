/*
 * Copyright (c) 2012 Ronald S. Bultje <rsbultje@gmail.com>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_ATOMIC_H
#define AVUTIL_ATOMIC_H

//#include "config.h"
#define HAVE_ATOMICS_NATIVE 1
#ifdef WIN32
#define HAVE_ATOMICS_WIN32  1
#endif


#if HAVE_ATOMICS_NATIVE

#if HAVE_ATOMICS_GCC
#define avpriv_atomic_int_get atomic_int_get_gcc
static inline int atomic_int_get_gcc(volatile int *ptr)
{
#if HAVE_ATOMIC_COMPARE_EXCHANGE
    return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
#else
    __sync_synchronize();
    return *ptr;
#endif
}

#define avpriv_atomic_int_set atomic_int_set_gcc
static inline void atomic_int_set_gcc(volatile int *ptr, int val)
{
#if HAVE_ATOMIC_COMPARE_EXCHANGE
    __atomic_store_n(ptr, val, __ATOMIC_SEQ_CST);
#else
    *ptr = val;
    __sync_synchronize();
#endif
}

#define avpriv_atomic_int_add_and_fetch atomic_int_add_and_fetch_gcc
static inline int atomic_int_add_and_fetch_gcc(volatile int *ptr, int inc)
{
#if HAVE_ATOMIC_COMPARE_EXCHANGE
    return __atomic_add_fetch(ptr, inc, __ATOMIC_SEQ_CST);
#else
    return __sync_add_and_fetch(ptr, inc);
#endif
}

#define avpriv_atomic_ptr_cas atomic_ptr_cas_gcc
static inline void *atomic_ptr_cas_gcc(void * volatile *ptr,
                                       void *oldval, void *newval)
{
#if HAVE_SYNC_VAL_COMPARE_AND_SWAP
#ifdef __ARMCC_VERSION
    // armcc will throw an error if ptr is not an integer type
    volatile uintptr_t *tmp = (volatile uintptr_t*)ptr;
    return (void*)__sync_val_compare_and_swap(tmp, oldval, newval);
#else
    return __sync_val_compare_and_swap(ptr, oldval, newval);
#endif
#else
    __atomic_compare_exchange_n(ptr, &oldval, newval, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return oldval;
#endif
}
#elif HAVE_ATOMICS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define avpriv_atomic_int_get atomic_int_get_win32
static inline int atomic_int_get_win32(volatile int *ptr)
{
    MemoryBarrier();
    return *ptr;
}

#define avpriv_atomic_int_set atomic_int_set_win32
static inline void atomic_int_set_win32(volatile int *ptr, int val)
{
    *ptr = val;
    MemoryBarrier();
}

#define avpriv_atomic_int_add_and_fetch atomic_int_add_and_fetch_win32
static inline int atomic_int_add_and_fetch_win32(volatile int *ptr, int inc)
{
    return inc + InterlockedExchangeAdd(ptr, inc);
}

#define avpriv_atomic_ptr_cas atomic_ptr_cas_win32
static inline void *atomic_ptr_cas_win32(void * volatile *ptr,
                                         void *oldval, void *newval)
{
    return InterlockedCompareExchangePointer(ptr, newval, oldval);
}
#endif

#else

/**
 * Load the current value stored in an atomic integer.
 *
 * @param ptr atomic integer
 * @return the current value of the atomic integer
 * @note This acts as a memory barrier.
 */
int avpriv_atomic_int_get(volatile int *ptr);

/**
 * Store a new value in an atomic integer.
 *
 * @param ptr atomic integer
 * @param val the value to store in the atomic integer
 * @note This acts as a memory barrier.
 */
void avpriv_atomic_int_set(volatile int *ptr, int val);

/**
 * Add a value to an atomic integer.
 *
 * @param ptr atomic integer
 * @param inc the value to add to the atomic integer (may be negative)
 * @return the new value of the atomic integer.
 * @note This does NOT act as a memory barrier. This is primarily
 *       intended for reference counting.
 */
int avpriv_atomic_int_add_and_fetch(volatile int *ptr, int inc);

/**
 * Atomic pointer compare and swap.
 *
 * @param ptr pointer to the pointer to operate on
 * @param oldval do the swap if the current value of *ptr equals to oldval
 * @param newval value to replace *ptr with
 * @return the value of *ptr before comparison
 */
void *avpriv_atomic_ptr_cas(void * volatile *ptr, void *oldval, void *newval);

#if HAVE_PTHREADS

#include <pthread.h>

static pthread_mutex_t atomic_lock = PTHREAD_MUTEX_INITIALIZER;

int avpriv_atomic_int_get(volatile int *ptr)
{
    int res;

    pthread_mutex_lock(&atomic_lock);
    res = *ptr;
    pthread_mutex_unlock(&atomic_lock);

    return res;
}

void avpriv_atomic_int_set(volatile int *ptr, int val)
{
    pthread_mutex_lock(&atomic_lock);
    *ptr = val;
    pthread_mutex_unlock(&atomic_lock);
}

int avpriv_atomic_int_add_and_fetch(volatile int *ptr, int inc)
{
    int res;

    pthread_mutex_lock(&atomic_lock);
    *ptr += inc;
    res = *ptr;
    pthread_mutex_unlock(&atomic_lock);

    return res;
}

void *avpriv_atomic_ptr_cas(void * volatile *ptr, void *oldval, void *newval)
{
    void *ret;
    pthread_mutex_lock(&atomic_lock);
    ret = *ptr;
    if (ret == oldval)
        *ptr = newval;
    pthread_mutex_unlock(&atomic_lock);
    return ret;
}

#elif !HAVE_THREADS

int avpriv_atomic_int_get(volatile int *ptr)
{
    return *ptr;
}

void avpriv_atomic_int_set(volatile int *ptr, int val)
{
    *ptr = val;
}

int avpriv_atomic_int_add_and_fetch(volatile int *ptr, int inc)
{
    *ptr += inc;
    return *ptr;
}

void *avpriv_atomic_ptr_cas(void * volatile *ptr, void *oldval, void *newval)
{
    if (*ptr == oldval) {
        *ptr = newval;
        return oldval;
    }
    return *ptr;
}

#else /* HAVE_THREADS */

/* This should never trigger, unless a new threading implementation
 * without correct atomics dependencies in configure or a corresponding
 * atomics implementation is added. */
#error "Threading is enabled, but there is no implementation of atomic operations available"

#endif /* HAVE_PTHREADS */
#endif /* HAVE_ATOMICS_NATIVE */

#endif /* AVUTIL_ATOMIC_H */
