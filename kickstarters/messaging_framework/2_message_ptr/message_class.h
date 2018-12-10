//#include <iostream>       // std::cout

#ifndef COMMON_MESSAGE_CLASS_H_
#define COMMON_MESSAGE_CLASS_H_

#include <iostream>       // std::cout
#include <memory>       // ptr

#include <cstring>      //memcpy

class message_class;

typedef std::unique_ptr<message_class> unique_message_ptr;

int constructor = 0, destructor = 0;

class message_class{

private:
    unsigned int message_id;
    int data;
    char *buffer;
    message_class();
    
    void allocate_buffer();

public:
    message_class(unsigned int this_id);

    void set_id(unsigned int this_id);
    unsigned int get_id();

    void set_data(int d);
    int get_data();

    void print_meta_data();

    ~message_class();
};


#endif /* COMMON_MESSAGE_CLASS_H_ */
