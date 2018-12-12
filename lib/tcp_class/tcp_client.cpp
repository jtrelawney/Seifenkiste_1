#include <tcp_client.h>

int tcp_client::start_up(){
    tcp_class::TCP_create_socket(8192);
    tcp_class::TCP_print_status();
    tcp_class::TCP_connect("192.168.0.15");
    tcp_class::TCP_print_status();

	if (my_queue_ptr_ == nullptr) {
		if (tcp_client_debug_level_>0) std::cout << "tcp client : startup call - no message queue ptr defined, can't register" << std::endl;
	} else {
		tcp_client_process_id_ = my_queue_ptr_ -> register_as_tcp_process(tcp_client_address_);
		if (tcp_client_process_id_<0) if (tcp_client_debug_level_>0) std::cout << "tcp client : startup call - registration failed, result = " << tcp_client_process_id_ << std::endl;
	}
    
    return tcp_client_process_id_;
}

// sends the buffer content to the server
int tcp_client::send_message(buffer_class &buffer){
    return tcp_class::TCP_send_buffer(std::move(buffer));
    //return TCP_send_message(buffer.data(),buffer.size());
}

tcp_client::tcp_client() :
	tcp_client_address_( address_class(address_class::platform_type_def::rpi,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::tcp_client) ),
	tcp_client_process_id_(-1),
	my_queue_ptr_(nullptr),
	tcp_client_debug_level_(TCP_CLIENT_DEBUG_LEVEL)
{
    if (tcp_client_debug_level_>0) std::cout << "tcp client : constructor call" << std::endl;
}

int tcp_client::shut_down(){ 
    std::cout << "tcp_client : shutdown call" << std::endl;
	return tcp_class::TCP_close_socket();
}

void tcp_client::set_debug_level(const int &level){  tcp_client_debug_level_ = level; }
void tcp_client::set_queue_ptr(message_queue_class *ptr) { my_queue_ptr_ =  ptr; }

tcp_client::~tcp_client(){
    std::cout << "tcp_client : destructor call" << std::endl;
	shut_down();
}

// returns a thread which calls the main processing loop
std::thread tcp_client::operator()() {
    return std::thread(&tcp_client::keep_processing,this);
}

// the main processing loop
// sleeps on the condition variable, until signal from the message queue
void tcp_client::keep_processing(){

    std::time_t ct;
    // get lock then in loop sleep until notified
    std::cout << "tcp_client : keep processing : tcp locks its mutex" << std::endl;
    std::unique_lock<std::mutex> lock_communication_mutex(G_QUEUE_COORDINATION_VARS_OBJ.message_available_mutex[tcp_client_process_id_]);

    bool the_end = false;
    while (!(the_end)){

        std::cout << "\n\n===========================================" << std::endl;
        std::cout << "tcp_client : keep processing : release mutex and sleep" << std::endl;
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[tcp_client_process_id_].wait(lock_communication_mutex);
        // after waking up the process holds the lock
		ct = get_time();
		if (tcp_client_debug_level_>1) std::cout << "tcp_client  = " << tcp_client_process_id_ << " wakes up @ " << ct << std::endl;

        // check if message is available and if yes send it via tcp
        if ( G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[tcp_client_process_id_] == true ) {

            // fetch and process the message
            unique_message_ptr message = my_queue_ptr_->dequeue(tcp_client_address_);
            if (message!=nullptr) {
				if (tcp_client_debug_level_>1) std::cout << "tcp_client  = " << tcp_client_process_id_ << " received message, send via tcp" << std::endl;
				send_message(std::move(message));
            } else {
				if (tcp_client_debug_level_>1) std::cout << "tcp_client  = " << tcp_client_process_id_ << " received nullptr message" << std::endl;
			}

            // message processed -> set com flag
            G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[tcp_client_process_id_] = false;

        } else {
            // if woken up, but the data flag is false then check for shutdown, else it is spurious wake up
            if (my_queue_ptr_ -> get_shut_down_flag() == true) break;
            else std::cout << "tcp_client : keep_processing : wake up , no data, no shutdown -> spurious wake up?" << ct << std::endl;
        }

		/*
        if ( check_termination_call_back_predicate != nullptr){
            bool test = check_termination_call_back_predicate();
            the_end = test;
            if (test == false)
                std::cout << "tcp_server : process messages : not yet at the end" << std::endl;
            else
                std::cout << "tcp_server : process messages : now at the end" << std::endl;
        }
        */
    }

    ct = get_time();
    std::cout << "\ntcp_client pid = " << tcp_client_process_id_ << " verified the shutdown flag and is exiting @ " << ct << std::endl;

    bool result = my_queue_ptr_ -> deregister_process(tcp_client_address_);
    std::cout << "\ntcp_client deregistering from message queue , result = " << result << std::endl;
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
