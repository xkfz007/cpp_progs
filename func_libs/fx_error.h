#ifndef _FX_ERROR_H
#define _FX_ERROR_H

#include <errno.h>
#include "fx_macros.h"
#include "fx_string.h"

/* error handling */
#if EDOM > 0
#define AVERROR(e) (-(e))   ///< Returns a negative error code from a POSIX error code, to return from library functions.
#define AVUNERROR(e) (-(e)) ///< Returns a POSIX error code from a library function error return value.
#else
/* Some platforms have E* and errno already negated. */
#define AVERROR(e) (e)
#define AVUNERROR(e) (e)
#endif



#define AVERROR_BSF_NOT_FOUND      FFERRTAG(0xF8,'B','S','F') ///< Bitstream filter not found
#define AVERROR_BUG                FFERRTAG( 'B','U','G','!') ///< Internal bug, also see AVERROR_BUG2
#define AVERROR_BUFFER_TOO_SMALL   FFERRTAG( 'B','U','F','S') ///< Buffer too small
#define AVERROR_DECODER_NOT_FOUND  FFERRTAG(0xF8,'D','E','C') ///< Decoder not found
#define AVERROR_DEMUXER_NOT_FOUND  FFERRTAG(0xF8,'D','E','M') ///< Demuxer not found
#define AVERROR_ENCODER_NOT_FOUND  FFERRTAG(0xF8,'E','N','C') ///< Encoder not found
#define AVERROR_EOF                FFERRTAG( 'E','O','F',' ') ///< End of file
#define AVERROR_EXIT               FFERRTAG( 'E','X','I','T') ///< Immediate exit was requested; the called function should not be restarted
#define AVERROR_EXTERNAL           FFERRTAG( 'E','X','T',' ') ///< Generic error in an external library
#define AVERROR_FILTER_NOT_FOUND   FFERRTAG(0xF8,'F','I','L') ///< Filter not found
#define AVERROR_INVALIDDATA        FFERRTAG( 'I','N','D','A') ///< Invalid data found when processing input
#define AVERROR_MUXER_NOT_FOUND    FFERRTAG(0xF8,'M','U','X') ///< Muxer not found
#define AVERROR_OPTION_NOT_FOUND   FFERRTAG(0xF8,'O','P','T') ///< Option not found
#define AVERROR_PATCHWELCOME       FFERRTAG( 'P','A','W','E') ///< Not yet implemented in FFmpeg, patches welcome
#define AVERROR_PROTOCOL_NOT_FOUND FFERRTAG(0xF8,'P','R','O') ///< Protocol not found

#define AVERROR_STREAM_NOT_FOUND   FFERRTAG(0xF8,'S','T','R') ///< Stream not found
/**
 * This is semantically identical to AVERROR_BUG
 * it has been introduced in Libav after our AVERROR_BUG and with a modified value.
 */
#define AVERROR_BUG2               FFERRTAG( 'B','U','G',' ')
#define AVERROR_UNKNOWN            FFERRTAG( 'U','N','K','N') ///< Unknown error, typically from an external library
#define AVERROR_EXPERIMENTAL       (-0x2bb2afa8) ///< Requested feature is flagged experimental. Set strict_std_compliance if you really want to use it.
#define AVERROR_INPUT_CHANGED      (-0x636e6701) ///< Input changed between calls. Reconfiguration is required. (can be OR-ed with AVERROR_OUTPUT_CHANGED)
#define AVERROR_OUTPUT_CHANGED     (-0x636e6702) ///< Output changed between calls. Reconfiguration is required. (can be OR-ed with AVERROR_INPUT_CHANGED)
/* HTTP & RTSP errors */
#define AVERROR_HTTP_BAD_REQUEST   FFERRTAG(0xF8,'4','0','0')
#define AVERROR_HTTP_UNAUTHORIZED  FFERRTAG(0xF8,'4','0','1')
#define AVERROR_HTTP_FORBIDDEN     FFERRTAG(0xF8,'4','0','3')
#define AVERROR_HTTP_NOT_FOUND     FFERRTAG(0xF8,'4','0','4')
#define AVERROR_HTTP_OTHER_4XX     FFERRTAG(0xF8,'4','X','X')
#define AVERROR_HTTP_SERVER_ERROR  FFERRTAG(0xF8,'5','X','X')

#define AV_ERROR_MAX_STRING_SIZE 64

struct error_entry {
    int num;
    const char *tag;
    const char *str;
};

