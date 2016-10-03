/*
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

/**
 * @file
 * @ingroup lavu_buffer
 * refcounted data buffer API
 */

#ifndef AVUTIL_BUFFER_H
#define AVUTIL_BUFFER_H

//#include <stdint.h>
#include "fx_thread.h"
#include "fx_memops.h"

/**
 * The buffer is always treated as read-only.
 */
#define BUFFER_FLAG_READONLY      (1 << 0)
/**
 * The buffer was fx_realloc()ed, so it is reallocatable.
 */
#define BUFFER_FLAG_REALLOCATABLE (1 << 1)

struct AVBuffer {
    uint8_t *data; /**< data described by this buffer */
    int      size; /**< size of data in bytes */

    /**
     *  number of existing AVBufferRef instances referring to this buffer
     */
    volatile int refcount;

    /**
     * a callback for freeing the data
     */
    void (*free)(void *opaque, uint8_t *data);

    /**
     * an opaque pointer, to be used by the freeing callback
     */
    void *opaque;

    /**
     * A combination of BUFFER_FLAG_*
     */
    int flags;
};

typedef struct BufferPoolEntry {
    uint8_t *data;

    /*
     * Backups of the original opaque/free of the AVBuffer corresponding to
     * data. They will be used to free the buffer when the pool is freed.
     */
    void *opaque;
    void (*free)(void *opaque, uint8_t *data);

    AVBufferPool *pool;
    struct BufferPoolEntry *next;
} BufferPoolEntry;

struct AVBufferPool {
    fx_pthread_mutex_t mutex;
    BufferPoolEntry *pool;

    /*
     * This is used to track when the pool is to be freed.
     * The pointer to the pool itself held by the caller is considered to
     * be one reference. Each buffer requested by the caller increases refcount
     * by one, returning the buffer to the pool decreases it by one.
     * refcount reaches zero when the buffer has been uninited AND all the
     * buffers have been released, then it's safe to free the pool and all
     * the buffers in it.
     */
    volatile int refcount;

    volatile int nb_allocated;

    int size;
    void *opaque;
    AVBufferRef* (*alloc)(int size);
    AVBufferRef* (*alloc2)(void *opaque, int size);
    void         (*pool_free)(void *opaque);
};
/**
 * @defgroup lavu_buffer AVBuffer
 * @ingroup lavu_data
 *
 * @{
 * AVBuffer is an API for reference-counted data buffers.
 *
 * There are two core objects in this API -- AVBuffer and AVBufferRef. AVBuffer
 * represents the data buffer itself; it is opaque and not meant to be accessed
 * by the caller directly, but only through AVBufferRef. However, the caller may
 * e.g. compare two AVBuffer pointers to check whether two different references
 * are describing the same data buffer. AVBufferRef represents a single
 * reference to an AVBuffer and it is the object that may be manipulated by the
 * caller directly.
 *
 * There are two functions provided for creating a new AVBuffer with a single
 * reference -- fx_buffer_alloc() to just allocate a new buffer, and
 * fx_buffer_create() to wrap an existing array in an AVBuffer. From an existing
 * reference, additional references may be created with fx_buffer_ref().
 * Use fx_buffer_unref() to free a reference (this will automatically free the
 * data once all the references are freed).
 *
 * The convention throughout this API and the rest of FFmpeg is such that the
 * buffer is considered writable if there exists only one reference to it (and
 * it has not been marked as read-only). The fx_buffer_is_writable() function is
 * provided to check whether this is true and fx_buffer_make_writable() will
 * automatically create a new writable buffer when necessary.
 * Of course nothing prevents the calling code from violating this convention,
 * however that is safe only when all the existing references are under its
 * control.
 *
 * @note Referencing and unreferencing the buffers is thread-safe and thus
 * may be done from multiple threads simultaneously without any need for
 * additional locking.
 *
 * @note Two different references to the same buffer can point to different
 * parts of the buffer (i.e. their AVBufferRef.data will not be equal).
 */

/**
 * A reference counted buffer type. It is opaque and is meant to be used through
 * references (AVBufferRef).
 */
typedef struct AVBuffer AVBuffer;

/**
 * A reference to a data buffer.
 *
 * The size of this struct is not a part of the public ABI and it is not meant
 * to be allocated directly.
 */
