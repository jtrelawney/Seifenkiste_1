#include <test1.h>
#include <test2.h>

int main(){
    test1_class tc1;
    tc1.increase();
    int t1 = tc1.increase();
    test2_class tc2;
    tc2.decrease();
    int t2 = tc2.decrease();
    std::cout << "added up  = " << t1 + t2 << std::endl;
}
