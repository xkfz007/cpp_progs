/*
 * this program package the arguments to a class using vector.
 */
#ifdef _VECARGLIST
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using std::string;
using std::vector;
using std::endl;
using std::cout;
using std::ifstream;
class Arglist
{
public:
    Arglist(size_t,char**);
    size_t count() const;
    const string& operator[](size_t)const;
private:
    vector<string>args;
    void expand(char*);
    void add(char*);
};
Arglist::Arglist(size_t arg_count,char**arg_vec)
{
    for(int i=0;i<arg_count;++i)
        if(arg_vec[i][0]=='@')
            expand(arg_vec[i]+1);
        else
            add(arg_vec[i]);
}
size_t Arglist::count()const
{
    return args.size();
}
const string& Arglist::operator[](size_t i)const
{
    return args[i];
}
void Arglist::add(char*arg)
{
    args.push_back(arg);
}
void Arglist::expand(char*fname)
{
    ifstream f(fname);
    char token[64];
    while(f>>token)
        if(token[0]=='@')
            expand(token+1);
        else
            add(token);

}

int main(int argc,char*argv[])
{
    Arglist args(--argc,++argv);
    for(int i=0;i<args.count();++i)
        cout<<"arg["<<i<<"]=="<<args[i]<<endl;

}
#endif