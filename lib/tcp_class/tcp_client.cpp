#include <tcp_client.h>

int tcp_client::start_up(){
    tcp_class::TCP_create_socket(8192);
    tcp_class::TCP_print_status();
    tcp_class::TCP_connect("192.168.0.15");
    tcp_class::TCP_print_status();
    return 0;
}

// sends the buffer content to the server
int tcp_client::send_message(buffer_class &buffer){
    return tcp_class::TCP_send_buffer(std::move(buffer));
    //return TCP_send_message(buffer.data(),buffer.size());
}

tcp_client::tcp_client() : tcp_client_debug_level_(TCP_CLIENT_DEBUG_LEVEL) {
    if (tcp_client_debug_level_>0) std::cout << "tcp client : constructor call" << std::endl;
}

int tcp_client::shut_down(){
    std::cout << "tcp_client : shutdown call" << std::endl;
	return tcp_class::TCP_close_socket();
}

tcp_client::~tcp_client(){
    std::cout << "tcp_client : destructor call" << std::endl;
	shut_down();
}

void tcp_client::print_status(){ tcp_class::TCP_print_status(); }

// takes ownership of message
// 1. extract the header and the data into buffers
// 2. tcp send the header
// 3. receive header confirmation
// 4. tcp send the data
// 5. receive data confirmation

int tcp_client::send_message(unique_message_ptr message){

    if (tcp_client_debug_level_>1) std::cout << "\n1. create a header buffer" << std::endl;
    buffer_class header_buffer(message -> create_header_buffer());
    if (tcp_client_debug_level_>1) header_buffer.print_buffer_meta_data();
    if (tcp_client_debug_level_>1) std::cout << "\n2. create a data buffer" << std::endl;
    buffer_class data_buffer(message -> create_data_buffer() );
    if (tcp_client_debug_level_>2) data_buffer.print_buffer_meta_data();

    if (tcp_client_debug_level_>1) std::cout << "\n3. send message header" << std::endl;
    int result = tcp_class::TCP_send_buffer(std::move(header_buffer) );
    if (result<0) {
        std::cout << "tcp_client : send_message : issue sending the header = " << result << std::endl;
        return result;
    }

    if (tcp_client_debug_level_>1) std::cout << "\n4. send message data" << std::endl;
    result = tcp_class::TCP_send_buffer(std::move(data_buffer) );
    if (result<0) {
        std::cout << "tcp_client : send_message : issue sending the data = " << result << std::endl;
        return result;
    }

    std::cout << "tcp_client : successfully sent message" << std::endl;
    return result;
}


/*
int tcp_client::send_message(std::string message){
    int l = message.length();
    int result = tcp_class::TCP_send_message(message.c_str(), l);
    return result;
}
*/
