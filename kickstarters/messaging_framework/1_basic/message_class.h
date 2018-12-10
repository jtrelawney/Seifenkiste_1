//#include <iostream>       // std::cout

#ifndef COMMON_MESSAGE_CLASS_H_
#define COMMON_MESSAGE_CLASS_H_

#include <iostream>       // std::cout




class message_class{

private:
    unsigned int message_id;
    int data;
    message_class();


public:
    message_class(int d);

    void set_id(unsigned int this_id);
    unsigned int get_id();

    void set_data(unsigned int d);
    unsigned int get_data();

    void print_meta_data();

    ~message_class();
};


#endif /* COMMON_MESSAGE_CLASS_H_ */
