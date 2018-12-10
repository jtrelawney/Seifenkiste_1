
#ifndef PROCESS_COMMUNICATION_CLASS_H_
#define PROCESS_COMMUNICATION_CLASS_H_

#include <iostream>       // std::cout

#include <mutex>          // std::mutex
#include <condition_variable>
#include <chrono>
#include <unistd.h>

const int COMMUNICATION_CLASS_DEBUG_LEVEL = 5;

// replace with above
std::time_t get_time();

// max number of processes who will want to communicate
// each of them will have an index which will be used to address their condication variables, mutexes and status variables
const int G_MAX_PROCESS_COUNT = 5;

// this global class contains the condition variables and mutexes which are used 
// to organize the notifications of the participating processes
// variables only!
// in the message queue cpp file all static varaibles are going to be initialized
// in the message queue constructor the flags are going to be initialized
class G_QUEUE_COORDINATION_VARS_DEF {

private:
    int registered_process_count;
public:
    G_QUEUE_COORDINATION_VARS_DEF();
    ~G_QUEUE_COORDINATION_VARS_DEF();

    // for the process to register, so that the message queue knows who to notify at shutdown
    static bool G_PROCESS_REGISTERED[G_MAX_PROCESS_COUNT];
    // mutexes for getting the locks
    static std::mutex message_available_mutex[G_MAX_PROCESS_COUNT];
    // cond vars to signal the wakeups
    static std::condition_variable message_available_condition[G_MAX_PROCESS_COUNT];
    // flags
    static bool message_available_flag[G_MAX_PROCESS_COUNT];
    //bool is_condition_flag_true(const int &index) { return (condition_flags[index] == true); };

    void register_process(int id);
    void deregister_process(int id);
    
    int coordination_class_debug_level_;

    // for testing
    bool the_end;
    bool tip_shoulder(int id);
    bool get_shutdown_flag();
    void set_shutdown_flag();
    void notify_to_deregister();

};

#endif
