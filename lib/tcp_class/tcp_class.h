// this is from
// http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
//https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/

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

//added those because earlier version worked with char and strings, now we have buffers, i.e. vector<char>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h> 
//#include <sstream>


// defines what warnings / errors are printed
#define TCP_DEBUG_LEVEL 9

// used to receive short messages
#define TCP_MAX_BUFFER_SIZE 2048

// get the proper tcp header length from a central definiton
#include <common.h>
extern const int TCP_HEADER_LENGTH;

// vector<char> objects to transfer the data (instead of char*)
#include <buffer_class.h>


// basic tcp class
// implements the basic functions to establish connections, send and receive data 
// currently all blocking

class tcp_class{

public:
    // con + de - structors, other management
	tcp_class();
	~tcp_class();

    //for both client and server
    // create and close sockets
    int TCP_create_socket(const int &portno);
    int TCP_close_socket();

    // for client only
    // connect to server
    int TCP_connect(std::string server_ip);

    // for server only    
    // listen and establish connections - used for server
    int TCP_bind();
    int TCP_listen();
    int TCP_accept_connection();
    int TCP_close_client_session_socket();

    // for both client and server
    // communication - send and receive buffers (i.e. access to char*, length via vectors of char
    // int TCP_send_buffer(buffer_class buffer);
    int TCP_send_buffer(buffer_class &&buffer);
    int TCP_receive_buffer(buffer_class &message_buffer);

    // prints the current status information based on tcp_state
    void TCP_print_status();
    // prints the fd descriptor information
    void TCP_print_fd_info();

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

protected:

	// object state
	enum tcp_state_def : int {
        tcp_initialized = 0,            // set by constructor, the tcp object is initialized, (not the port, fd, connection etc)
        socket_created = 1,             // call tcp.socket() was successful, the socket is valid            
        socket_error = 2,               // wrong state to call tcp.socket() or call was not successful, the socket is invalid
        gethost_error = 3,              // during connect, the ip address could not be resolved to server data
        gethost_successful = 4,         // the ip address was successfully resolved, (this is currently immediately followed by connect, so not a used state)
        tcp_connect_error = 5,          // if the attempt to connect to the server fails
        tcp_connect_successful = 6,     // if the attempt to connect to the server is sucessful
        bind_call_failed,
        bind_call_successful,
        listen_call_failed,
        listen_call_succesful,
        TCP_close_failed,
        TCP_close_successful
    };

	error_class error_state_;
	tcp_state_def tcp_state_;
	void reset_error_state(){ error_state_.reset_error_state(); };
	void set_error_state(const bool status, const std::string &message){ error_state_.set_error_state(status, message); };

    // not yet working, is supposed to read 0 length and retrieve status
    //int check_buffer(); code see bottom of this file
	// old, now use the buffer , used for debug - print n bytes of the given buffer
	//void print_n1(char *data, int n);

private:

    int port_number_;
	int socket_fd_;

    // host server information
    std::string server_ip_address_;
    struct hostent *host_info;             // keeps stuff like servername and alias lists
	int get_host_info(const char *server_addr);

	// host server socket information - for client to address the server
    struct sockaddr_in server_socket_info;
	// host client socket information - for server to address the client back
    struct sockaddr_in client_socket_info;

    // for server business

    // when a connection is accepted, the server continues to listen to the old fd and a new fd is created for a 
    // child process, which is used to handle the communication for that connection
    int accepted_connection_fd;

    // determines what messages are printed or suppressed during debuging
    int tcp_debug_level_;
};

#endif

/*
int tcp_class::check_buffer(){

    char test_buffer = {0};
    int result = read(accepted_connection_fd, &test_buffer, 0);

    if (result == 0){
        if (tcp_debug_level_>2) std::cout << "tcp class : test message buffer == 0 , is this EOF" << std::endl;
        return 0;
    }

    if (result < 0){
        std::cout << "tcp class : test message buffer < 0 , what is the error" << std::endl;
        error("error testing message buffer");
        return result;
    }

    return 1;
}


void tcp_class::print_n1(char *data, int n)
{
   for (int i=0; i<n; i++){
		fprintf(stderr, "%c   %i\n",data[i], (int)data[i]);
	}
   fprintf(stderr, "\n");
}

// read the next header message from TCP
// may encounter issues with partiallly transmitted headers, i.e. when the TCP buffer only contains part of the header and the rest is being transmitted
int tcp_class::TCP_receive_message1(char **header_buffer){

    //int check = check_buffer();
    //if (check == 0){
    //    if (tcp_debug_level_>2) std::cout << "tcp class : test buffer - returned = 0 , is this EOF" << std::endl;
    //    return 0;
    //}

    // read header from buffer and validate its length
    bzero(*header_buffer, TCP_MAX_BUFFER_SIZE);
    int header_length = read(accepted_connection_fd, *header_buffer, TCP_MAX_BUFFER_SIZE);
    header_buffer[header_length]=0;

    if (header_length == 0){
        std::cout << "tcp class : receive message - read returned length == 0 , is this EOF" << std::endl;
        return 0;
    }

    if (header_length < 0){
        if (tcp_debug_level_ > 1) std::cout << "tcp class : receive message - message length < 0, some error" << std::endl;
        perror("error receiving tcp");


        
        return header_length;
    } else {
        if (tcp_debug_level_ > 1) std::cout << "tcp class : received message - length = " << header_length << std::endl;
        //error("ERROR reading message header from connection");
    }

    return header_length+1;
}

int tcp_class::TCP_send_message(const char *buffer, int buffer_length){
	
    //printf("TCP - transmitting buffer, length = %lu\n", buffer_length);
    std::cout << "tcp_class : TCP - transmitting buffer, length = " << buffer_length << std::endl;

	if (tcp_state!=tcp_connect_successful) {
		std::cout << "TCP socket not ready for a connection" << std::endl;
		return -1;
	}
	
    std::cout << "connection to " << server_ip_address << " on port " << port_number << " is ready" << std::endl;
    
    int n = write(socket_fd,buffer,buffer_length);
    if (n < 0) 
         error("ERROR writing buffer to socket");
         
    return n;
}


*/
