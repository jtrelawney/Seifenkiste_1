#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <cstring> //memset

class B{
private:
    int i;
public:
    B(){i=1;};
    int get_val(){ return i; };
    void set_val(int v){ i=v; };

    friend std::ostream& operator<< (std::ostream &out, const B &b);
};

class A {

private:
    int i;
public:
    A(){i=0;};
    int get_val(){ return i; };
    int set_val(int v){ i=v; };

    friend std::ostream& operator<< (std::ostream &out, const A &a);

    A& operator << (B &b){
        int new_a = b.get_val();
        int new_b = i + new_a;

        std::cout << *this << "   " << b << std::endl;
        
        std::cout << "apply fib    ->     a = " << new_a << "   b = " << new_b << std::endl;
        i = new_a;
        b.set_val(new_b);
    };
};


std::ostream& operator<< (std::ostream &out, const B &b){
    out << "B = " << b.i;
    return out;
}

std::ostream& operator<< (std::ostream &out, const A &a){
    out << "A = " << a.i;
    return out;
}


int main(void)
{

    A a;
    B b;
    
    a << b;
    a << b;
    a << b;
    a << b;
    a << b;
    a << b;
    a << b;
    a << b;

}
