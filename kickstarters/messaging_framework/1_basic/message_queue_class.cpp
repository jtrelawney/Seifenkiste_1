
#include <message_queue_class.h>

message_queue_class::message_queue_class(){ printf("message_queue_class : Contructor call\n"); }
message_queue_class::~message_queue_class(){ printf("message_queue_class : Destructor call\n"); }


void message_queue_class::enqueue(message_class m) {
    int m_id = m.get_id();
    int m_data = m.get_data();
    printf("message_queue_class :  enqueue %i   %i\n",m_id, m_data);
    std::lock_guard<std::mutex> lck (controller_mutex);
    message_q.push(m);
}

message_class message_queue_class::dequeue() {
    std::lock_guard<std::mutex> lck (controller_mutex);
    if (message_q.empty()) return -1;
    message_class m=message_q.front();
    message_q.pop();
    return m;        
}

int message_queue_class::get_size() {
    std::lock_guard<std::mutex> lck (controller_mutex);
    int counter = message_q.size();
    return counter;
}
