#ifndef _ITERATOR_H_
#define _ITERATOR_H_
template <class T>
class Aggregate;
template <class T>
class Iterator{
    public:
        Iterator(Aggregate<T>*agg):_agg(agg),_indx(-1){}
        virtual ~Iterator(){}
        virtual void First()=0;
        virtual void Next()=0;
        virtual bool IsDone()=0;
        virtual T& CurrentItem()=0;
    protected:
        Aggregate<T>* _agg;
        int _indx;
};
template <class T>
class Aggregate{
    public:
        virtual ~Aggregate(){}
        virtual Iterator<T>* CreateItertor(Aggregate<T>*)=0;
        virtual int GetSize()=0;
        virtual T& GetItem(int i)=0;
};
template <class T>
class ConcreteIterator:public Iterator<T>{
    public:
        ConcreteIterator(Aggregate<T>*agg):Iterator<T>(agg){}
        void First(){ this->_indx=0; }
        void Next(){this->_indx++;}
        bool IsDone(){
            return this->_indx==this->_agg->GetSize();
        }
        T& CurrentItem(){
            return this->_agg->GetItem(this->_indx);
        }
};
template <class T>
class ConcreteAggregate:public Aggregate<T>{
    public:
        ConcreteAggregate(int sz):_arr(new T[sz]()),_sz(sz){}
        ~ConcreteAggregate(){ delete[] _arr; }
        Iterator<T>* CreateItertor(Aggregate<T>* agg){
            return new ConcreteIterator<T>(this);
        }
        int GetSize(){
            return _sz;
        }
        T& GetItem(int i){
            return _arr[i];
        }
    private:
        T *_arr;
        int _sz;
};

#endif
