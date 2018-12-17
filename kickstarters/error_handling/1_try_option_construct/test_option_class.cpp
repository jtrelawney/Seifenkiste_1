#include <option_class.h>

#include <iostream>

Option<int> divide(int a, int b) noexcept {
  if (b == 0) {
    return Nothing();
  } else {
    return a/b;
  }
}

int main(void) {
  Option<int> result = divide(10, 0);
  if (result) { // Check that result is not Nothing
    printf("I got a %d\n", result.unwrap());
  } else {
    printf("You divided by 0... the world blew up... Thanks\n");
  }
  
  result = divide(100,10);
  if (result) { // Check that result is not Nothing
    printf("I got a %d\n", result.unwrap());
  } else {
    printf("You divided by 0... the world blew up... Thanks\n");
  }

  return 0;
}
