
#ifndef TCP_ERROR_DEF_H
#define TCP_ERROR_DEF_H

// object state
enum tcp_state_def : int {
	tcp_ok = 0,
	tcp_initialized = 1,            // set by constructor, the tcp object is initialized, (not the port, fd, connection etc)
	socket_created = 2,             // call tcp.socket() was successful, the socket is valid            
	socket_error = 3,               // wrong state to call tcp.socket() or call was not successful, the socket is invalid
	gethost_error = 4,              // during connect, the ip address could not be resolved to server data
	gethost_successful = 5,         // the ip address was successfully resolved, (this is currently immediately followed by connect, so not a used state)
	tcp_connect_error = 6,          // if the attempt to connect to the server fails
	tcp_connect_successful = 7,     // if the attempt to connect to the server is sucessful
	bind_call_failed,
	bind_call_successful,
	listen_call_failed,
	listen_call_succesful,
	TCP_close_failed,
	TCP_close_successful
};

enum class error_category_def : int {
	ok = 0,
	warning,
	error,
	shutdown
};

#endif