typedef struct AVBufferRef {
    AVBuffer *buffer;

    /**
     * The data buffer. It is considered writable if and only if
     * this is the only reference to the buffer, in which case
     * fx_buffer_is_writable() returns 1.
     */
    uint8_t *data;
    /**
     * Size of data in bytes.
     */
    int      size;
} AVBufferRef;

/**
 * Allocate an AVBuffer of the given size using fx_malloc().
 *
 * @return an AVBufferRef of given size or NULL when out of memory
 */
AVBufferRef *fx_buffer_alloc(int size)
{
    AVBufferRef *ret = NULL;
    uint8_t    *data = NULL;

    data = fx_malloc(size);
    if (!data)
        return NULL;

    ret = fx_buffer_create(data, size, fx_buffer_default_free, NULL, 0);
    if (!ret)
        fx_freep(&data);

    return ret;
}

/**
 * Same as fx_buffer_alloc(), except the returned buffer will be initialized
 * to zero.
 */
AVBufferRef *av_buffer_allocz(int size)
{
    AVBufferRef *ret = av_buffer_alloc(size);
    if (!ret)
        return NULL;

    memset(ret->data, 0, size);
    return ret;
}

/**
 * Always treat the buffer as read-only, even when it has only one
 * reference.
 */
#define fx_BUFFER_FLAG_READONLY (1 << 0)

/**
 * Create an AVBuffer from an existing array.
 *
 * If this function is successful, data is owned by the AVBuffer. The caller may
 * only access data through the returned AVBufferRef and references derived from
 * it.
 * If this function fails, data is left untouched.
 * @param data   data array
 * @param size   size of data in bytes
 * @param free   a callback for freeing this buffer's data
 * @param opaque parameter to be got for processing or passed to free
 * @param flags  a combination of fx_BUFFER_FLAG_*
 *
 * @return an AVBufferRef referring to data on success, NULL on failure.
 */

AVBufferRef *fx_buffer_create(uint8_t *data, int size,
                              void (*free)(void *opaque, uint8_t *data),
                              void *opaque, int flags)
{
    AVBufferRef *ref = NULL;
    AVBuffer    *buf = NULL;

    buf = fx_mallocz(sizeof(*buf));
    if (!buf)
        return NULL;

    buf->data     = data;
    buf->size     = size;
    buf->free     = free ? free : fx_buffer_default_free;
    buf->opaque   = opaque;
    buf->refcount = 1;

    if (flags & fx_BUFFER_FLAG_READONLY)
        buf->flags |= BUFFER_FLAG_READONLY;

    ref = fx_mallocz(sizeof(*ref));
    if (!ref) {
        fx_freep(&buf);
        return NULL;
    }

    ref->buffer = buf;
    ref->data   = data;
    ref->size   = size;

    return ref;
}
/**
 * Default free callback, which calls fx_free() on the buffer data.
 * This function is meant to be passed to fx_buffer_create(), not called
 * directly.
 */
void fx_buffer_default_free(void *opaque, uint8_t *data)
{
    fx_free(data);
}

/**
 * Create a new reference to an AVBuffer.
 *
 * @return a new AVBufferRef referring to the same AVBuffer as buf or NULL on
 * failure.
 */
AVBufferRef *av_buffer_ref(AVBufferRef *buf)
{
    AVBufferRef *ret = av_mallocz(sizeof(*ret));

    if (!ret)
        return NULL;

    *ret = *buf;

    avpriv_atomic_int_add_and_fetch(&buf->buffer->refcount, 1);

    return ret;
}

/**
 * Free a given reference and automatically free the buffer if there are no more
 * references to it.
 *
 * @param buf the reference to be freed. The pointer is set to NULL on return.
 */
static void buffer_replace(AVBufferRef **dst, AVBufferRef **src)
{
    AVBuffer *b;

    b = (*dst)->buffer;

    if (src) {
        **dst = **src;
        av_freep(src);
    } else
        av_freep(dst);

    if (!avpriv_atomic_int_add_and_fetch(&b->refcount, -1)) {
        b->free(b->opaque, b->data);
        av_freep(&b);
    }
}

void av_buffer_unref(AVBufferRef **buf)
{
    if (!buf || !*buf)
        return;

    buffer_replace(buf, NULL);
}

