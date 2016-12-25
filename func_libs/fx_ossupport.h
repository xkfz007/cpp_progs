#ifndef _FX_OSSUPPORT_H
#define _FX_OSSUPPORT_H

#ifdef _WIN32
#include <direct.h>
#endif

#if defined(_WIN32) && !defined(__MINGW32CE__)
#  include <fcntl.h>
#  ifdef lseek
#   undef lseek
#  endif
#  define lseek(f,p,w) _lseeki64((f), (p), (w))
#  ifdef stat
#   undef stat
#  endif
#  define stat _stati64
#  ifdef fstat
#   undef fstat
#  endif
#  define fstat(f,s) _fstati64((f), (s))
#endif /* defined(_WIN32) && !defined(__MINGW32CE__) */


if defined(_WIN32)
#include <stdio.h>
#include <windows.h>
#include "libavutil/wchar_filename.h"

#ifdef WINAPI_FAMILY
#include <winapifamily.h>
// If a WINAPI_FAMILY is defined, check that the desktop API subset
// is enabled
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#define USE_MOVEFILEEXA
#endif
#else
// If no WINAPI_FAMILY is defined, assume the full API subset
#define USE_MOVEFILEEXA
#endif
#define DEF_FS_FUNCTION(name, wfunc, afunc)               \
static inline int win32_##name(const char *filename_utf8) \
{                                                         \
    wchar_t *filename_w;                                  \
    int ret;                                              \
                                                          \
    if (utf8towchar(filename_utf8, &filename_w))          \
        return -1;                                        \
    if (!filename_w)                                      \
        goto fallback;                                    \
                                                          \
    ret = wfunc(filename_w);                              \
    av_free(filename_w);                                  \
    return ret;                                           \
                                                          \
fallback:                                                 \
    /* filename may be be in CP_ACP */                    \
    return afunc(filename_utf8);                          \
}

DEF_FS_FUNCTION(unlink, _wunlink, _unlink)
DEF_FS_FUNCTION(mkdir,  _wmkdir,  _mkdir)
DEF_FS_FUNCTION(rmdir,  _wrmdir , _rmdir)

#define DEF_FS_FUNCTION2(name, wfunc, afunc, partype)     \
static inline int win32_##name(const char *filename_utf8, partype par) \
{                                                         \
    wchar_t *filename_w;                                  \
    int ret;                                              \
                                                          \
    if (utf8towchar(filename_utf8, &filename_w))          \
        return -1;                                        \
    if (!filename_w)                                      \
        goto fallback;                                    \
                                                          \
    ret = wfunc(filename_w, par);                         \
    av_free(filename_w);                                  \
    return ret;                                           \
                                                          \
fallback:                                                 \
    /* filename may be be in CP_ACP */                    \
    return afunc(filename_utf8, par);                     \
}

DEF_FS_FUNCTION2(access, _waccess, _access, int)
DEF_FS_FUNCTION2(stat, _wstati64, _stati64, struct stat*)

static inline int win32_rename(const char *src_utf8, const char *dest_utf8)
{
    wchar_t *src_w, *dest_w;
    int ret;

    if (utf8towchar(src_utf8, &src_w))
        return -1;
    if (utf8towchar(dest_utf8, &dest_w)) {
        av_free(src_w);
        return -1;
    }
    if (!src_w || !dest_w) {
        av_free(src_w);
        av_free(dest_w);
        goto fallback;
    }

    ret = MoveFileExW(src_w, dest_w, MOVEFILE_REPLACE_EXISTING);
    av_free(src_w);
    av_free(dest_w);
    // Lacking proper mapping from GetLastError() error codes to errno codes
    if (ret)
        errno = EPERM;
    return ret;

fallback:
    /* filename may be be in CP_ACP */
#ifdef USE_MOVEFILEEXA
    ret = MoveFileExA(src_utf8, dest_utf8, MOVEFILE_REPLACE_EXISTING);
    if (ret)
        errno = EPERM;
#else
    /* Windows Phone doesn't have MoveFileExA, and for Windows Store apps,
     * it is available but not allowed by the app certification kit. However,
     * it's unlikely that anybody would input filenames in CP_ACP there, so this
     * fallback is kept mostly for completeness. Alternatively we could
     * do MultiByteToWideChar(CP_ACP) and use MoveFileExW, but doing
     * explicit conversions with CP_ACP is allegedly forbidden in windows
     * store apps (or windows phone), and the notion of a native code page
     * doesn't make much sense there. */
    ret = rename(src_utf8, dest_utf8);
#endif
    return ret;
}

#define mkdir(a, b) win32_mkdir(a)
#define rename      win32_rename
#define rmdir       win32_rmdir
#define unlink      win32_unlink
#define access      win32_access
#endif

#endif
