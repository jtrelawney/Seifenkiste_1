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
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#include <message_class.h>
#include <message_queue.h>

#include <display.h>
extern opencv_display display_object;

//#include "opencv2/opencv.hpp"
//using namespace cv;

#define TCP_HEADER_LENGTH 37

class tcp_server {

private:
    int sockfd, connectionfd, portno;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // interface routines
    int init_socket();          // init socket and listen
    int connect_to_socket();    // connect to socket and wait for message - blocking
    int send_response(int connectionfd, const char *response, int length);  //sends a response string

    // message processing routines
    
    // reads message from connection and processes header, creates message and reads data
    // either queues a complete message or requests retransmission
    void process_messages(int sock);

    // processes a header message and returns an initialized message object
    message_class process_header_message(std::string header_message);
    ptr_message process_header_message1(std::string header_message);                                        

    //debug routines
	void error(const char *msg);
	void print_buffer(const char *buffer, const int n);

    int read_TCP_header(char **header_buffer);
    int read_TCP_data(char **data_buffer, unsigned int data_length);

public:

    // con / destructors
	tcp_server();
    tcp_server(int portnumber);
	~tcp_server();

    // receive messages
	int connect_and_receive();
};

#endif /* TCP_SERVER_H_ */
