#ifndef MESSAGE_QUEUE_CLASS_H_
#define MESSAGE_QUEUE_CLASS_H_

#include <iostream>       // std::cout
#include <queue>         // queue

#include <mutex>          // std::mutex
#include <condition_variable>

#include <chrono>

#include <process_communication_class.h>

#include <common.h>
#include <address_class.h>
#include <message_class.h>


const int MESSAGE_QUEUE_CLASS_DEBUG_LEVEL = 0;

extern G_QUEUE_COORDINATION_VARS_DEF G_QUEUE_COORDINATION_VARS_OBJ;
// global mutex and condition vars defined for message_queue
const int G_PROCESS_COUNT = address_class::MAX_PROCESS_COUNT;


// define as extern and initialize in the message_queue.cpp file
// through import the defintions are available where necessary
// initializing anywhere else blocks at program exit, maybe because this creates conflicting exit sequences
//extern G_QUEUE_COORDINATION_VARS_DEF G_QUEUE_COORDINATION_VARS_OBJ;// = G_QUEUE_COORDINATION_VARS_DEF();

using com = G_QUEUE_COORDINATION_VARS_DEF;

class message_queue_class : public G_QUEUE_COORDINATION_VARS_DEF {

private:

    int message_counter_;

    // debug output 
    int message_queue_class_debug_level_;

    // signal when the message will be stopped and the communication will be shut down
    bool shut_down_flag_;

    // protect internal vars from races
    std::mutex message_queue_class_mutex_;

    // the queue data :
    // 1. the recipients of the message in the queue
    // 2. the message
    std::queue<address_class> recipient_address_q_;
    std::queue<std::unique_ptr<message_class> > message_q_;

    // to notify a process that a message is available
    // only call when the data lock has been obtained
    void notify_first();
    void notify(const address_class &recipient);

    // delete when everythting workds
    //int id;
    //static std::mutex id_mutex;

public:
    message_queue_class();
    virtual ~message_queue_class();

    // add a message at the end of the queue
    void enqueue(std::unique_ptr<message_class> message);
    // remove the first message in the queue, the recipient calls and if he is the recipient the message is taken out of the queue
    std::unique_ptr<message_class> dequeue(address_class recipient);

    // flag to manage when the communcation will be ended
    bool get_shut_down_flag();
    bool set_shut_down_flag();

    // management functions
    void set_debug_level(const int &level);
    bool is_empty();

};
#endif
