#ifdef _STRINGS
#include <iostream>
#include <cstring>
#include <cstdlib>
using std::cout;
using std::endl;
using std::ostream;
using std::istream;

class String {
    char *p;
    int size;
    public:
    String(char *str);
    String(void);
    String(const String &obj);           // Copy constructor
    ~String(void) {delete [] p;}

    friend ostream &operator<<(ostream &stream, String &obj);
    friend istream &operator>>(istream &stream, String &obj);

    String& operator=(String &obj);       // assign a String object
    String& operator=(char *s);            // assign a quoted string
    String& operator+(String &obj);       // concatenate a String object
    String& operator+(char *s);            // concatenate a quoted string
    friend String operator+(char *s, String &obj);
    /* concatenates a quoted string with a String object */

    String& operator-(String &obj);       // subtract a String object
    String& operator-(char *s);            // subtract a quoted string

    /* relational operators between String objects. Note that the operators could
       just as easily return bool, rather than int */

    int operator==(String &obj) {return !strcmp(p, obj.p);}
    int operator!=(String &obj) {return strcmp(p, obj.p);}
    int operator<(String &obj) {return strcmp(p, obj.p) < 0;}
    int operator>(String &obj) {return strcmp(p, obj.p) > 0;}
    int operator<=(String &obj) {return strcmp(p, obj.p) <= 0;}
    int operator>=(String &obj) {return strcmp(p, obj.p) >= 0;}

    /* relational operators between String object and a quoted character string.
       Note that the operators could just as easily return bool, rather than int */

    int operator==(char *s) {return !strcmp(p, s);}
    int operator!=(char *s) {return strcmp(p, s);}
    int operator<(char *s) {return strcmp(p, s) < 0;}
    int operator>(char *s) {return strcmp(p, s) > 0;}
    int operator<=(char *s) {return strcmp(p, s) <= 0;}
    int operator>=(char *s) {return strcmp(p, s) >= 0;}

    int strsize(void) {return strlen(p);}      // return string size
    void makestr(char *s) {strcpy(s, p);}  // make quoted string from String object

    operator char *(void) {return p;}          // conversion to char
};

String::String(void)
{
    size = 1;
    p = new char[size];
    if(!p)
    {
        cout << "Allocation error!" << endl;
        exit(1);
    }
    *p = '\0';
}

String::String(char *str)
{
    size = strlen(str) + 1;
    p = new char[size];
    if(!p)
    {
        cout << "Allocation error!" << endl;
        exit(1);
    }
    strcpy(p, str);
}

String::String(const String &obj)
{
    size = obj.size;
    p = new char[size];
    if(!p)
    {
        cout << "Allocation error!" << endl;
        exit(1);
    }
    strcpy(p, obj.p);
}

ostream &operator<<(ostream &stream, String &obj)
{
    stream << obj.p;
    return stream;
}

istream &operator>>(istream &stream, String &obj)
{
    char t[255];      // arbitrary string length--yours can be larger
    int len;

    for(len=0; len<255; len++)
    {
        stream.get(t[len]);
        if(t[len]=='\n')
            break;
        if(t[len]=='\b')
            if(len)
            {
                len--;
                cout << "'\b'";
            }
    }
    t[len]='\0';
    len++;

    if(len>obj.size)
    {
        delete obj.p;
        obj.p = new char[len];
        if(!obj.p)
        {
            cout << "Allocation error!" << endl;
            exit(1);
        }
        obj.size = len;
    }
    strcpy(obj.p, t);
    return stream;
}

String& String::operator=(String &obj)
{
    if(this!=&obj)
    {
        if(obj.size > size)
        {
            delete p;
            p = new char[obj.size];
            size = obj.size;
            if(!p)
            {
                cout << "Allocation error!" << endl;
                exit(1);
            }
        }
        strcpy(p, obj.p);
    }
    return *this;
}

String& String::operator=(char *s)
{
    int len = strlen(s) + 1;

    if(size < len)
    {
        delete p;
        p = new char[len];
        size = len;
        if(!p)
        {
            cout << "Allocation error!" << endl;
            exit(1);
        }
    }
    strcpy(p, s);
    return *this;
}

