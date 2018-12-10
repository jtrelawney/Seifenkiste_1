#ifndef COMMON_MESSAGE_QUEUE_CLASS_H_
#define COMMON_MESSAGE_QUEUE_CLASS_H_

#include <message_class.h>

#include <mutex>  
#include <queue>


class message_queue_class{

private:
    std::mutex controller_mutex;
    std::queue<message_class> message_q;

public:

    message_queue_class();
    ~message_queue_class();

    void enqueue(message_class n);
    message_class dequeue();
    int get_size();
};

#endif 
