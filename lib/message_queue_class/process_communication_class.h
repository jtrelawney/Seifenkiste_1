
#ifndef PROCESS_COMMUNICATION_CLASS_H_
#define PROCESS_COMMUNICATION_CLASS_H_

#include <iostream>       // std::cout

// mutexes and cond vars
#include <mutex>          
#include <condition_variable>

// time
#include <chrono>
#include <unistd.h>

// own
#include <common.h>
#include <address_class.h>

// as it says
const int COMMUNICATION_CLASS_DEBUG_LEVEL = 5;


// max number of processes who will want to communicate
// each of them will have an index which will be used to address their condication variables, mutexes and status variables
const int G_MAX_PROCESS_COUNT = 5;


// this global class contains the condition variables and mutexes which are used 
// to organize the notifications of the participating processes
// all static varaibles are going to be initialized in the coord object cpp file 
// the flags are going to be initialized in the coord object constructor 
class G_QUEUE_COORDINATION_VARS_DEF {

private:
    // counts the number of registered processes, at shutdown this is a quick check to find out if processes have to be notified
    int registered_process_count;

    std::map<address_class,int> address_book_;

public:
    G_QUEUE_COORDINATION_VARS_DEF();
    ~G_QUEUE_COORDINATION_VARS_DEF();

    // for the process to register, so that the coordination object knows who to notify at shutdown
    static bool G_PROCESS_REGISTERED[G_MAX_PROCESS_COUNT];
    // mutexes for getting the communication locks
    static std::mutex message_available_mutex[G_MAX_PROCESS_COUNT];
    // cond vars to signal data availability
    static std::condition_variable message_available_condition[G_MAX_PROCESS_COUNT];
    // data flags
    static bool message_available_flag[G_MAX_PROCESS_COUNT];
    //bool is_condition_flag_true(const int &index) { return (condition_flags[index] == true); };

    // processes involved in communication register their id to be informed when a message is available for them
    virtual bool register_process(int id);
    virtual bool deregister_process(int id);

    // processes involved in communication register their address to be informed when a message is available for them
    virtual int register_process(const address_class address);
    virtual bool deregister_process(const address_class address);
    
    int get_registered_process_count();

    int coordination_class_debug_level_;
};

#endif
