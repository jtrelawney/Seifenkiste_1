#ifndef COMMON_MESSAGE_QUEUE_CLASS_H_
#define COMMON_MESSAGE_QUEUE_CLASS_H_

#include <message_class.h>

#include <mutex>  
#include <queue>


class message_queue_class{

private:
    std::mutex controller_mutex;
    //std::queue<message_class> message_q;
    std::queue<unique_message_ptr> message_q;

public:

    message_queue_class();
    ~message_queue_class();

    //void enqueue(message_class n);
    void enqueue(unique_message_ptr message);
    unique_message_ptr dequeue();
    int get_size();
};

#endif 