String& String::operator+(String &obj)
{
    int len;
    char* q=new char[size];
    strcpy(q,p);

    delete p;
    len = strlen(obj.p) + strlen(p) + 1;
    p = new char[len];
    size = len;
    if(!p)
    {
        cout << "Allocation error!" << endl;
        exit(1);
    }
    strcpy(p, q);
    strcat(p, obj.p);
    delete q;
    return *this;
}

String& String::operator+(char *s)
{
    int len;
    char* q=new char[size];
    strcpy(q,p);

    delete p;
    len = strlen(s) + strlen(p) + 1;
    p = new char[len];
    size = len;
    if(!p)
    {
        cout << "Allocation error!" << endl;
        exit(1);
    }
    strcpy(p, q);
    strcat(p, s);
    delete q;
    return *this;
}

String operator+(char *s, String &obj)
{
    int len;
    String temp;

    delete temp.p;
    len = strlen(s) + strlen(obj.p) + 1;
    temp.p = new char[len];
    temp.size = len;
    if(!temp.p)
    {
        cout << "Allocation error!" << endl;
        exit(1);
    }
    strcpy(temp.p, s);
    strcat(temp.p, obj.p);
    return temp;
}

String& String::operator-(String &substr)
{
    String temp(p);
    char *s1;
    int i,j;

    s1 = p;
    for(i=0; *s1; i++)
    {
        if(*s1!=*substr.p)
        {
            temp.p[i] = *s1;
            s1++;
        }
        else
        {
            for(j=0; substr.p[j]==s1[j] && substr.p[j]; j++)
                ;
            if(!substr.p[j])
            {
                s1 += j;
                i--;
            }
            else
            {
                temp.p[i] = *s1;
                s1++;
            }
        }
    }
    temp.p[i] = '\0';
    *this=temp;
    return *this;
}

String& String::operator-(char *substr)
{
    String temp(p);
    char *s1;
    int i,j;

    s1 = p;
    for(i=0; *s1; i++)
    {
        if(*s1!=*substr)
        {
            temp.p[i] = *s1;
            s1++;
        }
        else
        {
            for(j=0; substr[j]==s1[j] && substr[j]; j++)
                ;
            if(!substr[j])
            {
                s1 += j;
                i--;
            }
            else
            {
                temp.p[i] = *s1;
                s1++;
            }
        }
    }
    temp.p[i] = '\0';
    *this=temp;
    return *this;
}
int main()
{
    char *s="hello world";
    String str(s);
    cout<<str<<endl;

    return 0;
}
#endif

/*
 * implementation of String class
 */
#ifdef STRING
#include <iostream>
#include <cstring>
#include <iomanip>

using std::istream;
using std::ostream;
using std::cout;
using std::endl;
using std::cin;
using std::setw;

