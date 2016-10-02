#ifdef _WEAK_PTR_USAGE
#include <memory>
#include <iostream>
#include <string>
using namespace std;

class B;
class A {
    public:
        //shared_ptr<B> _pB;
        weak_ptr<B> _pB;
        ~A() { cout << ">> ~A() << " << endl; }
};
class B {
    public:
        weak_ptr<A> _pA;
        ~B() { cout << ">> ~B() << " << endl; }
};

void Run() {
    shared_ptr<A> pA(new A);
    shared_ptr<B> pB(new B);
    pA->_pB = pB;
    pB->_pA = pA;
}

int main(int argc, char *argv[]) {
    Run();
    cout << "over" << endl;
    return 0;
}
#endif