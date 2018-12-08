#include <message_queue_class.h>

message_queue_class::message_queue_class(): message_counter_(0), message_queue_class_debug_level_(MESSAGE_QUEUE_CLASS_DEBUG_LEVEL) , shut_down_flag_(false) {
    if (message_queue_class_debug_level_>0) std::cout << "message queue : default constructor" << std::endl;
}

message_queue_class::~message_queue_class() {
    if (message_queue_class_debug_level_>0) std::cout << "q destructor" << std::endl;
}

bool message_queue_class::is_empty() {
    std::lock_guard<std::mutex> lck (message_queue_class_mutex_);
    return message_q_.empty();
}

bool message_queue_class::get_shut_down_flag() {
        std::lock_guard<std::mutex> lck (message_queue_class_mutex_);
        return shut_down_flag_;
}

void message_queue_class::set_debug_level(const int &level) { message_queue_class_debug_level_ = level; }


void message_queue_class::enqueue(std::unique_ptr<message_class> message){
    message_counter_++;
    if (message_queue_class_debug_level_>1) {
        std::time_t ct = get_time();
        std::cout << "q ing = " << message_counter_ << " @ " << ct << std::endl;
    }
    // get a lock on the data
    std::lock_guard<std::mutex> lck (message_queue_class_mutex_);
    if (message_queue_class_debug_level_>1) std::cout << "message_queue : enqueue : lock and record the message " << (void*) &message << std::endl;

    // first get the address, after queuing the message (and moving ownership) it will be invalid
    address_class recipient = message -> get_recipient_address();
    if (message_queue_class_debug_level_>2) std::cout << "recipient = " << recipient << std::endl;

    // now queue the recipient and the message
    recipient_address_q_.push(recipient);
    message_q_.push(std::move(message));
    if (message_queue_class_debug_level_>1) std::cout << "message recorded" << std::endl;

    // at this point the message and its recepient have been recorded and the queue is consistent, with at least one message
    // hence notify the recipient of the first message in the queue
    // note: the lock on the message queue is still in place
    notify_first();
}

// this is called after a message has been queueu
// at this point at least one message is in the queue
// this function notifies the recipeint of the first message in the queue
// !!! important the caller holds the lock on the message queue mutex
void message_queue_class::notify_first(){
    // when notify is called the message queue shouldn't be empty, but check anyway
    if (!message_q_.empty()) {

        // determine the recipient and its index in the process table
        address_class recipient = recipient_address_q_.front();
        int recipient_index = (int) recipient.get_process();
        if (message_queue_class_debug_level_>1) std::cout << "q notifies " << recipient << std::endl;

        // get the lock on the commmunication mutex, set the flag, release the lock and then notify
        {
            std::lock_guard<std::mutex> lock_communication_mutex(G_QUEUE_COORDINATION_VARS_OBJ.message_available_mutex[recipient_index]);
            if (G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[recipient_index] == true) {
                std::cout << "message_queue - data flag still set - has the recipient " << recipient_index << " not picked it up yet" << std::endl;
            }
            G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[recipient_index] = true;
        }
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[recipient_index].notify_one();
    }
}

// caller gives its address and the function confirms that the 
std::unique_ptr<message_class> message_queue_class::dequeue(address_class target) {
    if (message_queue_class_debug_level_>1) {
        std::time_t ct = get_time();
        std::cout << "de - q ing for " << target << " @ " << ct << std::endl;
    }

    std::unique_ptr<message_class> dequeued_message = nullptr;

    // get a lock on the data and make sure the queue is not empty
    std::lock_guard<std::mutex> lck (message_queue_class_mutex_);
    if (message_queue_class_debug_level_>1) std::cout << "message queue obtained lock" << std::endl;
    if (message_q_.empty()) {
         if (message_queue_class_debug_level_>0) std::cout << "message queue is empty" << std::endl;
        return dequeued_message;
    }

    // if data is available find out for who the first entry is
    address_class stored_recipient = recipient_address_q_.front();
    if (stored_recipient == target){
        if (message_queue_class_debug_level_>1) std::cout << "caller matches recipient" << std::endl;
        recipient_address_q_.pop();
        dequeued_message = std::move(message_q_.front());
        message_q_.pop();
        // at this point the queues are consistent, both the target and the data queue have been removed
    } else {
        if (message_queue_class_debug_level_>1) std::cout << "q  message is for pid = " << stored_recipient << " , notify" << std::endl;
        int index = (int) stored_recipient.get_process_index();
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[index].notify_one();        
    }
    return std::move(dequeued_message);        
}


