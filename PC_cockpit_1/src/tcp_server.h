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
    int sockfd, connectionfd, portno, pid;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // interface routines
    int init_socket();
    int connect_to_socket();

    // message processing routines
    void process_message(int sock);
	int extract_header1(const char *buffer, const int length);
    message_class* extract_header(const char *buffer, const int length);

    // to be deleted after process_message works
	void extract_message (int sock);

    //debug routines
	void error(const char *msg);
	void print_buffer(const char *buffer, const int n);

    // will be moved to message class
	int writefile(unsigned long imagesize, char *image);

public:

    // con / destructors
	tcp_server();
    tcp_server(int portnumber);
	virtual ~tcp_server();

    // receive messages
	int connect_and_receive();

    // to be removed
	int listen_and_receive(char **message, unsigned long *message_length);
	int listen_and_receive_loop(char **message, unsigned long *message_length);
};

#endif /* TCP_SERVER_H_ */
