/*
 * str_match.cc
 *
 *  Created on: 2016-5月-23 下午10:23:55
 *      Author: Felix
 */
#include <iostream>
using namespace std;
template <class ElemType>
void NativeStrMatching( ElemType Target[], ElemType Pattern[] ) {
	register int TarLen = 0; // Length of Target
	register int PatLen = 0; // Length of Pattern
	// Compute the length of Pattern
	while( '\0' != Pattern[PatLen] )
		PatLen++;
	while( '\0' != Target[TarLen] ) {
		int TmpTarLen = TarLen;
		for(int i=0; i<PatLen; i++) {
			if( Target[TmpTarLen++] != Pattern[i] )
				break;
			if( i == PatLen-1 )
				cout<<"Native String Matching,pattern occurs with shift "<<TarLen<<endl;
		} TarLen++;
	}
}
#define MAX_SIZE 100
// Compute Prefix function
template <class ElemType>
void CptPfFunc( ElemType Pattern[], int PrefixFunc[] )
{
    register int iLen = 0; // Length of Pattern[]
    while( '\0' != Pattern[iLen] )
        iLen++;
    int LOLP = 0; // Lenth of longest prefix
    PrefixFunc[1] = 0;
    for( int NOCM=2; NOCM<iLen+1; NOCM++ ) // NOCM represent the number of characters matched
    	{
        while( LOLP>0 && (Pattern[LOLP] != Pattern[NOCM-1]) )
            LOLP = PrefixFunc[LOLP];
        if( Pattern[LOLP] == Pattern[NOCM-1] )
            LOLP++;
        PrefixFunc[NOCM] = LOLP;
    }
}
template <class ElemType>
void KMPstrMatching( ElemType Target[], ElemType Pattern[] )
{
    int PrefixFunc[MAX_SIZE];
    register int TarLen = 0;
    register int PatLen = 0;
    // Compute the length of array Target and Pattern
    while( '\0' != Target[TarLen] )
        TarLen++;
    while( '\0' != Pattern[PatLen] )
        PatLen++;
    // Compute the prefix function of Pattern    CptPfFunc( Pattern, PrefixFunc );
    int NOCM = 0; // Number of characters matched
    for( int i=0; i<TarLen; i++ )
    {
        while( NOCM>0 && Pattern[NOCM] != Target[i] )
            NOCM = PrefixFunc[NOCM];
        if( Pattern[NOCM] == Target[i] )
            NOCM++;
        if( NOCM == PatLen )
        {
            cout<<"KMP String Matching,pattern occurs with shift "<<i - PatLen + 1<<endl;
            NOCM = PrefixFunc[NOCM];
        }
    }
}





