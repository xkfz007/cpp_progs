#ifndef _DIRECTOR_H_
#define _DIRECTOR_H_
class Builder;
class Director{
    public:
        Director(Builder*);
        ~Director();
        void Construct();
    private:
        Builder* _bldr;
};

#endif
