#include <stdlib.h>
#include <glob.h>
#include <stdio.h>
int main2(int argc,char* argv[]){
    glob_t glob_buffer;
    const char * pattern = argv[1];
    int i;
    int match_count;

    glob( pattern , 0 , NULL , &glob_buffer  ); 
    match_count = glob_buffer.gl_pathc;
    printf("Number of mathces: %d \n", match_count);

    for (i=0; i < match_count; i++) 
        printf("match[%d] = %s \n",i,glob_buffer.gl_pathv[i]);

    globfree( &glob_buffer  );
    return 0;
}
int main(int argc,char* argv[]){
    unsigned int i;

    for (i = 1; i < argc; ++i)
    {
        char **value = glob_filename (argv[i], 1);
        if (value == NULL)
            puts ("Out of memory.");
        else if (value == &glob_error_return)
            perror (argv[i]);
        else
            for (i = 0; value[i] != NULL; i++)
                puts (value[i]);

    }
}
