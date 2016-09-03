#ifndef _SINGLETON_H_
#define _SINGLETON_H_
class Singleton{
    public:
       static Singleton* Instance();
    private:
        Singleton();
    private:
        static Singleton* _instance;
};

#endif
