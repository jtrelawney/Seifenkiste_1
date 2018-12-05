#include <iostream>       // std::cout

#include <string.h>
#include <vector>

#include <common.h>       //  basoc stuff for all

int main ()
{
    std::cout << "\n\nunit test for various common functions \n";
    time_format t1 = get_time_stamp();
    std::cout << "timestamp = " << t1 << "\n";

    cv::Mat test_mat(20,50,CV_8U);
    cvMat_params_class test_params(test_mat);
    test_params.print_cvMat_params();

    test_mat = cv::Mat(100,30,CV_64FC3);
    test_params = cvMat_params_class(test_mat);
    test_params.print_cvMat_params();

}
