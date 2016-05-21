// Author:  Keith A. Shomper
// Date:    11/17/03
// Purpose: To demonstrate a class
#ifdef _COMPLEX_NUMBER
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::ostream;
using std::istream;

class complex_number {
 public:
   complex_number(double real = 0.0, double imag = 0.0):r(real),i(imag) {}
   complex_number(const complex_number& rhs):r(rhs.r),i(rhs.i){};
   complex_number operator=(const complex_number& rhs){
       if(this!=&rhs){
           r=rhs.r;
           i=rhs.i;
       }
       return *this;
   }
   complex_number& operator+=(const complex_number& rhs);
   complex_number& operator-=(const complex_number& rhs);
   complex_number& operator*=(const complex_number& rhs);
   complex_number& operator/=(const complex_number& rhs);

   friend complex_number operator+ (const complex_number &a, const complex_number &b);
   friend complex_number operator- (const complex_number &a, const complex_number &b);
   friend complex_number operator* (const complex_number &a, const complex_number &b);
   friend complex_number operator/ (const complex_number &a, const complex_number &b);

   friend ostream& operator<<(ostream& os, const complex_number& rhs);
 private:
	double r;
	double i;
};
complex_number& complex_number::operator+=(const complex_number& rhs){
    r+=rhs.r;
    i+=rhs.i;
    return *this;
}
complex_number& complex_number::operator-=(const complex_number& rhs){
    r-=rhs.r;
    i-=rhs.i;
    return *this;
}
complex_number& complex_number::operator*=(const complex_number& rhs){
	r = (r * rhs.r - i * rhs.i);
	i = (r * rhs.i + i * rhs.r);
    return *this;
}
complex_number& complex_number::operator/=(const complex_number& rhs){
	r = (r * rhs.r + i * rhs.i) / (rhs.r * rhs.r + rhs.i * rhs.i);
	i = (i * rhs.r - r * rhs.i) / (rhs.r * rhs.r + rhs.i * rhs.i);
    return *this;
}

complex_number operator+ (const complex_number &a, const complex_number &b) {
	complex_number result(a);
    result+=b;

	return result;
}
complex_number operator- (const complex_number &a, const complex_number &b) {
	complex_number result(a);
    result-=b;

	return result;
}

complex_number operator* (const complex_number &a, const complex_number &b) {
	complex_number result(a);
    result*=b;

	return result;
}

complex_number operator/ (const complex_number &a, const complex_number &b) {
	complex_number result(a);
    result/=b;
	return result;
}

ostream& operator<<(ostream& os,const complex_number& rhs){
    os<<rhs.r<<"+"<<rhs.i<<"i";
}

int main () {
	complex_number x(1, 2), y, z;
	complex_number sum, difference, product, quotient;

	y = complex_number(2, 4);

	z = complex_number(3, 0);

	cout << "x is:  "<< x<< endl;
	cout << "y is:  "<< y<< endl;
	cout << "z is:  "<< z<< endl;

	sum        = x + y;
	difference = x - y;
	product    = x * y;
	quotient   = sum / z;
	
	cout << "The sum (x + y) is:  "<<sum << endl;
	cout << "The difference (x - y) is:  "<<difference<< endl;
	cout << "The product (x * y) is:  "<<product << endl;
	cout << "The quotient (sum / z) is:  "<<quotient << endl;
    complex_number a=5;
    cout<<"a="<<a<<endl;
    a+=3;
    cout<<"a="<<a<<endl;

	return 0;
}
#endif