/*
** Shrink runs of white space in the given string to a single space.
*/
#define _DEBLANK
#ifdef _DEBLANK
#define NUL '\0'
void deblank( char *string )
{
	char *dest;
	char *src;
	int ch;
	/*
	** Set source and destination pointers to beginning of the string, then
	** move to 2nd character in string.
	*/
	src = string;
	dest = string++;
	/*
	** Examine each character from the source string.
	*/
	while( (ch = *src++) != NUL ){
		if( is_white( ch ) ){
			/*
			** We found white space. If we¡¯re at the beginning of
			** the string OR the previous char in the dest is not
			** white space, store a blank.
			*/
			if( src == string || !is_white( dest[-1] ) )
				*dest++ = ' ';
		}
		else {
			/*
			** Not white space: just store it.
			*/
			*dest++ = ch;
		}
	}
	*dest = NUL;
}
int
	is_white( int ch )
{
	return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\n'
		|| ch == '\r';
}
#endif