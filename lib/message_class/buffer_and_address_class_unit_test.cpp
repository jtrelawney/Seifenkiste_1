#include <buffer_class.h>
#include <address_class.h>

void address_class_unit_test(){

    address_class::platform_type_def platform;
    address_class::sensor_type_def sensor;
    address_class::process_type_def process;

    platform = address_class::platform_type_def::pc;
    sensor = address_class::sensor_type_def::undefined_sensor;
    process = address_class::process_type_def::cockpit;
    address_class recipient_addr(platform,sensor,process);

    platform = address_class::platform_type_def::rpi;
    sensor = address_class::sensor_type_def::camera1;
    process = address_class::process_type_def::undefined;
    address_class sensor_addr(platform,sensor,process);

    std::cout << "unit test address class" << std::endl;
    std::cout << "test recipient  = " << recipient_addr << std::endl;
    std::cout << "test sensor = " << sensor_addr << std::endl;    
}

void buffer_class_unit_test(){

    std::cout << "buffer class unit tests" << std::endl;
    std::cout << "1. tests the insert and extract functions on a vector buffer" << std::endl;

    buffer_class test(10);

    std::cout << "inserting test at the first 6 positions" << std::endl;
    test.insert(0,"test!!");
    test.print_buffer_meta_data();

    std::string e1;
    test.extract(0,6,e1);
    std::cout << "extracting the first 6  positions = " << e1 << std::endl;

    int valin(-128),result(0);
    result = test.insert(6,valin);
    std::cout << "result = " << result << " , inserting -128 at pos 7 (after test string)" << std::endl;
    test.print_buffer_meta_data();

    int valout(0);
    result = test.extract(6,valout);
    std::cout << "result = " << result << " , extracting the int at pos 7 = " << valout << std::endl;

    unsigned int valin2(128);
    result = test.insert(6,valin2);
    std::cout << "result = " << result << " , inserting 128 at pos 7 (after test string)" << std::endl;
    test.print_buffer_meta_data();

    unsigned int valout2(0);
    result = test.extract(6,valout2);
    std::cout << "result = " << result << " , extracting the int at pos 7 = " << valout2 << std::endl;

    //cv::Mat t2 = cv::Mat(3,3, CV_8UC3, cv::Scalar(0,128,255) );
    //std::unique_ptr<cv::Mat> te1 = std::unique_ptr<cv::Mat> ( new cv::Mat(3,3, CV_8UC3, cv::Scalar(0,128,255) ) );
    //std::unique_ptr<cv::Mat> te = std::unique_ptr<cv::Mat> (new (cv::Mat));
    std::cout << "\n\n2. creates a test mat with 27 elems and makes a buffer from it" << std::endl;
    std::unique_ptr<cv::Mat> data_mat = std::unique_ptr<cv::Mat> ( new cv::Mat(3,3, CV_8UC3, cv::Scalar(0,128,255) ) );
    buffer_class test_data_buffer(std::move(data_mat));
    std::cout << "and the result is " << std::endl;
    test_data_buffer.print_buffer_meta_data();

    std::cout << "\n\n2. creates a test mat with a nullptr content and tries to create a buffer from it" << std::endl;
    std::cout << "\n\ntry to create a buffer from a nullptr" << std::endl;
    std::unique_ptr<cv::Mat> null_data_mat = std::unique_ptr<cv::Mat> ( nullptr );
    buffer_class test_null_data_buffer(std::move(null_data_mat));
    std::cout << "and the result is " << std::endl;
    test_null_data_buffer.print_buffer_meta_data();
}

