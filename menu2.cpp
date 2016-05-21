/*
 * this program demenstrates the method to list a menu on the screen
 */
#ifdef MENU2
#include <iostream>
using namespace std;
class Object
{
    public:
    void retrieve()
    {
        cout<<"Object::retrieve\n";
    }
    void insert()
    {
        cout<<"Object::insert\n";
    }
    void update()
    {
        cout<<"Object::update\n";
    }
    void process(int choice);
    private:
    typedef void (Object::*Omf)();
    static Omf farray[3];
};
Object::Omf Object::farray[3]=
{
    &Object::retrieve,
    &Object::insert,
    &Object::update,

};
void Object::process(int choice)
{
    if(choice>=0&&choice<=2)
        (this->*farray[choice])();
}
int show_menu()
{
    int choice;
    cout<<"Menu:"<<endl;
    cout<<" 1. retrieve "<<endl;
    cout<<" 2. insert "<<endl;
    cout<<" 3. update "<<endl;
    cout<<"Enter your choice:"<<endl;
    cin>>choice;
    return choice;
}
main()
{
    Object o;
    for(;;)
    {
        int choice=show_menu();
        if(choice>=1&&choice<=3)
            o.process(choice-1);
        else
            break;
    }
}

#endif