class String{
    public:
        friend istream& operator>>(istream&,String&);
        friend ostream& operator<<(ostream&,const String&);
        friend String operator+(const String&,const String&);
    public:
        String():_string(0),_size(0){}
        String(int,char);
        String(const char*);
        String(const String&);
        ~String();
        String& operator=(const String&);
        String& operator=(const char*);
        bool operator==(const String &)const;
        bool operator==(const char*)const;
        char operator[](int)const;//
        int size(){return _size;}
        char* c_str(){return _string;}
    private:
        char* _string;
        int _size;
};
istream& operator>>(istream& is,String& str){
    const int limit_string_size=4096;
    char inBuf[limit_string_size];
    is>>setw(limit_string_size)>>inBuf;
    str=inBuf;
    return is;
}
ostream& operator<<(ostream& os,const String& str){
    os<<str._string;
    return os;
}
String::String(int n,char ch)
{
    _string=new char[n+1];
    for(int i=0;i<n;i++)
        _string[i]=ch;
    _string[n]='\0';
    _size=n;
}
String::String(const char* str)
{
    if(!str){
        _size=0;
        _string=NULL;
    }
    else{
        _size=strlen(str);
        _string=new char[_size+1];
        strcpy(_string,str);
    }
}
String::String(const String& str)
{
    _size=str._size;
    if(!str._string)
        _string=0;
    else{
        _string=new char[_size+1];
        strcpy(_string,str._string);
    }
}
String::~String()
{
    if(_string)
        delete[] _string;
}
String& String::operator=(const String& str)
{
    if(this!=&str){
        if(_size==str._size){
            strcpy(_string,str._string);
        }
        else{
            _size=str._size;
            if(_string)
                delete[] _string;
            if(!str._string)
                _string=NULL;
            else{
                _string=new char[_size+1];
                strcpy(_string,str._string);
            }
        }
    }
    return *this;
}
String& String::operator=(const char*str)
{
    if(!str){
        _size=0;
        if(_string)
            delete[] _string;
        _string=NULL;
    }
    else{
        _size=strlen(str);
        if(_string)
            delete[] _string;
        _string=new char[_size+1];
        strcpy(_string,str);
    }
    return *this;
}
bool String::operator==(const String& str)const
{
    if(_size!=str._size)
        return false;
    else
        return strcmp(_string,str._string)==0?true:false;
}
bool String::operator==(const char* str)const
{
    return strcmp(_string,str)==0?true:false;
}
char String::operator[](int n)const
{
    if(n>=0&&n<_size)
        return _string[n];
    else
        return -1;
}
String operator+(const String&str1,const String& str2)
{
    String newstr;
    if(str1._size==0&&str2._size==0)
        return newstr;
    else if(str1._size==0&&str2._size!=0){
        newstr=str2;
    }
    else if(str1._size!=0&&str2._size==0){
        newstr=str1;
    }
    else{
        newstr._size=str1._size+str2._size;
        newstr._string=new char[newstr._size+1];
        strcpy(newstr._string,str1._string);
        strcat(newstr._string,str2._string);
    }
    return newstr;

}
int main()
{
    int aCnt = 0, eCnt = 0, iCnt = 0, oCnt = 0, uCnt = 0,
        theCnt = 0, itCnt = 0, wdCnt = 0, notVowel = 0;
    // 为了使用 operator==( const char* )
    // 我们并不定义 The( "The" )和 It( "It" )
    String buf, the( "the" ), it( "it" );
    // 调用 operator>>( istream&, String& )
    while ( cin >> buf ) {
        ++wdCnt;
        // 调用 operator<<( ostream&, const String& )
        cout << buf << ' ';
        if ( wdCnt % 12 == 0 )
            cout << endl;
        // 调用 String::operator==(const String&) and
        //
        if ( buf == the || buf == "The" )
            ++theCnt;
        if ( buf == it || buf == "It" )
                ++itCnt;
        // 调用 String::size()
        for ( int ix = 0; ix < buf.size(); ++ix )
        {
            // 调用 String::operator[](int)
            switch( buf[ ix ] )
            {
                case 'a': case 'A': ++aCnt;
                                    break;
                case 'e': case 'E': ++eCnt;
                                    break;
                case 'i': case 'I': ++iCnt;
                                    break;
                case 'o': case 'O': ++oCnt;
                                    break;
                case 'u': case 'U': ++uCnt;
                                    break;
                default: ++notVowel; break;
            }
        }
    }
    // 调用 operator<<( ostream&, const String& )
    cout << "\n\n"
        <<"Words read: " << wdCnt << "\n\n"
        <<"the/The: " << theCnt << '\n'
        <<"it/It: " << itCnt << "\n\n"
        <<"non-vowels read: " << notVowel << "\n\n"
        <<"a: " << aCnt << '\n'
        <<"e: " << eCnt << '\n'
        <<"i: " << iCnt << '\n'
        <<"o: " << oCnt << '\n'
        <<"u: " << uCnt << endl;

    String a="hello";
    String b="world";
    String c=a+b;
    cout<<c<<endl;

    return 0;

}
#endif
