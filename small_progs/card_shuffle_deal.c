/* 
 * Card shuffling dealing program 
 */
//#define _CARD_SHUFFLE_DEAL
#ifdef _CARD_SHUFFLE_DEAL
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* card structure definition */               
struct card {                                 
   const char *face; /* define pointer face */
   const char *suit; /* define pointer suit */
}; /* end structure card */                   

typedef struct card Card; /* new type name for struct card */

/* place strings into Card structures */
void fillDeck( Card * const wDeck, const char * wFace[], 
   const char * wSuit[] )
{ 
   int i; /* counter */

   /* loop through wDeck */
   for ( i = 0; i <= 51; i++ ) { 
      wDeck[ i ].face = wFace[ i % 13 ];
      wDeck[ i ].suit = wSuit[ i / 13 ];
   } /* end for */
} /* end function fillDeck */

/* shuffle cards */
void shuffle( Card * const wDeck )
{ 
   int i; /* counter */
   int j; /* variable to hold random value between 0 - 51 */
   Card temp; /* define temporary structure for swapping Cards */

   /* loop through wDeck randomly swapping Cards */
   for ( i = 0; i <= 51; i++ ) { 
      j = rand() % 52;
      temp = wDeck[ i ];      
      wDeck[ i ] = wDeck[ j ];
      wDeck[ j ] = temp;      
   } /* end for */
} /* end function shuffle */

/* deal cards */
void deal( const Card * const wDeck )
{ 
   int i; /* counter */

   /* loop through wDeck */
   for ( i = 0; i <= 51; i++ ) {
      printf( "%5s of %-8s%s", wDeck[ i ].face, wDeck[ i ].suit,
         ( i + 1 ) % 4 ? "  " : "\n" );
   } /* end for */
} /* end function deal */
int main( void )
{ 
   Card deck[ 52 ]; /* define array of Cards */

   /* initialize array of pointers */
   const char *face[] = { 
      "Ace", "Deuce", "Three", "Four", "Five",
      "Six", "Seven", "Eight", "Nine", "Ten",
      "Jack", "Queen", "King"
   };

   /* initialize array of pointers */
   const char *suit[] = { "Hearts", "Diamonds", "Clubs", "Spades"};

   srand( time( NULL ) ); /* randomize */

   fillDeck( deck, face, suit ); /* load the deck with Cards */
   shuffle( deck ); /* put Cards in random order */
   deal( deck ); /* deal all 52 Cards */
   return 0; /* indicates successful termination */
} /* end main */


#endif
