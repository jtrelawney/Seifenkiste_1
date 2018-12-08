#include <tcp_server.h>

int tcp_server::start_up(){
    tcp_class::TCP_create_socket(8192);
    if (tcp_server_debug_level_ > 0) tcp_class::TCP_print_status();
    tcp_class::TCP_bind();
    if (tcp_server_debug_level_ > 0) tcp_class::TCP_print_status();
    tcp_class::TCP_listen();
    if (tcp_server_debug_level_ > 0) tcp_class::TCP_print_status();
    return 0;
}

int tcp_server::shut_down(){
    int result1 = tcp_class::TCP_close_client_session_socket();
    int result2 = tcp_class::TCP_close_socket();
    return std::min(result1,result2);
}

// process the incoming messages
// assume endless cycle, but can implement a break when the client signals the end of the communication
// process one complete message : a header and the data
void tcp_server::process_messages(){
    
    // keep reading messages, which should be alternating between a header message and a data message
    //while(G_END_FLAG.read_flag() == false ){
    bool the_end = false;
    std::cout << "\n\ntcp_server : process messages : check the end criteria for while" << std::endl;
    while (!(the_end)){
    //while(true){

        if (tcp_server_debug_level_ > 0) std::cout << "\n\ntcp_server : process messages : starting the message receiver loop" << std::endl;

        // 1. read header message from TCP
        buffer_class header_buffer(TCP_HEADER_LENGTH);
        int result = tcp_class::TCP_receive_buffer(header_buffer);
        if (result < 0) {
            if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process messages : error processing header message" << std::endl;
            perror("quitting receiver loop with error in header message length");
        }

        if (result > 0) {
            if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : process messages : header received, size = " << header_buffer.get_data_size() << std::endl;
        }

        if (result == 0) {
            if (tcp_server_debug_level_ > 0) printf("tcp_server : process messages : received 0 length header message - is the client shutting down ???\n");
            break;
        }
    
        if (tcp_server_debug_level_ > 2) header_buffer.print_buffer_content();

        // 2. header ok, initialize the message, find how long the data message will be
        if (tcp_server_debug_level_ > 2) std::cout << "tcp_server : process messages : initializing message object" << std::endl;
        unique_message_ptr message = unique_message_ptr ( new message_class(header_buffer) );
        if (tcp_server_debug_level_ > 2) message -> print_meta_data();

        //unsigned int message_data_size = message->get_data_size();
        //buffer_class data_buffer(message_data_size);
        buffer_class data_buffer = message -> get_properly_sized_empty_data_buffer_according_to_header_data();
        if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : process messages : createing data buffer" << std::endl;
        if (tcp_server_debug_level_ > 1) data_buffer.print_buffer_meta_data();

        if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : process messages : reading data message" << std::endl;
        result = tcp_class::TCP_receive_buffer(data_buffer);
        if (result < 0) {
            if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process messages : error processing data message" << std::endl;
            perror("quitting receiver loop with error in data message length");
        }

        if (result > 0) {
            if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : process messages : data received, size = " << data_buffer.get_data_size() << std::endl;
        }

        if (result == 0) {
            if (tcp_server_debug_level_ > 0) printf("tcp_server : process messages : received 0 length data message - is the client shutting down ???\n");
            break;
        }    
        if (tcp_server_debug_level_ > 2) data_buffer.print_buffer_content();

        result = message->insert_data_buffer(std::move(data_buffer));
        if (tcp_server_debug_level_ > 2) message -> print_meta_data();


        if (my_queue_ != nullptr) my_queue_ -> enqueue(std::move(message) );
        else if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process messages : no queue access, can't file the message" << std::endl;


        if ( check_termination_call_back_predicate != nullptr){
            bool test = check_termination_call_back_predicate();
            the_end = test;
            if (test == false)
                std::cout << "tcp_server : process messages : not yet at the end" << std::endl;
            else
                std::cout << "tcp_server : process messages : now at the end" << std::endl;
        }
    }

    if (tcp_server_debug_level_ > 0)
        std::cout << "tcp_server : process messages : end processing messsage -> endflag has been set or connection has been terminated!" << std::endl;
}

void tcp_server::set_termination_predicate(std::function<bool ()> pred){
    if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : setting termination predicate" << std::endl;
    check_termination_call_back_predicate = pred;
}

// this routine accepts a connection on the listening port (i.e. gets a filedescriptor) and then forks a child 
// the parent closes the connection and returns to continue the calling thread
// the child processes the message and then terminates
// this call is blocking until a connection is requested (i.e. a message is being sent by the client) but then returns right away 
int tcp_server::connect_and_receive(){

    if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : connect_and_receive : blocking to wait for incoming connection ..." << std::endl;
    int result = tcp_class::TCP_accept_connection();
    if (result < 0 ) {
        if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : connect_and_receive : error accepting connection" << std::endl;
        return result;
    }

     if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : connection accepted" << std::endl;

    // communicate until end condition is fullfilled
    process_messages();

    // shutdown the connection and close the fd
    if (tcp_server_debug_level_ > 1) {
        std::cout << "tcp server : closing the client connection socket, current fd = " << std::endl;
        tcp_class::TCP_print_fd_info();
        tcp_class::TCP_close_client_session_socket();
        tcp_class::TCP_close_socket();
    }
	return 0;
}

tcp_server::tcp_server():tcp_class(), tcp_server_debug_level_(TCP_SERVER_DEBUG_LEVEL), my_queue_(nullptr) {
    if (tcp_server_debug_level_>0) std::cout << "tcp server : constructor call" << std::endl;
}

void tcp_server::set_debug_level(const int level){tcp_server_debug_level_=level;}
void tcp_server::set_queue_ptr(message_queue_class *ptr) { my_queue_ =  ptr; }

tcp_server::~tcp_server() {
    if (tcp_server_debug_level_>0) std::cout << "tcp_server: destructor call" << std::endl;
    shut_down();
}
