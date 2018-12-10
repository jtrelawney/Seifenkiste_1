/*
 * tcp_server.h
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#include <tcp_class.h>

//#include <message_class.h>
//#include <message_queue_class.h>

#define MAX_HEAD_LENGTH 256
#define TCP_HEADER_LENGTH 37

class tcp_server: public tcp_class {

private:

    int debug_level =  TCP_DEBUG_LEVEL;

    // here the received message can be processed
    // for example header verification, data separation, pushing out to a 3rd party interface
    void process_messages();

public:

    // con / destructors
	tcp_server();
	~tcp_server();

    // creats a socket, binds and listens
    int start_up();
    // shuts down the server by closing any open file descriptors
    // since the call blocks (see below) ...
    int shut_down();

    // receive messages
    // this implements a blocking call to tcp - accept
    // once a connection is requested a child process if forked to handle the communication and
    // the routine makes another blocking accept call so that other connections can be accepted
	// currently this is a endless loop which does not return
    // in a next version a non blocking call could be implemented
    int connect_and_receive();

    // currently the incoming messages are displayed on the screen
    // in the next version an interface needs to be implmented
    // the messages may need processing , i.e. header verificiation and data conversion
    // - either a caller copies the messages over or the server process copies them out into a different object
    // both need an interface such as
    
    // void set_global_message_queue
};

#endif /* TCP_SERVER_H_ */
