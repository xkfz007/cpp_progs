#ifndef _FACTORY_H_
#define _FACTORY_H_
class Product;
class Factory{
    public:
        virtual ~Factory();
        virtual Product* CreateProduct(int)=0;
    protected:
        Factory();
};
class ConcreteFactory:
    public Factory{
        public:
            ConcreteFactory();
            ~ConcreteFactory();
            Product* CreateProduct(int);
    };

#endif
