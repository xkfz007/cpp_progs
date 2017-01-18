/* ACCESS.C: This example uses _access to check the
 * file named "ACCESS.C" to see if it exists and if
 * writing is allowed.
 */

#ifdef WIN32
#include  <io.h>
#define access _access
#else
#include <unistd.h>
#endif
#include  <stdio.h>
#include  <stdlib.h>

int main(int argc, char** argv)
{
    /* Check for existence */
    if( (access( argv[0], 0 )) != -1 )
    {
        printf( "File ACCESS.C exists " );
        ///* Check for write permission */
        //if( (access( "ACCESS.C", 2 )) != -1 )
        //    printf( "File ACCESS.C has write permission " );
    }
}
