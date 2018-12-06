#include <iostream>         // std::cout

#include "opencv2/opencv.hpp"

#include <common.h>
#include <address_class.h>
#include <buffer_class.h>
#include <message_class.h>

// create a test message, dissolve in header and data buffer
// then assemble a new message from the buffers and return to main
// compare the original with the reconstructed message 
// also verify that moving unique ptrs arround works
unique_message_ptr integrated_test()
{
    std::cout << "\n\ncalling unit test messaging class to create a test message" << std::endl;
    unique_message_ptr test_message = message_class_unit_test_create_test_message(20,50,3,100);
    
    // this is the sitation where the tcp class has taken oawnership of the message with tcp_send(message);
    std::cout << "\n1. create a header buffer" << std::endl;
    
    buffer_class header_buffer(test_message -> create_header_buffer());
    std::cout << "\nbuffer returned" << std::endl;
    header_buffer.print_buffer_meta_data();


    // header is done now create the data buffer 
    std::cout << "\n2. create a data buffer" << std::endl;
    buffer_class data_buffer(test_message -> create_data_buffer() );
    std::cout << "\nbuffer returned" << std::endl;
    data_buffer.print_buffer_meta_data();

    std::cout << "\nreleasing test message" << std::endl;
    test_message.release();


    std::cout << "\n3. from the header buffer initialize a sensor message" << std::endl;
    unique_message_ptr recovery_message = unique_message_ptr( new message_class(header_buffer) );

    if (recovery_message->get_state() == message_class::message_state_def::error_when_constructing_from_buffer) std::cout << "error constructing message from tcp buffer" << std::endl;
    else recovery_message->print_meta_data();

    std::cout << "\n4. from the data buffer complete the sensor message" << std::endl;
    recovery_message -> insert_data_buffer( std::move(data_buffer) );

    if (recovery_message->get_state() == message_class::message_state_def::error_when_constructing_from_buffer) std::cout << "error constructing message from tcp buffer" << std::endl;
    else {
        std::cout << "\nrecovered message" << std::endl;
        recovery_message->print_meta_data();
        recovery_message->print_data(20);
    }
    
    return std::move(recovery_message);
}

int main ()
{
    std::cout << "\n\n\n ==========================================================" << std::endl;
    std::cout << "\n\ncalling unit test address class" << std::endl;
    address_class_unit_test();


    std::cout << "\n\n\n ==========================================================" << std::endl;
    std::cout << "\n\ncalling unit test messaging class" << std::endl;
    buffer_class_unit_test();


    std::cout << "\n\n\n ==========================================================" << std::endl;
    unique_message_ptr test_message = integrated_test(); //message_class_unit_test_create_test_message();
    std::cout << "\n\n\n ==========================================================" << std::endl;
    std::cout << "\nthis is the reassembled message" << std::endl;
    test_message -> print_meta_data();
    test_message -> print_data(20);
    

    return 0;
}

