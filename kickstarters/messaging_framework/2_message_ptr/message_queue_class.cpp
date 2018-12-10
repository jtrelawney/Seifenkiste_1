
#include <message_queue_class.h>

message_queue_class::message_queue_class(){ printf("message_queue_class : Contructor call\n"); }
message_queue_class::~message_queue_class(){ printf("message_queue_class : Destructor call\n"); } //message_q = std::queue<unique_message_ptr>(); }


void message_queue_class::enqueue(unique_message_ptr message) {
    //int m_id = message->get_id();
    //int m_data = message->get_data();
    //printf("message_queue_class :  enqueue %i   %i\n",m_id, m_data);
    std::lock_guard<std::mutex> lck (controller_mutex);
    //message_q.push(message); // this doesn't compile, ownerhsip needs to be transfered
    message_q.push(std::move(message));
}

unique_message_ptr message_queue_class::dequeue() {
    std::lock_guard<std::mutex> lck (controller_mutex);
    if (message_q.empty()) return NULL;
    //unique_message_ptr m = message_q.front();     // this throws up bcuase ownership should change
    unique_message_ptr m = std::move(message_q.front());
    message_q.pop();
    return m;        
}

int message_queue_class::get_size() {
    std::lock_guard<std::mutex> lck (controller_mutex);
    int counter = message_q.size();
    return counter;
}
