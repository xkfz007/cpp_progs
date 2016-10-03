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

//#define _ATOMICS
#ifdef _ATOMICS
#include "fx_atomic.h"
#include "fx_macros.h"

int main(void)
{
    volatile int val = 1;
    int res;

    res = avpriv_atomic_int_add_and_fetch(&val, 1);
    ASSERT(res == 2);
    avpriv_atomic_int_set(&val, 3);
    res = avpriv_atomic_int_get(&val);
    ASSERT(res == 3);

    return 0;
}



#endif /* !HAVE_ATOMICS_NATIVE */
