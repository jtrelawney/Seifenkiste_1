#ifndef TCP_STATE_DEF_H
#define TCP_STATE_DEF_H

// object state
enum tcp_state_def : int {
	state_ok = 0,
	
	// state machine - the proper states the tcp object can take
	tcp_initialized = 10,           // set by constructor, the tcp object is initialized, (not the port, fd, connection etc)
	socket_created,					// call tcp.socket() was successful, the socket is valid            
	gethost_successful,		        // the ip address was successfully resolved, (this is currently immediately followed by connect, so not a used state)
	bind_call_successful,			// the attempt to bind to the socket was succesful
	listen_call_succesful,			// listen call successful, ready to connect
	tcp_connect_successful,			// if the attempt to connect to the server is sucessful
	tcp_close_successful
};


//enum receiver_states_ { read_header, read_data, data_complete, wrong_state, error_reading_tcp, client_shutting_down, error_initializing_message, error_inserting_data_into_message, fucked_up, totally_fucked_up };


// states related to transactions / operation
enum tcp_error_def : int {
	no_error = 0,

	// success states
	tcp_read_successful = 10,
	tcp_write_successful,
	tcp_write_failed,
	write_failed_no_connection,
	tcp_read_failed,
	header_message_received,
	data_message_received,
	tcp_accept_connection_successeful,

	// error states, when something goes wrong
	invalid_port_number,			// port number not in range 8192 - double it
	socket_already_created = 50,	// attempt to create a socket but it was set / nor reset earlier
	portnumber_undefined,           // attempt to create socket, but portnumber undefined
	tcp_error_creating_socket,			// error during tcp socket call
	socket_not_open,				// trying to close socket, but it is not open
	tcp_shutdown_socket_failed,			// error during tcp shtudown call
	tcp_close_socket_failed,			// error during tcp close call
	
	tcp_gethost_error,		            // during connect, the ip address could not be resolved to server data
	tcp_bind_call_failed,				// attempt to bind to socket failed
	tcp_listen_call_failed,				// listen call failed
	tcp_connect_error,		        // if the attempt to connect to the server fails
	buffer_too_small,
	error_creating_message_from_header_buffer,
	error_inserting_data_buffer_into_message,
	
	no_client_connection_to_close,
	tcp_accept_connection_failed,

	wrong_state_for_transition,

	// states related to shutdown
	client_shutting_down,
	close_failed = 50
};

/*
    class error_class {
    private:
        error_class(){reset_error_state();};
    public:
        bool error_status_;
        std::string error_message_;
        error_class(bool status, std::string message) : error_status_(status), error_message_(message){};
        ~error_class(){};
        void set_error_state(const bool status, const std::string &message){ error_message_ = message; error_status_ = status;};
        void reset_error_state(){ error_message_ = "ok"; error_status_ = false; };
        bool is_error(){ return error_status_;};
        bool read_error_state(std::string &message){ message = error_message_; return error_status_;};
        std::string read_error_message(){ return error_message_;};
    };
    
    std::string read_error_message() { return error_state_.read_error_message(); };
    * 	void reset_error_state(){ error_state_.reset_error_state(); };
	void set_error_state(const bool status, const std::string &message){ error_state_.set_error_state(status, message); };


*/


enum error_category_def : int {
	ok = 0,				// operation succeeded
	warning,			// tcp operation succeeded, but the functional result is wrong
	error,				// tcp operation failed
	shutdown_next		// shutdown condition noticed
};


