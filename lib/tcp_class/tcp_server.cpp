#include <tcp_server.h>

int tcp_server::start_up(){
	
    tcp_error_def result = tcp_class::TCP_create_socket(8192);
    if ( result != no_error ) {
		if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : startup : error creating socket" << std::endl;
		return result;    
	}	
    if (tcp_server_debug_level_ > 0) tcp_class::TCP_print_status();

    tcp_class::TCP_bind();
    if ( result != no_error ) {
		if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : startup : error creating socket" << std::endl;
		return result;    
	}	
    if (tcp_server_debug_level_ > 0) tcp_class::TCP_print_status();
    tcp_class::TCP_listen();
    if ( result != no_error ) {
		if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : startup : error creating socket" << std::endl;
		return result;    
	}
    if (tcp_server_debug_level_ > 0) tcp_class::TCP_print_status();

    return 0;
}

int tcp_server::shut_down(){
    if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : shutdown : closing client session socket" << std::endl;
    int result1 = tcp_class::TCP_close_client_session_socket();
    if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : shutdown : closing socket" << std::endl;
    int result2 = tcp_class::TCP_close_socket();
    int result = std::min(result1,result2);
    if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : shutdown complete , result = " << result << std::endl;
    return result;
}

//tcp_server::receiver_states_ tcp_server::receive_header_message(const receiver_states_ &current_state, unique_message_ptr &message)
bool tcp_server::receive_header_message(unique_message_ptr &message)
{
	// read header message from TCP into buffer
    buffer_class header_buffer(TCP_HEADER_LENGTH);
    tcp_error_def result = tcp_class::TCP_receive_buffer(header_buffer);
    //Return_type<buffer_class> result = tcp_class::TCP_receive_buffer(std::move(header_buffer));
    
    // in case of issues nothing to do here
    if (result != tcp_read_successful ) {
		if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_header_message : issues reading header message " << std::endl;
		return false;
	}
	/*
	if (result) {
		std::cout << "honkey dorey" << std::endl;
	} else {
		std::cout << "error" << std::endl;
		header_buffer = result.unwrap();
	}
	*/
	
    // no issues -> process header
    if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_header_message : header received, size = " << header_buffer.get_data_size() << std::endl;
    if (tcp_server_debug_level_ > 3) header_buffer.print_buffer_content();
    if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_header_message : initializing message object" << std::endl;
    message = unique_message_ptr ( new message_class(header_buffer) );

	std::cout << "tcp_server : message initialized , addr = " << (void *) &message << std::endl;
	if (message == nullptr) std::cout << "fuck" << std::endl;
	message_class::message_state_def s = message->get_state();
	std::cout << "tcp_server : found out state of message " << std::endl;
	
    //if (message->get_state() != message_class::message_state_def::initialized) {
    if ( s != message_class::message_state_def::initialized) {
        if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_header_message : error initializing message with header buffer" << std::endl;
        // dont do that with a nullptr: if (tcp_server_debug_level_ > 1) message -> print_meta_data();
        //set_error_state(true, "tcp_server : receive_header_message : error initializing message with header buffer");
        error_state_ = error_creating_message_from_header_buffer;
		return false;
    } 

    // all fine, return the number of bytes received
    if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_header_message : message initialized" << std::endl;
    if (tcp_server_debug_level_ > 3) message -> print_meta_data();
	error_state_ = header_message_received;
	error_category_ = ok;
    return true;
}

// after the header message has been successfully received , the data will arrive next
// the size of the data message has been communicated in the message header
// 1. get a properly sized buffer (provided by the message, it knows the correct buffer size)
// 2. read the number of bytes from tcp, if that works return true since all worked fine
// 3. otherwise set the detailed error state and return false
// error cases:
// - 
bool tcp_server::receive_data_message(unique_message_ptr &message){

    // prepare the data buffer
    if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_data_message : creating data buffer" << std::endl;
    buffer_class data_buffer = message -> get_properly_sized_empty_data_buffer_according_to_header_data();
    if (tcp_server_debug_level_ > 3) data_buffer.print_buffer_meta_data();
    if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_data_message : reading data message from tcp" << std::endl;
    
    // read the data message, any issues the call failed
    tcp_error_def result = tcp_class::TCP_receive_buffer(data_buffer);
    //if ( ! ( ( result == no_error ) || ( result == tcp_read_successful) ) ) {
    if ( ! ( ( result == no_error ) || ( result == tcp_read_successful) ) ) {
    //if  ( ! ( result == tcp_read_successful) ) {
		if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_data_message : error receiving the data message , errorcode = " << error_state_ << std::endl;
		return false;
	}	
	
    // no issues -> process data buffer
    if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_data_message : data received, size = " << data_buffer.get_data_size() << std::endl;
    if (tcp_server_debug_level_ > 3) data_buffer.print_buffer_content();

    int result1 = message->insert_data_buffer(std::move(data_buffer));
    if (tcp_server_debug_level_ > 2) message -> print_meta_data();
    
    // result 0 : everything is ok
    if (result1 < 0) {
        if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_data_message : error inserting data buffer into message" << std::endl;
        //if (tcp_server_debug_level_ > 1) message -> print_meta_data();
		error_state_ = error_inserting_data_buffer_into_message;
		//set_error_state(true,"tcp_server : receive_data_message : error inserting data buffer into message");
		return false;
    } 
    if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : receive_data_message : message completed" << std::endl;
	if (tcp_server_debug_level_ > 3) message -> print_meta_data();
	return true;
}

