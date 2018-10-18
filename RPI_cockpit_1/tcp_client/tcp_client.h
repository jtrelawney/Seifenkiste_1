// this is from
// http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

// it sends a short message to the server and gets one back
// call with the server ip address and the port
// ./tcp.exe 192.168.0.15 8192

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//added those because of warnings
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <sstream>

#include <message_class.h>

class tcp_client{
	
private:

	// object state
	enum tcp_state_def {tcp_not_initialized, socket_error, socket_fd_ready, gethost_error, tcp_ready_to_connect, connect_error, connection_ready }; 
	tcp_state_def tcp_state;
	
	int sockfd, portno;
	char *server_addr;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	// prepare socket for connection
	int create_socketfd();
	int get_hostname(char *server_addr);
	
	// connect and disconnect
	int TCP_connect();	
	int TCP_disconnect();

	// internal send command, sends a given buffer over TCP
	// considers current state in case the socket was not openend during object construction
	int send(const char *buffer,unsigned int length);

	// process the error state and create the appropriate error message
	void process_error();
	// output the error message
	void error(const char *msg);
	
	// debug - print n bytes of the given buffer
	void print_n(char *data, int n);
	
	// this should be deleted, header is created in message class
	//int create_header(char **sessioninfo, unsigned long data_length);	

public:
	tcp_client();
	~tcp_client();

	// send message over TCP
	int send_message(message_class &message);
	
	// creates a test message and sends it over TCP	    
	int send_test_image(char *image, unsigned long imagelen);
};
