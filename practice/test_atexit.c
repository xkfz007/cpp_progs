/* Fig. 14.4: fig14_04.c
   Using the exit and atexit functions */
//#define _TEST_ATEXIT
#ifdef _TEST_ATEXIT
#include <stdio.h>
#include <stdlib.h>

void print( void ); /* prototype */

int main( void )
{ 
   int answer; /* user's menu choice */

   atexit( print ); /* register function print */
   printf( "Enter 1 to terminate program with function exit"
      "\nEnter 2 to terminate program normally\n" );
   scanf( "%d", &answer );

   /* call exit if answer is 1 */
   if ( answer == 1 ) { 
      printf( "\nTerminating program with function exit\n" );
      exit( EXIT_SUCCESS );
   } /* end if */

   printf( "\nTerminating program by reaching the end of main\n" );
   return 0; /* indicates successful termination */
} /* end main */

/* display message before termination */
void print( void )
{ 
   printf( "Executing function print at program "
      "termination\nProgram terminated\n" );
} /* end function print */
#endif