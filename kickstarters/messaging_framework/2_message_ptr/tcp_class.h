//#include <iostream>       // std::cout

#ifndef TCP_CLASS_H_
#define TCP_CLASS_H_

#include <iostream>       // std::cout

#include <message_class.h>


class tcp_class{

private:
    int port;
    int mps;
    int message_counter;
    
    tcp_class();

public:

    tcp_class(int p);
    void connect();
    void set_frequency(int mps);
    unique_message_ptr receive_message();

    ~tcp_class();
};


#endif
