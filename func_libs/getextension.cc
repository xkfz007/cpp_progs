/*
 * getextension.cc
 *
 *  Created on: 2016-5月-21 下午2:19:54
 *      Author: Felix
 */

#include <string.h>
//get extension of from filename
static inline char *get_filename_extension( const char *filename )
{
	char *ext;
	if(!filename)
		return NULL;
    ext = filename + strlen( filename );
    while( *ext != '.' && ext > filename )
        ext--;
    ext += *ext == '.';
    return ext;
}
static char *get_filename_extension_use_strrchr(const char *filename){
	char *ext;
	if(!filename)
		return NULL;
	ext=strrchr(filename,'.');
	return ext;
}



