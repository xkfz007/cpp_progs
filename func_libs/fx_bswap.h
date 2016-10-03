/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

/**
 * @file
 * byte swapping routines
 */

#ifndef AVUTIL_BSWAP_H
#define AVUTIL_BSWAP_H

#include "fx_types.h"
#define FX_HAVE_BIGENDIAN  1

#define FX_BSWAP16C(x) (((x) << 8 & 0xff00)  | ((x) >> 8 & 0x00ff))
#define FX_BSWAP32C(x) (FX_BSWAP16C(x) << 16 | FX_BSWAP16C((x) >> 16))
#define FX_BSWAP64C(x) (FX_BSWAP32C(x) << 32 | FX_BSWAP32C((x) >> 32))

#define FX_BSWAPC(s, x) FX_BSWAP##s##C(x)

#ifndef fx_bswap16
static uint16_t fx_bswap16(uint16_t x)
{
    x= (x>>8) | (x<<8);
    return x;
}
#endif

#ifndef fx_bswap32
static uint32_t fx_bswap32(uint32_t x)
{
    return FX_BSWAP32C(x);
}
#endif

#ifndef fx_bswap64
static inline uint64_t fx_bswap64(uint64_t x)
{
    return (uint64_t)fx_bswap32(x) << 32 | fx_bswap32(x >> 32);
}
#endif

// be2ne ... big-endian to native-endian
// le2ne ... little-endian to native-endian

#if FX_HAVE_BIGENDIAN
#define fx_be2ne16(x) (x)
#define fx_be2ne32(x) (x)
#define fx_be2ne64(x) (x)
#define fx_le2ne16(x) fx_bswap16(x)
#define fx_le2ne32(x) fx_bswap32(x)
#define fx_le2ne64(x) fx_bswap64(x)
#define FX_BE2NEC(s, x) (x)
#define FX_LE2NEC(s, x) FX_BSWAPC(s, x)
#else
#define fx_be2ne16(x) fx_bswap16(x)
#define fx_be2ne32(x) fx_bswap32(x)
#define fx_be2ne64(x) fx_bswap64(x)
#define fx_le2ne16(x) (x)
#define fx_le2ne32(x) (x)
#define fx_le2ne64(x) (x)
#define FX_BE2NEC(s, x) FX_BSWAPC(s, x)
#define FX_LE2NEC(s, x) (x)
#endif

#define FX_BE2NE16C(x) FX_BE2NEC(16, x)
#define FX_BE2NE32C(x) FX_BE2NEC(32, x)
#define FX_BE2NE64C(x) FX_BE2NEC(64, x)
#define FX_LE2NE16C(x) FX_LE2NEC(16, x)
#define FX_LE2NE32C(x) FX_LE2NEC(32, x)
#define FX_LE2NE64C(x) FX_LE2NEC(64, x)

#endif /* AVUTIL_BSWAP_H */
