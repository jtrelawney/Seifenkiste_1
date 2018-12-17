//#include <error_class.h>

#include <iostream>
#include <tuple>

std::tuple<bool,int> divide(int a, int b) noexcept {
  if (b == 0) {
    return std::make_tuple<bool,int>(true,0);
  } else {
    return std::make_tuple<bool,int> (false,a/b);
  }
}

int main(void) {
	
  std::tuple<bool,int> result = divide(10, 0);
  if (std::get<0>(result) == false) {
		std::cout << "I got " << std::get<1>(result) << std::endl;
  } else {
		std::cout << "You divided by 0" << std::endl;
  }
  
  result = divide(100,10);
  if (std::get<0>(result) == false) {
		std::cout << "I got " << std::get<1>(result) << std::endl;
  } else {
		std::cout << "You divided by 0" << std::endl;
  }

  return 0;
}
