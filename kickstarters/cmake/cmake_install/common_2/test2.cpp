#include <test2.h>

test2_class::test2_class():something(0){};
int test2_class::decrease(){ something--; std::cout << "counter = " << something << std::endl; return something; };

