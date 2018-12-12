#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

#include <functional>

//#include <common.h>
//extern end_flag_class G_END_FLAG;

const int TCP_SERVER_DEBUG_LEVEL = 0;
#include <tcp_class.h>

#include <buffer_class.h>
#include <address_class.h>
#include <message_class.h>
#include <message_queue_class.h>

// try this with predicate first
//extern end_flag_class G_END_FLAG;
class tcp_server: public tcp_class {

private:

    enum receiver_states_ { read_header, read_data, data_complete, wrong_state, error_reading_tcp, client_shutting_down, error_initializing_message, error_inserting_data_into_message, fucked_up, totally_fucked_up };


    int tcp_server_debug_level_;

    // here the received message can be processed
    // for example header verification, data separation, pushing out to a 3rd party interface
    void process_messages();
    receiver_states_ receive_header_message(const receiver_states_ &current_state, unique_message_ptr &message);
    receiver_states_ receive_data_message(const receiver_states_ &current_state, unique_message_ptr &message);

public:

    // con / destructors
	tcp_server();
	~tcp_server();
    void set_debug_level(const int level);

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

    std::function<bool ()> check_termination_call_back_predicate = nullptr;
    void set_termination_predicate(std::function<bool ()>);

    // define a queue object which can be set, it will not be deleted in the destructor, the provider  
    message_queue_class *my_queue_;
    void set_queue_ptr(message_queue_class *ptr);


    // currently the incoming messages are displayed on the screen
    // in the next version an interface needs to be implmented
    // the messages may need processing , i.e. header verificiation and data conversion
    // - either a caller copies the messages over or the server process copies them out into a different object
    // both need an interface such as
    
    // void set_global_message_queue
};

#endif /* TCP_SERVER_H_ */
