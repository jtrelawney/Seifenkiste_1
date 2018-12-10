#include <iostream>       // std::cout
#include <tcp_class.h>

tcp_class::tcp_class(int p){ port = p; message_counter = 0; connect(); printf("tcp_class : Contructor call\n"); }
void tcp_class::connect(){ printf("tcp_class : connecting to port %i\n",port); }
message_class tcp_class::receive_message() { int r = rand() % 100; printf("tcp_class : received %i\n",r); message_class m(r); m.set_id(message_counter); m.print_meta_data(); message_counter++; return m;}
void tcp_class::set_frequency(int mps) { printf("tcp_class : set f=%i\n",mps); }
tcp_class::~tcp_class(){ printf("tcp_class : Destructor call\n"); }

