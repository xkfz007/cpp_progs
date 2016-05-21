/*
 * implementation of the stack using vector
 */
#ifdef _STACK
#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ostream;

template <class T>
class Stack{
    public:
        Stack(int cap=0){
            if(cap)
                _stack.reserve(cap);
        }
        bool pop(T &vaulue);
        bool push(T  value);
        bool full();
        bool empty();
        void display();
        int size();
    private:
//        int _top;
        vector<T> _stack;
};
template<class T>
inline int Stack<T>::size(){
    return _stack.size();
//    return _top;
}
template<class T>
inline bool Stack<T>::empty(){
    return _stack.empty();
//    return _top==0;
}
template<class T>
inline bool Stack<T>::full(){
    return _stack.max_size()==_stack.size();
//    return _top==_stack.size();
}
template<class T>
bool Stack<T>::pop(T &value){
    if(empty())
        return false;
    value=_stack.back();
    _stack.pop_back();
//    value=_stack[--_top];
    cout<<"Stack:pop()"<<value<<endl;
    return true;
}
template<class T>
bool Stack<T>::push(T value){
    if(full())
        return false;
    _stack.push_back(value);
//    _stack[_top++]=value;
    cout<<"Stack:push()"<<value<<endl;
    return true;

}
template<class T>
void Stack<T>::display(){
    if(size()==0) { cout<<"(0)"<<endl;
    }
    else{
        cout<<"("<<size()<<")(bot:";
        for(int i=0;i<size();++i)
            cout<<_stack[i]<<" ";
        cout<<":top)"<<endl;
    }
}
class MyTest{
    friend ostream& operator<<(ostream& os,MyTest& mt){
        os<<mt.str;
        return os;
    }

    public:
        MyTest(string s=""):str(s){
            cout<<"constructor"<<endl;
        };
    private:
        string str;
};
int main()
{
//    Stack<int> stack( 32 );
//    stack.display();
//    for ( int ix = 1; ix < 51; ++ix )
//    {
//        if ( ix%2 == 0 )
//            stack.push( ix );
//        if ( ix%5 == 0 )
//            stack.display();
//        if ( ix%10 == 0) {
//            int dummy;
//            stack.pop( dummy ); stack.pop( dummy );
//            stack.display();
//        }
//    }
     Stack<MyTest> stack(10);
     stack.display();
     stack.push(MyTest("hello"));
     stack.display();
    return 0;
}
#endif