
/*
** Compute and print all the prime numbers from 1 to 100.
*/
//#define _PRIME_PRINT
#ifdef _PRIME_PRINT
#include <stdio.h>
#include <stdlib.h>
int main()
{
	int number;
	int divisor;
	/*
	** One and two are easy.
	*/
	printf( "1\n2\n" );
	/*
	** No other even numbers are prime; look at the remaining odd ones.
	*/
	for( number = 3; number <= 100; number = number + 2 ){
		/*
		** See if any divisor from 3 up to the number evenly divides the
		** number.
		*/
		for( divisor = 3; divisor < number; divisor = divisor + 2 ){
			if( number % divisor == 0 )
				break;
		}
		/*
		** If the loop above stopped because the divisor got too big,
		** we¡¯ve got a prime.
		*/
		if( divisor >= number )
			printf( "%d\n", number );
	}
}

#endif