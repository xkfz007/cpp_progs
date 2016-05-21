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