//#include <error_class.h>
#include <tcp_error_defs.h>
#include <iostream>
#include <return_type_class.h>

Returntype<int> divide(int a, int b) noexcept {
  if (b == 0) {
    return Returntype<int>(Nothing(),error_category_def::error);
  } else {
    return a/b;
  }
}

int main(void) {
  Returntype<int> result = divide(10, 0);
  if (result) { // Check that result is not Nothing
    printf("I got a %d\n", result.unwrap());
  } else {
    printf("You divided by 0... the world blew up... Thanks\n");
    result.print_error_state();
  }
  
  result = divide(100,10);
  if (result) { // Check that result is not Nothing
    printf("I got a %d\n", result.unwrap());
  } else {
    printf("An error occured, lets see ...\n");
    result.print_error_state();
}

  return 0;
}