/**
 * @return 1 if the caller may write to the data referred to by buf (which is
 * true if and only if buf is the only reference to the underlying AVBuffer).
 * Return 0 otherwise.
 * A positive answer is valid until fx_buffer_ref() is called on buf.
 */
int av_buffer_is_writable(const AVBufferRef *buf)
{
    if (buf->buffer->flags & AV_BUFFER_FLAG_READONLY)
        return 0;

    return avpriv_atomic_int_get(&buf->buffer->refcount) == 1;
}

/**
 * @return the opaque parameter set by fx_buffer_create.
 */
void *av_buffer_get_opaque(const AVBufferRef *buf)
{
    return buf->buffer->opaque;
}

int av_buffer_get_ref_count(const AVBufferRef *buf)
{
    return buf->buffer->refcount;
}

/**
 * Create a writable reference from a given buffer reference, avoiding data copy
 * if possible.
 *
 * @param buf buffer reference to make writable. On success, buf is either left
 *            untouched, or it is unreferenced and a new writable AVBufferRef is
 *            written in its place. On failure, buf is left untouched.
 * @return 0 on success, a negative AVERROR on failure.
 */
int av_buffer_make_writable(AVBufferRef **pbuf)
{
    AVBufferRef *newbuf, *buf = *pbuf;

    if (av_buffer_is_writable(buf))
        return 0;

    newbuf = av_buffer_alloc(buf->size);
    if (!newbuf)
        return AVERROR(ENOMEM);

    memcpy(newbuf->data, buf->data, buf->size);

    buffer_replace(pbuf, &newbuf);

    return 0;
}

/**
 * Reallocate a given buffer.
 *
 * @param buf  a buffer reference to reallocate. On success, buf will be
 *             unreferenced and a new reference with the required size will be
 *             written in its place. On failure buf will be left untouched. *buf
 *             may be NULL, then a new buffer is allocated.
 * @param size required new buffer size.
 * @return 0 on success, a negative AVERROR on failure.
 *
 * @note the buffer is actually reallocated with fx_realloc() only if it was
 * initially allocated through fx_buffer_realloc(NULL) and there is only one
 * reference to it (i.e. the one passed to this function). In all other cases
 * a new buffer is allocated and the data is copied.
 */
int av_buffer_realloc(AVBufferRef **pbuf, int size)
{
    AVBufferRef *buf = *pbuf;
    uint8_t *tmp;

    if (!buf) {
        /* allocate a new buffer with av_realloc(), so it will be reallocatable
         * later */
        uint8_t *data = av_realloc(NULL, size);
        if (!data)
            return AVERROR(ENOMEM);

        buf = av_buffer_create(data, size, av_buffer_default_free, NULL, 0);
        if (!buf) {
            av_freep(&data);
            return AVERROR(ENOMEM);
        }

        buf->buffer->flags |= BUFFER_FLAG_REALLOCATABLE;
        *pbuf = buf;

        return 0;
    } else if (buf->size == size)
        return 0;

    if (!(buf->buffer->flags & BUFFER_FLAG_REALLOCATABLE) ||
        !av_buffer_is_writable(buf)) {
        /* cannot realloc, allocate a new reallocable buffer and copy data */
        AVBufferRef *new = NULL;

        av_buffer_realloc(&new, size);
        if (!new)
            return AVERROR(ENOMEM);

        memcpy(new->data, buf->data, FFMIN(size, buf->size));

        buffer_replace(pbuf, &new);
        return 0;
    }

    tmp = av_realloc(buf->buffer->data, size);
    if (!tmp)
        return AVERROR(ENOMEM);

    buf->buffer->data = buf->data = tmp;
    buf->buffer->size = buf->size = size;
    return 0;
}

/**
 * @}
 */

