#ifndef _PROTOTYPE_H_
#define _PROTOTYPE_H_
class Prototype{
    public:
        Prototype();
        virtual ~Prototype();
        virtual Prototype* Clone()=0;
};
class ConcretePrototype0:
    public Prototype{
        public:
            ConcretePrototype0();
            ~ConcretePrototype0();
            ConcretePrototype0(const ConcretePrototype0&);
            Prototype* Clone();
};
class ConcretePrototype1:
    public Prototype{
        public:
            ConcretePrototype1();
            ~ConcretePrototype1();
            ConcretePrototype1(const ConcretePrototype1&);
            Prototype* Clone();
};

#endif
