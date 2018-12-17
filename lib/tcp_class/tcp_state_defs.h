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
	accept_connection_successeful,

	// error states, when something goes wrong
	socket_already_created = 50,	// attempt to create a socket but it was set / nor reset earlier
	portnumber_undefined,           // attempt to create socket, but portnumber undefined
	error_creating_socket,			// error during tcp socket call
	socket_not_open,				// trying to close socket, but it is not open
	shutdown_socket_failed,			// error during tcp shtudown call
	close_socket_failed,			// error during tcp close call
	
	gethost_error,		            // during connect, the ip address could not be resolved to server data
	bind_call_failed,				// attempt to bind to socket failed
	listen_call_failed,				// listen call failed
	tcp_connect_error,		        // if the attempt to connect to the server fails
	buffer_too_small,
	error_creating_message_from_header_buffer,
	error_inserting_data_buffer_into_message,
	
	no_client_connection_to_close,
	accept_connection_failed,

	wrong_state_for_transition,

	// states related to shutdown
	client_shutting_down,
	tcp_close_failed = 50
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
	ok = 0,
	warning,
	error,
	shutdown_next
};

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


