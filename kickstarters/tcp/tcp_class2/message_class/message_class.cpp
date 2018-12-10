#include "message_class.h"


//message_class::message_class(unsigned int mid):message_id(mid), header(new buffer_class(TCP_HEADER_LENGTH) ), debug_level(MESSAGE_DEBUG_LEVEL){
message_class::message_class(unsigned int mid):message_id(mid), header(TCP_HEADER_LENGTH), data(0), debug_level(MESSAGE_DEBUG_LEVEL){
    std::cout << "very basic test message class constructor" <<  std::endl;
    std::cout << "message id =  " <<  message_id << std::endl;

    unsigned int test_data = (unsigned int)(random() % 1024);
    int data_size = sizeof(test_data);
    data_length = test_data * data_size;
    std::cout << "test data = " <<  test_data << std::endl;



    std::cout << "creating header" << std::endl;
    header.insert(0,"con");
    header.insert(3,message_id);
    header.insert(7,data_length);
    header.insert(11,"test params");
    header.insert(31,"end");

    std::cout << "creating data, ie. transmit value " <<  test_data << " times " << test_data << " (which is of size " << data_length << ")" << std::endl;
    // often at this point a pointer to the data will be available (created by the sensor) and a char* ptr has to be created for the buffer to use it with TCP
    std::vector<unsigned int> mydata(test_data,test_data);
    //glorious , this will certainly cause grief
    data = (void *) &mydata;
    //std::vector<unsigned int> (test_data,test_data);


    /*
    char *dp = data.data();
    int v = 69;
    int *vptr = &v;

    std::copy( dp, dp + data_size, vptr);
    */

}  

//message_class::message_class(buffer_class &header):data(nullptr), debug_level(MESSAGE_DEBUG_LEVEL){
message_class::message_class(buffer_class &new_header):header(std::move(new_header)){
    std::cout << "very basic test message class constructor from buffer" <<  std::endl;

    unsigned int t1 = header.extract_ui(3);
    std::cout << "extracted message id = " << t1 << std::endl;


    unsigned int t2 = header.extract_ui(7);
    std::cout << "extracted data length = " << t2 << std::endl;

    
    //std::cout << "test =  " <<  test << std::endl;

}

void message_class::print(){
    std::cout << std::endl << std::endl << "message content" << std::endl;
    std::cout << "message id = " << message_id << std::endl;
    std::cout << "header     = " << header << std::endl;
    header.print();
    std::cout << std::endl;
}

std::ostream& message_class::myostream(std::ostream& out) const {

    /*
    buffer_type::const_iterator start = buffer.buffer.begin();
    buffer_type::const_iterator ende = buffer.buffer.end();
    for (auto i=start; i<ende; ++i){
        out << *i;
    }
    //buffer.print();
    */
    out << std::endl << std::endl << "message content" << std::endl;
    out << "message id = " << message_id << std::endl;
    out << "header     = " << header << std::endl;
    out << std::endl;
    return out;  

}



message_class::~message_class(){}
