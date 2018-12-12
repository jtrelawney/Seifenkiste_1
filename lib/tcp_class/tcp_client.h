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

#include <unistd.h> 
#include <thread> 
#include <iostream>

#include <buffer_class.h>
#include <tcp_class.h>
#include <message_class.h>
#include <message_queue_class.h>

const int TCP_CLIENT_DEBUG_LEVEL = 0;

//#include <message_class.h>

class tcp_client: public tcp_class {
	
private:

	address_class tcp_client_address_;
	int tcp_client_process_id_;
    // define a queue object which can be set, it will not be deleted in the destructor, the provider  
    message_queue_class *my_queue_ptr_;
    
    int tcp_client_debug_level_;

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
    void print_status();
	void set_debug_level(const int &level);
	
    int send_message(unique_message_ptr message);
    int send_message(buffer_class &buffer);
    
    // define a queue object which can be set, it will not be deleted in the destructor, the provider  
    void set_queue_ptr(message_queue_class *ptr);
    
    // returns a client thread which is a call to keep_processing
    // call: std::thread cockpit_thread = cockpit_class(cockpit_addr)();
    std::thread operator()();
        
    // this is the main routine of the cockpit thread
    // it sleeps waiting for the condition variable which is set by the message queue
    // then it fetches data from teh message queue, processes and displays the result
    // in each wakeup it monitors the global endflag to learn when it should terminate
    // in the main loop the message queue shutdown flag indicates when to exit the process
    void keep_processing();

	// creates a test message and sends it over TCP	    
	//int send_test_image(char *image, unsigned long imagelen);
};

#endif
