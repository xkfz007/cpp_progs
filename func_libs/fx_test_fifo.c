/*
 * a very simple circular buffer FIFO implementation
 * Copyright (c) 2000, 2001, 2002 Fabrice Bellard
 * Copyright (c) 2006 Roman Shaposhnik
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
//#define _FIFO
#ifdef _FIFO
#include "fx_fifo.h"
int main(void)
{
    /* create a FIFO buffer */
    AVFifoBuffer *fifo = fx_fifo_alloc(13 * sizeof(int));
    int i, j, n;

    /* fill data */
    for (i = 0; fx_fifo_space(fifo) >= sizeof(int); i++)
        fx_fifo_generic_write(fifo, &i, sizeof(int), NULL);

    /* peek at FIFO */
    n = fx_fifo_size(fifo) / sizeof(int);
    for (i = -n + 1; i < n; i++) {
        int *v = (int *)fx_fifo_peek2(fifo, i * sizeof(int));
        printf("%d: %d\n", i, *v);
    }
    printf("\n");

    /* peek_at at FIFO */
    n = fx_fifo_size(fifo) / sizeof(int);
    for (i = 0; i < n; i++) {
        fx_fifo_generic_peek_at(fifo, &j, i * sizeof(int), sizeof(j), NULL);
        printf("%d: %d\n", i, j);
    }
    printf("\n");

    /* read data */
    for (i = 0; fx_fifo_size(fifo) >= sizeof(int); i++) {
        fx_fifo_generic_read(fifo, &j, sizeof(int), NULL);
        printf("%d ", j);
    }
    printf("\n");

    /* test *ndx overflow */
    fx_fifo_reset(fifo);
    fifo->rndx = fifo->wndx = ~(uint32_t)0 - 5;

    /* fill data */
    for (i = 0; fx_fifo_space(fifo) >= sizeof(int); i++)
        fx_fifo_generic_write(fifo, &i, sizeof(int), NULL);

    /* peek_at at FIFO */
    n = fx_fifo_size(fifo) / sizeof(int);
    for (i = 0; i < n; i++) {
        fx_fifo_generic_peek_at(fifo, &j, i * sizeof(int), sizeof(j), NULL);
        printf("%d: %d\n", i, j);
    }

    fx_fifo_free(fifo);

    return 0;
}
#endif




















