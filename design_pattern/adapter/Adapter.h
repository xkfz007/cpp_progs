#ifndef _ADAPTER_H_
#define _ADAPTER_H_
class Target{
    public:
        Target();
        virtual ~Target();
        virtual void Request();
};
class Adaptee{
    public:
        Adaptee();
        ~Adaptee();
        void SpecificRequest();
};
class Adapter:public Target{
    public:
        Adapter(Adaptee* adp);
        void Request();
        ~Adapter();
    private:
        Adaptee* _adp;
};

#endif
