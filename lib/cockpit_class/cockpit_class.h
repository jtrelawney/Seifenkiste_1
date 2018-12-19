#ifndef COCKPIT_CLASS_H
#define COCKPIT_CLASS_H

// this class defines the message processing and display operations
// - create one large dipslay window and display at a certain frequency
// - receive messages and process the messages according to its source/content
// - subdivide the output window into subwindows which display their own information
//          - this allows creation of independent processes which update their area
// - clean up after global end flag has been set

#include <iostream>
#include <chrono>

// message_queue_class.h imports the coordination variables and mutex
#include <thread>
#include <mutex>
#include <condition_variable>


#include "opencv2/opencv.hpp"

#include <common.h>
#include <address_class.h>
#include <sensor_camera_class.h>
#include <message_class.h>
#include <message_queue_class.h>

// define a global queue object for all processes / objects to refer to using extern 
extern message_queue_class *G_MESSAGE_QUEUE_PTR;
extern end_flag_class G_END_FLAG;

const int COCKPIT_DEBUG_LEVEL = 3;

class cockpit_class{
private:
    unsigned int event_counter_;
    address_class whoami_;
    int cockpit_process_id_;
    int cockpit_debug_level_;
    cockpit_class();

public:
    cockpit_class(const address_class &who_am_i);
    virtual ~cockpit_class();

    // returns a cockpit thread which is a call to keep_processing
    // call: std::thread cockpit_thread = cockpit_class(cockpit_addr)();
    std::thread operator()();

    // this is the main routine of the cockpit thread
    // it sleeps waiting for the condition variable which is set by the message queue
    // then it fetches data from teh message queue, processes and displays the result
    // in each wakeup it monitors the global endflag to learn when it should terminate
    // in the main loop the message queue shutdown flag indicates when to exit the process
    void keep_processing();

    // data processing functions

};

#endif
