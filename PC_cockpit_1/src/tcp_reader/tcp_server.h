/*
 * tcp_server.h
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#define MAX_HEAD_LENGTH 256

#include <stdio.h>
//#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
//#include <strings.h>
#include <string.h>
#include <unistd.h>

#include <common/message_class.h>

class tcp_server {

private:
    int sockfd, connectionfd, portno;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // interface routines
    int init_socket();          // init socket and listen
    int connect_to_socket();    // connect to socket and wait for message - blocking
    int send_response(int connectionfd, const char *response);  //sends a response string

    // message processing routines
    
    // reads message from connection and processes header, creates message and reads data
    // either queues a complete message or requests retransmission
    void process_message(int sock);                                         

    // from the message extract the header and initialize a message
    // for short messages it inserts the data
    // for long messages it reserves memory and the rest of the data will be fetched upon return to process message
	message_class* extract_header(const char *buffer, const int length);

    //debug routines
	void error(const char *msg);
	void print_buffer(const char *buffer, const int n);

public:

    // con / destructors
	tcp_server();
    tcp_server(int portnumber);
	~tcp_server();

    // receive messages
	int connect_and_receive();
};

#endif /* TCP_SERVER_H_ */
