#ifndef _PRODUCT_H_
#define _PRODUCT_H_
class Product{
    public:
        virtual ~Product();
        virtual void Operation()=0;
    protected:
        Product();
};
class ConcreteProduct1:
    public Product{
        public:
            ConcreteProduct1();
            ~ConcreteProduct1();
            void Operation();
    };
class ConcreteProduct0:
    public Product{
        public:
            ConcreteProduct0();
            ~ConcreteProduct0();
            void Operation();
    };

#endif
