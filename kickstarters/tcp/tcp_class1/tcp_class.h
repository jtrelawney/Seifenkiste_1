// this is from
// http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

// it sends a short message to the server and gets one back
// call with the server ip address and the port
// ./tcp.exe 192.168.0.15 8192


#ifndef TCP_CLASS_H
#define TCP_CLASS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//added those because of warnings
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#define TCP_DEBUG_LEVEL 2

#define TCP_MAX_BUFFER_SIZE 2048

#include <sstream>

#include <defs.h>

class tcp_class{
	
private:

	// object state
	enum tcp_state_def {tcp_initialized, socket_created, socket_error, gethost_error, gethost_successful, tcp_connect_error, tcp_connect_successful, bind_call_failed, bind_call_successful, listen_call_failed, listen_call_succesful, TCP_close_failed, TCP_close_successful}; 
	tcp_state_def tcp_state;
    int debug_level = TCP_DEBUG_LEVEL;

    int port_number;
	int socket_fd;

    // host server information
    std::string server_ip_address;
    struct hostent *host_info;             // keeps stuff like servername and alias lists
	int get_host_info(const char *server_addr);

	// host server socket information - for client to address the server
    struct sockaddr_in server_socket_info;
	// host client socket information - for server to address the client back
    struct sockaddr_in client_socket_info;



    // for server business

    // when a connection is accepted, the server continues to listen to the old fd and a new fd is created for a child process, which is used to handle the communication for that connection
    int accepted_connection_fd;

    // not yet working, is supposed to read 0 length and retrieve status
    int check_buffer();

	// debug - print n bytes of the given buffer
	void print_n(char *data, int n);
	

protected:
	// process the error state and create the appropriate error message
	void process_error();
	// output the error message
	void error(const char *msg);

public:
    // con + de - structors, other management
	tcp_class();
    void print_status(int type=0);
	~tcp_class();

    // both client and server
    int TCP_create_socket(int portno);
    int TCP_close_socket();
    //int TCP_close();

    // client business
    int TCP_connect(std::string server_ip);
    int TCP_close_client_session_socket();
    
    //server business
    int TCP_bind();
    int TCP_listen();
    int TCP_accept_connection();

    // communication
    int TCP_send_message(const char *buffer, int buffer_length);
    int TCP_receive_message(char **header_buffer);

	// send message over TCP
	//int send_message(message_class &message);
	
};

#endif