void print_error(tcp_error_def error_code){
	
	std::cout << "error code   =  " << (int) error_code << std::endl;
	switch( (int)error_code){

// ------------------------- success states
		case no_error:
			std::cout << "tcp error : no error" << std::endl;
			break;
		case tcp_read_successful:
			std::cout << "tcp error : tcp read successful" << std::endl;
			break;
		case tcp_write_successful:
			std::cout << "tcp error : tcp write successful" << std::endl;
			break;			
		case header_message_received:
			std::cout << "tcp error : header message successfully received" << std::endl;
			break;
		case data_message_received:
			std::cout << "tcp error : data_message_received" << std::endl;
			break;
		case tcp_accept_connection_successeful:
			std::cout << "tcp error : tcp_accept_connection_successeful" << std::endl;
			break;


// ------------------------- operational errors

		case tcp_write_failed:
			std::cout << "tcp error : tcp_write_failed" << std::endl;
			break;	
		case tcp_read_failed:
			std::cout << "tcp error : tcp_read_failed" << std::endl;
			break;
		case tcp_error_creating_socket:
			std::cout << "tcp error : tcp_error_creating_socket" << std::endl;
			break;
		case tcp_shutdown_socket_failed:
			std::cout << "tcp error : tcp_shutdown_socket_failed" << std::endl;
			break;
		case tcp_close_socket_failed:
			std::cout << "tcp error : tcp_close_socket_failed" << std::endl;
			break;
		case tcp_gethost_error:
			std::cout << "tcp error : tcp_gethost_error" << std::endl;
			break;
		case tcp_bind_call_failed:
			std::cout << "tcp error : tcp_bind_call_failed" << std::endl;
			break;
		case tcp_listen_call_failed:
			std::cout << "tcp error : tcp_listen_call_failed" << std::endl;
			break;
		case tcp_connect_error:
			std::cout << "tcp error : tcp_connect_error" << std::endl;
			break;
		case tcp_accept_connection_failed:
			std::cout << "tcp error : tcp_accept_connection_failed" << std::endl;
			break;


// ------------------------- error related to state, input, etc, not related to tcp calls

		case invalid_port_number:
			std::cout << "tcp error : invalid_port_number" << std::endl;
			break;
		case close_failed:
			std::cout << "tcp error : close_failed" << std::endl;
			break;
		case portnumber_undefined:
			std::cout << "tcp error : portnumber_undefined" << std::endl;
			break;	
		case buffer_too_small:
			std::cout << "tcp error : buffer_too_small" << std::endl;
			break;	
		case error_inserting_data_buffer_into_message:
			std::cout << "tcp error : error_inserting_data_buffer_into_message" << std::endl;
			break;	
		case error_creating_message_from_header_buffer:
			std::cout << "tcp error : error_creating_message_from_header_buffer: " << std::endl;
			break;
		case write_failed_no_connection:
			std::cout << "tcp error : write_failed_no_connection: " << std::endl;
			break;
		case socket_not_open:
			std::cout << "tcp error : socket_not_open: " << std::endl;
			break;
		case no_client_connection_to_close:
			std::cout << "tcp error : no_client_connection_to_close: " << std::endl;
			break;
		case wrong_state_for_transition:
			std::cout << "tcp error : wrong_state_for_transition: " << std::endl;
			break;
		
// ------------------------- shutdown mode

		case client_shutting_down:
			std::cout << "tcp error : client_shutting_down" << std::endl;
			break;


		default:
			std::cout << "tcp error : undefined message for code " << (int) error_code << std::endl;
		}
		
}

/*

			
		case tcp_initialized:
  			std::cout << "tcp_class : status = TCP object initialized" << std::endl;
            break;
        case socket_created:
  			std::cout << "tcp_class : status = TCP socket created, port = " << port_number_ << std::endl;
			break;
		case error_creating_socket:
			std::cout << "tcp_class : status = error creating socket" << std::endl;
			break;
		case gethost_successful:
			std::cout << "tcp_class : status = host information resolved" << std::endl;
			break;
		case gethost_error:
			std::cout << "tcp_class : status = failed to resolve host information" << std::endl;
			break;
		case tcp_connect_successful:
			std::cout << "tcp_class : status = connection established to remote host" << std::endl;
			break;
		case tcp_connect_error:
			std::cout << "tcp_class : status = error connecting to remote host" << std::endl;
			break;
		case bind_call_failed:
			std::cout << "tcp_class : status = bind call failed" << std::endl;
			break;
		case bind_call_successful:
			std::cout << "tcp_class : status = bind call succesful" << std::endl;
			break;
		case listen_call_failed:
			std::cout << "tcp_class : status = listen call failed" << std::endl;
			break;
		case listen_call_succesful:
			std::cout << "tcp_class : status = listen call succesful" << std::endl;
			break;
		default:
			std::cout << "tcp state is ok" << std::endl;
	}
	*/


/*
class tcp_error_state_class{
private:
	tcp_state_def current_state_;
	tcp_error_category current_category_;
	
public:
	tcp_state_def get_tcp_state() { return current_state_; }; 
	tcp_error_category get_error_category() { return current_category_; }; 
};
*/

#endif