#define ERROR_TAG(tag) AVERROR_##tag, #tag
#define EERROR_TAG(tag) AVERROR(tag), #tag
#define AVERROR_INPUT_AND_OUTPUT_CHANGED (AVERROR_INPUT_CHANGED | AVERROR_OUTPUT_CHANGED)
static const struct error_entry error_entries[] = {
    { ERROR_TAG(BSF_NOT_FOUND),      "Bitstream filter not found"                     },
    { ERROR_TAG(BUG),                "Internal bug, should not have happened"         },
    { ERROR_TAG(BUG2),               "Internal bug, should not have happened"         },
    { ERROR_TAG(BUFFER_TOO_SMALL),   "Buffer too small"                               },
    { ERROR_TAG(DECODER_NOT_FOUND),  "Decoder not found"                              },
    { ERROR_TAG(DEMUXER_NOT_FOUND),  "Demuxer not found"                              },
    { ERROR_TAG(ENCODER_NOT_FOUND),  "Encoder not found"                              },
    { ERROR_TAG(EOF),                "End of file"                                    },
    { ERROR_TAG(EXIT),               "Immediate exit requested"                       },
    { ERROR_TAG(EXTERNAL),           "Generic error in an external library"           },
    { ERROR_TAG(FILTER_NOT_FOUND),   "Filter not found"                               },
    { ERROR_TAG(INPUT_CHANGED),      "Input changed"                                  },
    { ERROR_TAG(INVALIDDATA),        "Invalid data found when processing input"       },
    { ERROR_TAG(MUXER_NOT_FOUND),    "Muxer not found"                                },
    { ERROR_TAG(OPTION_NOT_FOUND),   "Option not found"                               },
    { ERROR_TAG(OUTPUT_CHANGED),     "Output changed"                                 },
    { ERROR_TAG(PATCHWELCOME),       "Not yet implemented in FFmpeg, patches welcome" },
    { ERROR_TAG(PROTOCOL_NOT_FOUND), "Protocol not found"                             },
    { ERROR_TAG(STREAM_NOT_FOUND),   "Stream not found"                               },
    { ERROR_TAG(UNKNOWN),            "Unknown error occurred"                         },
    { ERROR_TAG(EXPERIMENTAL),       "Experimental feature"                           },
    { ERROR_TAG(INPUT_AND_OUTPUT_CHANGED), "Input and output changed"                 },
    { ERROR_TAG(HTTP_BAD_REQUEST),   "Server returned 400 Bad Request"         },
    { ERROR_TAG(HTTP_UNAUTHORIZED),  "Server returned 401 Unauthorized (authorization failed)" },
    { ERROR_TAG(HTTP_FORBIDDEN),     "Server returned 403 Forbidden (access denied)" },
    { ERROR_TAG(HTTP_NOT_FOUND),     "Server returned 404 Not Found"           },
    { ERROR_TAG(HTTP_OTHER_4XX),     "Server returned 4XX Client Error, but not one of 40{0,1,3,4}" },
    { ERROR_TAG(HTTP_SERVER_ERROR),  "Server returned 5XX Server Error reply" },
#if !HAVE_STRERROR_R
    { EERROR_TAG(E2BIG),             "Argument list too long" },
    { EERROR_TAG(EACCES),            "Permission denied" },
    { EERROR_TAG(EAGAIN),            "Resource temporarily unavailable" },
    { EERROR_TAG(EBADF),             "Bad file descriptor" },
    { EERROR_TAG(EBUSY),             "Device or resource busy" },
    { EERROR_TAG(ECHILD),            "No child processes" },
    { EERROR_TAG(EDEADLK),           "Resource deadlock avoided" },
    { EERROR_TAG(EDOM),              "Numerical argument out of domain" },
    { EERROR_TAG(EEXIST),            "File exists" },
    { EERROR_TAG(EFAULT),            "Bad address" },
    { EERROR_TAG(EFBIG),             "File too large" },
    { EERROR_TAG(EILSEQ),            "Illegal byte sequence" },
    { EERROR_TAG(EINTR),             "Interrupted system call" },
    { EERROR_TAG(EINVAL),            "Invalid argument" },
    { EERROR_TAG(EIO),               "I/O error" },
    { EERROR_TAG(EISDIR),            "Is a directory" },
    { EERROR_TAG(EMFILE),            "Too many open files" },
    { EERROR_TAG(EMLINK),            "Too many links" },
    { EERROR_TAG(ENAMETOOLONG),      "File name too long" },
    { EERROR_TAG(ENFILE),            "Too many open files in system" },
    { EERROR_TAG(ENODEV),            "No such device" },
    { EERROR_TAG(ENOENT),            "No such file or directory" },
    { EERROR_TAG(ENOEXEC),           "Exec format error" },
    { EERROR_TAG(ENOLCK),            "No locks available" },
    { EERROR_TAG(ENOMEM),            "Cannot allocate memory" },
    { EERROR_TAG(ENOSPC),            "No space left on device" },
    { EERROR_TAG(ENOSYS),            "Function not implemented" },
    { EERROR_TAG(ENOTDIR),           "Not a directory" },
    { EERROR_TAG(ENOTEMPTY),         "Directory not empty" },
    { EERROR_TAG(ENOTTY),            "Inappropriate I/O control operation" },
    { EERROR_TAG(ENXIO),             "No such device or address" },
    { EERROR_TAG(EPERM),             "Operation not permitted" },
    { EERROR_TAG(EPIPE),             "Broken pipe" },
    { EERROR_TAG(ERANGE),            "Result too large" },
    { EERROR_TAG(EROFS),             "Read-only file system" },
    { EERROR_TAG(ESPIPE),            "Illegal seek" },
    { EERROR_TAG(ESRCH),             "No such process" },
    { EERROR_TAG(EXDEV),             "Cross-device link" },
#endif
};

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
