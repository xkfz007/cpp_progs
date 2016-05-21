/*
 * remove comments in C++ programs
 */
//#define _REMOVE_COMMENTS2
#ifdef _REMOVE_COMMENTS2

#include <iostream>
using namespace std;
enum Context{
    c_comment,cpp_comment,string_literal,char_literal,file_end
};
void handle_c_comment(){
    char ch;
    while(cin.get(ch)){
        if(ch=='*'){
            while(cin.get(ch)&&ch=='*')
                ;
            if(ch=='/')
                break;
        }
    }
}
void handle_cpp_comment(){
    char ch;
    while(cin.get(ch)&&ch!='\n')
        ;
}
void handle_literal(char delimiter){
    cout<<delimiter;
    char ch;
    while(cin.get(ch)){
        cout<<ch;
        if(ch==delimiter)
            break;
        else if (ch=='\\')
            cin.get(ch)&&cout<<ch;
    }
}
Context handle_code(){
    char ch;
    while(cin.get(ch)){
        switch(ch){
            case '/':
                if(!cin.get(ch)){
                    cout<<'/';
                    return file_end;
                }
                else {
                    if(ch=='*')
                        return c_comment;
                    else if(ch=='/')
                        return cpp_comment;
                    else{
                        cout<<'/';
                        cin.putback(ch);
                        break;
                    }
                }
            case '\"':
                return string_literal;
            case '\'':
                return char_literal;
            default:
                cout<<ch;
        }
    }
    return file_end;    
}
int main(int argc,char**argv){
    if(argc!=1){
        cerr<<"This program takes no arguments.\n";
        return -1;
    }
    else{
        Context contxt;
        while((contxt=handle_code())!=file_end)
            switch(contxt){
                case c_comment:
                    handle_c_comment();
                    break;
                case cpp_comment:
                    handle_cpp_comment();
                    break;
                case string_literal:
                    handle_literal('\"');
                    break;
                case char_literal:
                    handle_literal('\'');
                    break;
            }
    }
    return 0;
}
#endif