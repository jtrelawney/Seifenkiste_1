#include <tcp_client.h>

int tcp_client::start_up(){
    tcp_class::TCP_create_socket(8192);
    tcp_class::print_status();
    tcp_class::TCP_connect("192.168.0.15");
    tcp_class::print_status();
    return 0;
}

// sends the buffer content to the server
int tcp_client::send_message(buffer_class &buffer){
    return TCP_send_message(buffer.data(),buffer.size());
}


tcp_client::tcp_client(){
    if (debug_level>0) std::cout << "tcp client : constructor call" << std::endl;
}

int tcp_client::shut_down(){
    std::cout << "tcp_client : shutdown call" << std::endl;
	return tcp_class::TCP_close_socket();
}

tcp_client::~tcp_client(){
    std::cout << "tcp_client : destructor call" << std::endl;
	shut_down();
}

/*
int tcp_client::send_message(std::string message){
    int l = message.length();
    int result = tcp_class::TCP_send_message(message.c_str(), l);
    return result;
}
*/