// return codes
// -1 : server not in listening status
// -2 : requested buffer data length too small (<0)
// -3 : message length received < 0 : tcp error
// -4 : message length received = 0 : has client terminated the connection?
// -10 : error creating message from buffer
// -11 : message_queue_ptr is null
// -12 : error recieved from message queue
// > 0: number of bytes received

/*
void tcp_server::print_error_state(const int &error){
    // process errors
    switch(result) {
        case -1:
            break;
        case -1:
            if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : receive_header_message : error receiving header message" << std::endl;
            return error_reading_tcp;

    if (result == 0) {
        if (tcp_server_debug_level_ > 0) std::cout << ("tcp_server : receive_header_message : received 0 length header message - is the client shutting down ???\n");
        return client_shutting_down;
    }
        if (next_state != read_data) {
            if (tcp_server_debug_level_ > 0) std::cout << "\n\ntcp_server : process_messages : error initializing message from header" << std::endl;
        }

    0
        if (tcp_server_debug_level_ > 1) std::cout << "tcp_server : process_messages : message queued" << std::endl;

    -12
        if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process_messages : error queuing message" << std::endl;
    -11
    if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process_messages : no queue access, can't file the message" << std::endl;

        if (current_state != read_header) {
        if (tcp_server_debug_level_ > 0) std::cout << "\n\ntcp_server : receive_header_message : not in state 'read_header'" << std::endl;
        return -10;
    }
    * 
    *     if (result == 0) {
        if (tcp_server_debug_level_ > 0) std::cout << ("tcp_server : receive_data_message : received 0 length header message - is the client shutting down ???\n");
        return client_shutting_down;
    }    



}
*/

// process the incoming messages
// assume endless cycle, but can implement a break when the client signals the end of the communication
// process one complete message : a header and the data
void tcp_server::process_messages(){

    // voila : the message
    unique_message_ptr message = nullptr;
    
    // keep reading messages, which should be alternating between a header message and a data message
    //while(G_END_FLAG.read_flag() == false ){
    bool the_end = false;
    while (!(the_end)){

        if (tcp_server_debug_level_ > 0) std::cout << "\n\ntcp_server : process_messages : starting the message receiver loop" << std::endl;
        message = nullptr;

        // first receive the header message - only if a proper header is received then initialize a message and increase the state
        if ( receive_header_message(message) == false) {
            if (tcp_server_debug_level_ > 0) {
				std::cout << "tcp_server : process_messages : error reading header buffer from tcp , error state : " << error_state_ << "\n";
			}
        } else {

        // second receive the data message - only if a proper cvmat is received then complete the message
            if (receive_data_message(message) == false) {
                if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process_messages : error reading data buffer from tcp , error_state : " << error_state_ << "\n";
            } else {
                if (tcp_server_debug_level_ > 0) std::cout << "\n\ntcp_server : process_messages : message successfully completed" << std::endl;
        // third queue the message
                if (my_queue_ != nullptr) {
                    bool queue_result = my_queue_ -> enqueue( std::move(message) );
                    if (queue_result == false) {
                        if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process_messages : error queuing message" << "\n";
                    } else if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process_messages : success queuing message" << "\n";
                } else if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process_messages : no message queue defined, can't queue message\n"; 
            }
        }
        
        // forth check for end condition - either shutdown signaled or end predicate set
        
        // 1. shut down required?
        if (error_state_ == client_shutting_down) {
            if (tcp_server_debug_level_ > 0) std::cout << "\n\ntcp_server : process_messages : seems client is shutting down" << std::endl;
            the_end = true;
        } else {
		// 2. termination predicate
            if (tcp_server_debug_level_ > 0) std::cout << "\n\ntcp_server : process_messages : checking termination predicate\n";
			// A. call predicate
            if ( check_termination_call_back_predicate != nullptr){
                bool test = check_termination_call_back_predicate();
                the_end = test;
                if (test == false) {
                    if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process_messages : not yet at the end" << std::endl;
                } else {
                    if (tcp_server_debug_level_ > 0) std::cout << "tcp_server : process_messages : now at the end" << std::endl;
                }
            }
            // B. no predicate
			else if (tcp_server_debug_level_ > 0) std::cout << "\n\ntcp_server : process_messages : no termination predicate defined\n";

        }

		// decide wether to continue
		if ( the_end == true) {
			if (tcp_server_debug_level_ > 1) std::cout << "\n\ntcp_server : process_messages : exiting processing loop" << std::endl;
		} else {
			if (tcp_server_debug_level_ > 1) std::cout << "\n\ntcp_server : process_messages : next round" << std::endl;
		}
        
    }



    if (tcp_server_debug_level_ > 0) {
        std::cout << "tcp_server : process messages : end processing messsage -> endflag has been set or connection has been terminated!" << std::endl;
    }
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
void tcp_server::set_queue_ptr(message_queue_class *ptr) {
    my_queue_ =  ptr;
    if (tcp_server_debug_level_>0) std::cout << "tcp server : set_queue_ptr = " << (void*) my_queue_ << std::endl;
}

tcp_server::~tcp_server() {
    if (tcp_server_debug_level_>0) std::cout << "tcp_server: destructor call" << std::endl;
    shut_down();
    if (tcp_server_debug_level_>0) std::cout << "tcp_server: destructor call complete" << std::endl;
}
