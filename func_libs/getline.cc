/*
 * implementation of getline
 */


#ifdef _GETLINE_CPP
/*
 * In cpp, the standard lib contains the getline function
 * Actually, the inputstream objects have a member function called getline,
 * and also there is global getline fucntion
 */

//getline member function
/*
 * istream::getline
 * istream& getline (char* s, streamsize n );
 * istream& getline (char* s, streamsize n, char delim );
 */
void test_istream_getline(){
    char line[100];
    while(cin.getline(line,100))
        cout<<line<<endl;
}

//global getline
/*
 * istream& getline ( istream& is, string& str, char delim );
 * istream& getline ( istream& is, string& str );
 */
void test_global_getline(){
    string line;
    while(getline(cin,line))
        cout<<line<<endl;
}

#endif
