//#include "./glob/glob.h"
#include <stdio.h>
extern char **glob_filename (char *, int);

int main(int argc,char* argv[]){
    unsigned int i;

    for (i = 1; i < argc; ++i)
    {
        char **value = glob_filename (argv[i], 1);
        if (value == NULL)
            puts ("Out of memory.");
        else
            for (i = 0; value[i] != NULL; i++){
                puts (value[i]);

            }

    }

}
