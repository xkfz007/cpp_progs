#ifndef _BUILDER_H_
#define _BUILDER_H_
class Builder{
    public:
        Builder();
        virtual ~Builder();
        virtual void BuildPartA()=0;
        virtual void BuildPartB()=0;
        virtual void BuildPartC()=0;
};
class ConcreteBuilder0:
    public Builder{
        public:
            ConcreteBuilder0();
            ~ConcreteBuilder0();
            void BuildPartA();
            void BuildPartB();
            void BuildPartC();
    };
class ConcreteBuilder1:
    public Builder{
        public:
            ConcreteBuilder1();
            ~ConcreteBuilder1();
            void BuildPartA();
            void BuildPartB();
            void BuildPartC();
    };

#endif
