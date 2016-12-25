#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glob.h>
/*  Locally define as 0 (bitwise-OR no-op) any missing glob options that
 *     are non-posix glibc/bsd extensions. */
#ifndef GLOB_NOMAGIC
#define GLOB_NOMAGIC 0
#endif
#ifndef GLOB_BRACE
#define GLOB_BRACE 0
#endif

static void print_gl(glob_t *gl)
{
    unsigned int i;
    for(i = 0; i < gl->gl_pathc; ++i)
        printf("%s\n", gl->gl_pathv[i]);
    printf("++++++++++++++++++++++\n");
}
static int test_fun(int argc, char *argv[])
{
    glob_t gl;
    int i;
    for(i = 1; i < argc; ++i) {
        gl.gl_offs = 0;
        glob(argv[i], GLOB_NOCHECK|GLOB_BRACE|GLOB_NOMAGIC, 0, &gl);
        print_gl(&gl);
        globfree(&gl);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc > 1)
        test_fun(argc, argv);
    else
        printf("./mytest {'path list'}\n");
    return 0;
}