/**
 * @defgroup lavu_bufferpool AVBufferPool
 * @ingroup lavu_data
 *
 * @{
 * AVBufferPool is an API for a lock-free thread-safe pool of AVBuffers.
 *
 * Frequently allocating and freeing large buffers may be slow. AVBufferPool is
 * meant to solve this in cases when the caller needs a set of buffers of the
 * same size (the most obvious use case being buffers for raw video or audio
 * frames).
 *
 * At the beginning, the user must call fx_buffer_pool_init() to create the
 * buffer pool. Then whenever a buffer is needed, call fx_buffer_pool_get() to
 * get a reference to a new buffer, similar to fx_buffer_alloc(). This new
 * reference works in all aspects the same way as the one created by
 * fx_buffer_alloc(). However, when the last reference to this buffer is
 * unreferenced, it is returned to the pool instead of being freed and will be
 * reused for subsequent fx_buffer_pool_get() calls.
 *
 * When the caller is done with the pool and no longer needs to allocate any new
 * buffers, fx_buffer_pool_uninit() must be called to mark the pool as freeable.
 * Once all the buffers are released, it will automatically be freed.
 *
 * Allocating and releasing buffers with this API is thread-safe as long as
 * either the default alloc callback is used, or the user-supplied one is
 * thread-safe.
 */

/**
 * The buffer pool. This structure is opaque and not meant to be accessed
 * directly. It is allocated with fx_buffer_pool_init() and freed with
 * fx_buffer_pool_uninit().
 */
typedef struct AVBufferPool AVBufferPool;

/**
 * Allocate and initialize a buffer pool.
 *
 * @param size size of each buffer in this pool
 * @param alloc a function that will be used to allocate new buffers when the
 * pool is empty. May be NULL, then the default allocator will be used
 * (fx_buffer_alloc()).
 * @return newly created buffer pool on success, NULL on error.
 */
AVBufferPool *av_buffer_pool_init(int size, AVBufferRef* (*alloc)(int size))
{
    AVBufferPool *pool = av_mallocz(sizeof(*pool));
    if (!pool)
        return NULL;

    ff_mutex_init(&pool->mutex, NULL);

    pool->size     = size;
    pool->alloc    = alloc ? alloc : av_buffer_alloc;

    avpriv_atomic_int_set(&pool->refcount, 1);

    return pool;
}

/**
 * Allocate and initialize a buffer pool with a more complex allocator.
 *
 * @param size size of each buffer in this pool
 * @param opaque arbitrary user data used by the allocator
 * @param alloc a function that will be used to allocate new buffers when the
 *              pool is empty.
 * @param pool_free a function that will be called immediately before the pool
 *                  is freed. I.e. after fx_buffer_pool_can_uninit() is called
 *                  by the pool and all the frames are returned to the pool and
 *                  freed. It is intended to uninitialize the user opaque data.
 * @return newly created buffer pool on success, NULL on error.
 */
AVBufferPool *av_buffer_pool_init2(int size, void *opaque,
                                   AVBufferRef* (*alloc)(void *opaque, int size),
                                   void (*pool_free)(void *opaque))
{
    AVBufferPool *pool = av_mallocz(sizeof(*pool));
    if (!pool)
        return NULL;

    ff_mutex_init(&pool->mutex, NULL);

    pool->size      = size;
    pool->opaque    = opaque;
    pool->alloc2    = alloc;
    pool->pool_free = pool_free;

    avpriv_atomic_int_set(&pool->refcount, 1);

    return pool;
}
/*
 * This function gets called when the pool has been uninited and
 * all the buffers returned to it.
 */
static void buffer_pool_free(AVBufferPool *pool)
{
    while (pool->pool) {
        BufferPoolEntry *buf = pool->pool;
        pool->pool = buf->next;

        buf->free(buf->opaque, buf->data);
        av_freep(&buf);
    }
    ff_mutex_destroy(&pool->mutex);

    if (pool->pool_free)
        pool->pool_free(pool->opaque);

    av_freep(&pool);
}
/**
 * Mark the pool as being available for freeing. It will actually be freed only
 * once all the allocated buffers associated with the pool are released. Thus it
 * is safe to call this function while some of the allocated buffers are still
 * in use.
 *
 * @param pool pointer to the pool to be freed. It will be set to NULL.
 */
void av_buffer_pool_uninit(AVBufferPool **ppool)
{
    AVBufferPool *pool;

    if (!ppool || !*ppool)
        return;
    pool   = *ppool;
    *ppool = NULL;

    if (!avpriv_atomic_int_add_and_fetch(&pool->refcount, -1))
        buffer_pool_free(pool);
}

/**
 * Allocate a new AVBuffer, reusing an old buffer from the pool when available.
 * This function may be called simultaneously from multiple threads.
 *
 * @return a reference to the new buffer on success, NULL on error.
 */
AVBufferRef *fx_buffer_pool_get(AVBufferPool *pool);

