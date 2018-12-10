// this is from
// http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

// it sends a short message to the server and gets one back
// call with the server ip address and the port
// ./tcp.exe 192.168.0.15 8192

#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

//networking 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//added those because of warnings
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <iostream>
#include <sstream>

//#include <defs.h>
#include <tcp_class.h>
#include <buffer_class.h>

//#include <message_class.h>

class tcp_client: public tcp_class {
	
private:

    int debug_level =  TCP_DEBUG_LEVEL;

	// connect and disconnect
	//int TCP_connect();	
	//int TCP_disconnect();

    // this should be deleted, header is created in message class
	//int create_header(char **sessioninfo, unsigned long data_length);	

public:
	tcp_client();
	~tcp_client();
    int start_up();
    int shut_down();

    // send a message, currently of stringtype
    // will implement a message_type next to avoid mess with 0 termniation
    //int send_message(std::string message);
    int send_message(buffer_class &buffer);
    //int send_message(message_class &message);
	
	// creates a test message and sends it over TCP	    
	//int send_test_image(char *image, unsigned long imagelen);
};

#endif
