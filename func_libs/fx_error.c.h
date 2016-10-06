
#ifndef _FX_ERROR_C
#define _FX_ERROR_C
#include "fx_error.h"
#include "fx_string.h"

int fx_strerror(int errnum, char *errbuf, size_t errbuf_size)
{
    int ret = 0, i;
    const struct error_entry *entry = NULL;

    for (i = 0; i < ARRAY_ELEMS(error_entries); i++) {
        if (errnum == error_entries[i].num) {
            entry = &error_entries[i];
            break;
        }
    }
    if (entry) {
        fx_strlcpy(errbuf, entry->str, errbuf_size);
    } else {
#if HAVE_STRERROR_R
        ret = AVERROR(strerror_r(AVUNERROR(errnum), errbuf, errbuf_size));
#else
        ret = -1;
#endif
        if (ret < 0)
            snprintf(errbuf, errbuf_size, "Error number %d occurred", errnum);
    }

    return ret;
}
/**
 * Fill the provided buffer with a string containing an error string
 * corresponding to the AVERROR code errnum.
 *
 * @param errbuf         a buffer
 * @param errbuf_size    size in bytes of errbuf
 * @param errnum         error code to describe
 * @return the buffer in input, filled with the error description
 * @see av_strerror()
 */
static inline char *fx_make_error_string(char *errbuf, size_t errbuf_size, int errnum)
{
    fx_strerror(errnum, errbuf, errbuf_size);
    return errbuf;
}

/**
 * Convenience macro, the return value should be used only directly in
 * function arguments but never stand-alone.
 */
#define fx_err2str(errnum) \
    fx_make_error_string((char[AV_ERROR_MAX_STRING_SIZE]){0}, AV_ERROR_MAX_STRING_SIZE, errnum)

#endif