/*
 * tcp_server.h
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

class tcp_server {
private:
    int sockfd, newsockfd, portno, pid;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;

	void error(const char *msg);
	int extract_header(const char *buffer, const int length);
	void extract_message (int sock);
	int writefile(unsigned long imagesize, char *image);
	void print_buffer(const char *buffer, const int n);

public:
	tcp_server();
	int listen_and_receive(char **message, unsigned long *message_length);
	int listen_and_receive_loop(char **message, unsigned long *message_length);
	virtual ~tcp_server();
};

#endif /* TCP_SERVER_H_ */
