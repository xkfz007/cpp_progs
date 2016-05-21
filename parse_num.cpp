#ifdef _PARSE_NUM
/*
读取文件中的数字-C++代码
示例文件：
读入数据为：
5 34  324w4 23w 33
323 23f 23 wer 234
; ?
112
12345 

*/
#include <string>
#include <iostream>
#include <fstream>
using namespace std ;

int main (int argc ,char **argv ){
    if( argc<2){
        cerr<< "Invaild input"<<endl ;
        return -1;
    }
    ifstream fin( argv[1]);
    if(! fin){
        cerr<< "Invaild file:"<<argv [1]<<endl;
        return -2;
    }
    string line;
    string digits("0123456789" );
    while( getline(fin ,line )){
        cout<< "Read from file: "<<line <<endl;
        cout<< ">>>>:";
        string word ;
        string::size_type pos =0,pos2 =0;
        while((pos2 =line .find_first_of (digits ,pos ))!=string ::npos ){
            pos= line. find_first_not_of(digits ,pos2 );
            if( pos!= string::npos ){
                word= line. substr(pos2 ,pos -pos2 );
            }
            else{
                word= line. substr(pos2 );
            }
            cout<< word<< ' ';
        }
        cout<< endl;
    }

    return 0;
}


#endif