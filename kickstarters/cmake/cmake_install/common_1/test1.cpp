#include <test1.h>

test1_class::test1_class():something(0){};
int test1_class::increase(){ something++; std::cout << "counter = " << something << std::endl; return something; };
