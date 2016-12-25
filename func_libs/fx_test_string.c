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
//#define _STRING
#ifdef _STRING
#include <stdio.h>
#include "fx_string.h"
#include "fx_string2.h"
#include "fx_memops.h"
#include "fx_bprint.h"
#include "fx_log.h"


int main2(void)
{
    int i;
    char *fullpath;
    static const char * const strings[] = {
        "''",
        "",
        ":",
        "\\",
        "'",
        "    ''    :",
        "    ''  ''  :",
        "foo   '' :",
        "'foo'",
        "foo     ",
        "  '  foo  '  ",
        "foo\\",
        "foo':  blah:blah",
        "foo\\:  blah:blah",
        "foo\'",
        "'foo :  '  :blahblah",
        "\\ :blah",
        "     foo",
        "      foo       ",
        "      foo     \\ ",
        "foo ':blah",
        " foo   bar    :   blahblah",
        "\\f\\o\\o",
        "'foo : \\ \\  '   : blahblah",
        "'\\fo\\o:': blahblah",
        "\\'fo\\o\\:':  foo  '  :blahblah"
    };

    printf("Testing fx_get_token()\n");
    for (i = 0; i < ARRAY_ELEMS(strings); i++) {
        const char *p = strings[i];
        char *q;
        printf("|%s|", p);
        q = fx_get_token(&p, ":");
        printf(" -> |%s|", q);
        printf(" + |%s|\n", p);
        fx_free(q);
    }

    printf("Testing fx_append_path_component()\n");
    #define TEST_APPEND_PATH_COMPONENT(path, component, expected) \
        fullpath = fx_append_path_component((path), (component)); \
        printf("%s = %s\n", fullpath ? fullpath : "(null)", expected); \
        fx_free(fullpath);
    TEST_APPEND_PATH_COMPONENT(NULL, NULL, "(null)")
    TEST_APPEND_PATH_COMPONENT("path", NULL, "path");
    TEST_APPEND_PATH_COMPONENT(NULL, "comp", "comp");
    TEST_APPEND_PATH_COMPONENT("path", "comp", "path/comp");
    TEST_APPEND_PATH_COMPONENT("path/", "comp", "path/comp");
    TEST_APPEND_PATH_COMPONENT("path", "/comp", "path/comp");
    TEST_APPEND_PATH_COMPONENT("path/", "/comp", "path/comp");
    TEST_APPEND_PATH_COMPONENT("path/path2/", "/comp/comp2", "path/path2/comp/comp2");
    return 0;
}



static void bprint_bytes(AVBPrint *bp, const uint8_t *ubuf, size_t ubuf_size)
{
    int i;
    //fx_bprintf(bp, "0X");
    for (i = 0; i < ubuf_size; i++)
        fx_bprintf(bp, "%02X", ubuf[i]);
}

static inline int validate_string(char **dstp, const char *src)
{
    const uint8_t *p, *endp;
    AVBPrint dstbuf;
    //int invalid_chars_nb = 0;
    int ret = 0;
    wchar_t *filename_w;
    uint8_t buf[100]={0};
    char *pb=buf;
    int i=0;
    int invalid = 0;
    fx_bprint_init(&dstbuf, 0, FX_BPRINT_SIZE_UNLIMITED);

    endp = src + strlen(src);
    for (p = (uint8_t *)src; *p;) {
        uint32_t code;
        const uint8_t *p0 = p;

        if (fx_utf8_decode(&code, &p, endp, FX_UTF8_FLAG_EXCLUDE_XML_INVALID_CONTROL_CODES) < 0) {
            AVBPrint bp;
            fx_bprint_init(&bp, 0, FX_BPRINT_SIZE_AUTOMATIC);
            bprint_bytes(&bp, p0, p-p0);
            //pb+=sprintf(pb,"%s",bp.str);
            //if(!invalid){
            //    buf[i]=

            //}
            strncat(buf,p0,p-p0);
            fx_log(NULL, FX_LOG_INFO,
                   "Invalid UTF-8 sequence %s found in string '%s'\n", bp.str, src);
            invalid = 1;
        }
        fx_log(NULL, FX_LOG_INFO, "%x:%c\n", code,code);

        //if (invalid) {
        //    invalid_chars_nb++;

        //    switch (wctx->string_validation) {
        //    case WRITER_STRING_VALIDATION_FAIL:
        //        fx_log(wctx, fx_LOG_ERROR,
        //               "Invalid UTF-8 sequence found in string '%s'\n", src);
        //        ret = AVERROR_INVALIDDATA;
        //        goto end;
        //        break;

        //    case WRITER_STRING_VALIDATION_REPLACE:
        //        fx_bprintf(&dstbuf, "%s", wctx->string_validation_replacement);
        //        break;
        //    }
        //}

        //if (!invalid || wctx->string_validation == WRITER_STRING_VALIDATION_IGNORE)
        //    fx_bprint_append_data(&dstbuf, p0, p-p0);
    }
    utf8towchar(buf,&filename_w);
    fwprintf(stdout,L"%s=%ls\n",buf,filename_w);
    fx_free(filename_w);

    //if (invalid_chars_nb && wctx->string_validation == WRITER_STRING_VALIDATION_REPLACE) {
    //    fx_log(wctx, fx_LOG_WARNING,
    //           "%d invalid UTF-8 sequence(s) found in string '%s', replaced with '%s'\n",
    //           invalid_chars_nb, src, wctx->string_validation_replacement);
    //}
    //ret = MoveFileExW(src, buf, MOVEFILE_REPLACE_EXISTING);
//end:
    fx_bprint_finalize(&dstbuf, dstp);
    return ret;
}

int main(){
    //char *src="Act.of.Valor.勇者行动.2012.1024x576.中英字幕.mkv.txt";
    char *src="Act.of.Valor.勇者行动.2012.1024x576.mkv.txt";
    char *dst=NULL;
    validate_string(&dst,src);
    fx_log(NULL, FX_LOG_INFO, "%s\n", dst);


}


#endif