void message_queue_class::notify(const address_class &recipient){
    int index = (int) recipient.get_process();
    if (message_queue_class_debug_level_>1) std::cout << "q notifies " << recipient << std::endl;
    G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[index].notify_one();
}

bool message_queue_class::set_shut_down_flag() {
    std::time_t ct = get_time();
    if (message_queue_class_debug_level_>0) std::cout << "q shutdown down @ " << ct << std::endl;
    
    // the writer will not send new messages, so first make sure the queue is empty
    // little problem: we don't own the lock, so choose controlled shutdown with waits 
    while (!is_empty()) {
        if (message_queue_class_debug_level_>1) std::cout << "notifying first in queue ..." << std::endl;
        notify_first();
        usleep(10000);
    }

    // once the queue is empty set the flag and wake all sleeping processes so that they can see it and exit
    if (message_queue_class_debug_level_>1) std::cout << "queue is empty, set flag and wake everybody" << std::endl;
    {
        std::lock_guard<std::mutex> lck (message_queue_class_mutex_);
        if (message_queue_class_debug_level_>1) std::cout << "queue has mutex to set shutdownflag" << std::endl;
        shut_down_flag_ = true;
    }

    if (message_queue_class_debug_level_>1) std::cout << "shutdown waking all process to ensure they see the end flag" << std::endl;
    for (int i=0; i<G_PROCESS_COUNT; i++){
        //std::unique_lock<std::mutex> lock_it(message_available_mutex[reader_index]);
        if (message_queue_class_debug_level_>1) std::cout << "notify reader = " << i << std::endl;
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[i].notify_one();
    }
    return true;
}

/*
    message_queue_class(): id(0),shut_down_flag(false){};

    virtual ~message_queue_class(){ std::cout << "q destructor" << std::endl;};

    int next_id(){
        std::lock_guard<std::mutex> lck (id_mutex);
        id++;
        return id;
    }

    void enqueue(int n, int target_id) {
        std::time_t ct = get_time();
        std::cout << "q ing = " << n << " for " << target_id << " @ " << ct << std::endl;

        // get a lock on the data
        std::lock_guard<std::mutex> lck (message_queue_class_mutex);
        std::cout << "q gets message lock and records " << n << std::endl;
        message_q.push(n);
        target_ids_q.push(target_id);
        notify_first();
    }
    int dequeue(int target_id) {
        std::time_t ct = get_time();
        std::cout << "de - q ing for " << target_id << " @ " << ct << std::endl;

        // get a lock on the data
        std::lock_guard<std::mutex> lck (message_queue_class_mutex);
        if (message_q.empty()) return -1;

        // if data is available find out for who the first entry is
        int n, stored_target_id = target_ids_q.front();
        if (stored_target_id == target_id){
            target_ids_q.pop();
            n = message_q.front();
            message_q.pop();
            std::cout << "q  message is for r = " << stored_target_id << " n = " << n << std::endl;
            // at this point the queues are consistent, both the target and the data queue have been poped
            // but if the queue is still not empty then the next message needs to be processed too
            notify_first();
        } else {
            std::cout << "q  message is for r = " << stored_target_id << " , notify" << std::endl;
            message_available_condition[stored_target_id].notify_one();
            n = -1;            
        }
        return n;        
    }
    int get_size() {
        std::lock_guard<std::mutex> lck (message_queue_class_mutex);
        int counter = message_q.size();
        return counter;
    };




};
*/
