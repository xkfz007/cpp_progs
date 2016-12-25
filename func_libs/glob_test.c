//gcc -o glob glob.c -D_GLOB -DHAVE_UNISTD_H=1 -DHAVE_DIRENT_H     
//#define _GLOB
#if defined (_GLOB)
#include "fx_glob.h"
int main (int argc,char* argv[])
{
	unsigned int i;

	for (i = 1; i < argc; ++i)
	{
		char **value = glob_filename (argv[i], 0);
		if (value == NULL)
		puts ("Out of memory.");
		else if (value == &glob_error_return)
		perror (argv[i]);
		else
		for (i = 0; value[i] != NULL; i++)
		puts (value[i]);
	}

	exit (0);
}
#endif	/* TEST.  */
