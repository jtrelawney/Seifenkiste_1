#include <iostream>       // std::cout

#include <string.h>
#include <vector>

#include <common.h>       //  basoc stuff for all
#include <return_type_class.h>

Return_type<int> divide(int a, int b) noexcept {
  if (b == 0) {
    return Nothing();
  } else {
    return a/b;
  }
}

void test_return_type() {
  Return_type<int> result = divide(10, 0);
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
}

int main ()
{
    std::cout << "\n\nunit test for various common functions \n";
    time_format t1 = get_time_stamp();
    std::cout << "timestamp = " << t1 << "\n";

    std::cout << "\n\nunit test cvmat params\n";
    std::cout << "\n\nsize 20 by 50 unsigned 8bit\n";
    cv::Mat test_mat(20,50,CV_8U);
    cvMat_params_class test_params(test_mat);
    test_params.print_cvMat_params();
 

    std::cout << "\n\nsize 50 by 20 signed 16bit\n";
    test_mat = cv::Mat(50,20,CV_16S);
    test_params = cvMat_params_class(test_mat);
    test_params.print_cvMat_params();

    std::cout << "\n\nsize 100 by 10  / 2 channel 32 bit flaot\n";
    test_mat = cv::Mat(100,10,CV_32FC2);
    test_params = cvMat_params_class(test_mat);
    test_params.print_cvMat_params();

}


