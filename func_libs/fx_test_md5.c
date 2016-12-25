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

//#define _MD5
#ifdef _MD5 
#include "fx_md5.h"
#include "fx_types.h"

static void print_md5(uint8_t *md5)
{
    int i;
    for (i = 0; i < 16; i++)
        printf("%02x", md5[i]);
    printf("\n");
}

int main(void)
{
    uint8_t md5val[16];
    int i;
    volatile uint8_t in[1000]; // volatile to workaround http://llvm.org/bugs/show_bug.cgi?id=20849
    // FIXME remove volatile once it has been fixed and all fate clients are updated

    for (i = 0; i < 1000; i++)
        in[i] = i * i;
    fx_md5_sum(md5val, in, 1000);
    print_md5(md5val);
    fx_md5_sum(md5val, in, 63);
    print_md5(md5val);
    fx_md5_sum(md5val, in, 64);
    print_md5(md5val);
    fx_md5_sum(md5val, in, 65);
    print_md5(md5val);
    for (i = 0; i < 1000; i++)
        in[i] = i % 127;
    fx_md5_sum(md5val, in, 999);
    print_md5(md5val);

    return 0;
}
#endif /* AVUTIL_BUFFER_INTERNAL_H */