#if USE_ATOMICS
/* remove the whole buffer list from the pool and return it */
static BufferPoolEntry *get_pool(AVBufferPool *pool)
{
    BufferPoolEntry *cur = *(void * volatile *)&pool->pool, *last = NULL;

    while (cur != last) {
        last = cur;
        cur = avpriv_atomic_ptr_cas((void * volatile *)&pool->pool, last, NULL);
        if (!cur)
            return NULL;
    }

    return cur;
}

static void add_to_pool(BufferPoolEntry *buf)
{
    AVBufferPool *pool;
    BufferPoolEntry *cur, *end = buf;

    if (!buf)
        return;
    pool = buf->pool;

    while (end->next)
        end = end->next;

    while (avpriv_atomic_ptr_cas((void * volatile *)&pool->pool, NULL, buf)) {
        /* pool is not empty, retrieve it and append it to our list */
        cur = get_pool(pool);
        end->next = cur;
        while (end->next)
            end = end->next;
    }
}
#endif

static void pool_release_buffer(void *opaque, uint8_t *data)
{
    BufferPoolEntry *buf = opaque;
    AVBufferPool *pool = buf->pool;

    if(CONFIG_MEMORY_POISONING)
        memset(buf->data, FF_MEMORY_POISON, pool->size);

#if USE_ATOMICS
    add_to_pool(buf);
#else
    ff_mutex_lock(&pool->mutex);
    buf->next = pool->pool;
    pool->pool = buf;
    ff_mutex_unlock(&pool->mutex);
#endif

    if (!avpriv_atomic_int_add_and_fetch(&pool->refcount, -1))
        buffer_pool_free(pool);
}

/* allocate a new buffer and override its free() callback so that
 * it is returned to the pool on free */
static AVBufferRef *pool_alloc_buffer(AVBufferPool *pool)
{
    BufferPoolEntry *buf;
    AVBufferRef     *ret;

    ret = pool->alloc2 ? pool->alloc2(pool->opaque, pool->size) :
                         pool->alloc(pool->size);
    if (!ret)
        return NULL;

    buf = av_mallocz(sizeof(*buf));
    if (!buf) {
        av_buffer_unref(&ret);
        return NULL;
    }

    buf->data   = ret->buffer->data;
    buf->opaque = ret->buffer->opaque;
    buf->free   = ret->buffer->free;
    buf->pool   = pool;

    ret->buffer->opaque = buf;
    ret->buffer->free   = pool_release_buffer;

#if USE_ATOMICS
    avpriv_atomic_int_add_and_fetch(&pool->refcount, 1);
    avpriv_atomic_int_add_and_fetch(&pool->nb_allocated, 1);
#endif

    return ret;
}

AVBufferRef *av_buffer_pool_get(AVBufferPool *pool)
{
    AVBufferRef *ret;
    BufferPoolEntry *buf;

#if USE_ATOMICS
    /* check whether the pool is empty */
    buf = get_pool(pool);
    if (!buf && pool->refcount <= pool->nb_allocated) {
        av_log(NULL, AV_LOG_DEBUG, "Pool race dectected, spining to avoid overallocation and eventual OOM\n");
        while (!buf && avpriv_atomic_int_get(&pool->refcount) <= avpriv_atomic_int_get(&pool->nb_allocated))
            buf = get_pool(pool);
    }

    if (!buf)
        return pool_alloc_buffer(pool);

    /* keep the first entry, return the rest of the list to the pool */
    add_to_pool(buf->next);
    buf->next = NULL;

    ret = av_buffer_create(buf->data, pool->size, pool_release_buffer,
                           buf, 0);
    if (!ret) {
        add_to_pool(buf);
        return NULL;
    }
#else
    ff_mutex_lock(&pool->mutex);
    buf = pool->pool;
    if (buf) {
        ret = av_buffer_create(buf->data, pool->size, pool_release_buffer,
                               buf, 0);
        if (ret) {
            pool->pool = buf->next;
            buf->next = NULL;
        }
    } else {
        ret = pool_alloc_buffer(pool);
    }
    ff_mutex_unlock(&pool->mutex);
#endif

    if (ret)
        avpriv_atomic_int_add_and_fetch(&pool->refcount, 1);

    return ret;
}
/**
 * @}
 */

#endif /* AVUTIL_BUFFER_H */
