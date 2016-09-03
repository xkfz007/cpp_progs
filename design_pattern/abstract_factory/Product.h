#ifndef _PRODUCT_H_
#define _PRODUCT_H_
class AbstractProductA{
    public:
        virtual ~AbstractProductA();
        virtual void Operation()=0;
    protected:
        AbstractProductA();
};
class AbstractProductB{
    public:
        virtual ~AbstractProductB();
        virtual void Operation()=0;
    protected:
        AbstractProductB();
};
class ProductA0:
    public AbstractProductA{
        public:
            ProductA0();
            ~ProductA0();
            void Operation();
    };
class ProductA1:
    public AbstractProductA{
        public:
            ProductA1();
            ~ProductA1();
            void Operation();
    };
class ProductB0:
    public AbstractProductB{
        public:
            ProductB0();
            ~ProductB0();
            void Operation();
    };
class ProductB1:
    public AbstractProductB{
        public:
            ProductB1();
            ~ProductB1();
            void Operation();
    };
#endif
