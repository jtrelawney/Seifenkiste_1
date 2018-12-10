/*
 * tcp_server.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#include <tcp_server.h>

int tcp_server::start_up(){
    tcp_class::TCP_create_socket(8192);
    if (debug_level > 0) tcp_class::print_status();
    tcp_class::TCP_bind();
    if (debug_level > 0) tcp_class::print_status();
    tcp_class::TCP_listen();
    if (debug_level > 0) tcp_class::print_status();
    return 0;
}

int tcp_server::shut_down(){
    int result1 = tcp_class::TCP_close_client_session_socket();
    int result2 = tcp_class::TCP_close_socket();
    return std::min(result1,result2);
}

// process the incoming messages
// assume endless cycle, but can implent a break when the client signals the end of the communication
void tcp_server::process_messages(){
    
    // asume failure
    int result=-1;

    // prepare buffer
    char buffer[TCP_MAX_BUFFER_SIZE];
    char *header_buffer = &buffer[0];
   
    // keep reading messages, which should be alternating between a header message and a data message

    int N=INT_MAX;
    //int N=5;
    while(N>0){

        if (debug_level > 2) std::cout << "\n\ntcp_server : starting the message receiver loop , receive " << N << " messages\n\n" << std::endl;

        // read message from TCP
        result = tcp_class::TCP_receive_message(&header_buffer);
        if (result < 0) {
            std::cout << "error processing header message" << std::endl;
            perror("quitting receiver loop with error in header message length");
        }

        if (result > 0) printf("\n\nreading  %s \n\n",header_buffer);

        if (result == 0) {
            printf("received 0 length message - is the client shutting down ???\n");
            break;
        }

        N--;
    }

    std::cout << "the endless TCP message processing loop exited and the child process will wind up and exit  ..." << std::endl;
}



// this routine accepts a connection on the listening port (i.e. gets a filedescriptor) and then forks a child 
// the parent closes the connection and returns to continue the calling thread
// the child processes the message and then terminates
// this call is blocking until a connection is requested (i.e. a message is being sent by the client) but then returns right away 
int tcp_server::connect_and_receive(){

	fprintf(stderr,"tcp_server : parent : blocking to wait for incoming connection ...\n");
    int result = tcp_class::TCP_accept_connection();
    if (result < 0 ) {
        std::cout << "error accapting connection" << std::endl;
        return result;
    }

    std::cout << "tcp_server : parent : connection accepted" << std::endl;

	int pid = fork();
	if (pid < 0)
		error("ERROR on fork");
	if (pid == 0)  {
        fprintf(stderr,"child here, closing socket and starting process to receive message\n");
		tcp_class::TCP_close_socket();
		process_messages();
        std::cout << "\n\ntcp server : child here, seems client done sending messages, closing the connection and exiting process" << std::endl;
        std::cout << "tcp server : childs view on the fds = " << std::endl;
        tcp_class::print_status(1);
        std::cout << "tcp server : closing the client connection socket" << std::endl;
        tcp_class::TCP_close_client_session_socket();
		exit(0);
	} else {
        //fprintf(stderr,"parent here, closing connection and returning to caller\n");
        std::cout << "\n\ntcp_server : parent : child process created, pid = " << pid << std::endl;
        std::cout << "tcp server : parent view on the fds = " << std::endl;
        tcp_class::print_status(1);
        //std::cout << "tcp_server : parent : closing parent connection and returning to caller" << std::endl;
        //tcp_class::TCP_close_client_session_socket();
	}

	return 0;
}

tcp_server::tcp_server():tcp_class() {
    if (debug_level>0) std::cout << "tcp server : constructor call" << std::endl;
}

tcp_server::~tcp_server() {
    std::cout << "tcp_server: destructor call" << std::endl;
    shut_down();
}
