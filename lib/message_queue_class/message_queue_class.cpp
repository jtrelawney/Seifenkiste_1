#include <message_queue_class.h>

message_queue_class::message_queue_class() :
    message_counter_(0),
    queue_platform_for_routing_(address_class::platform_type_def::undefined_platform),
    message_queue_class_debug_level_(MESSAGE_QUEUE_CLASS_DEBUG_LEVEL),
    shut_down_flag_(false),
    q_state_(ok)
{
    if (message_queue_class_debug_level_>0) std::cout << "message queue : default constructor" << std::endl;
}

message_queue_class::message_queue_class(address_class::platform_type_def queue_platform) :
    message_counter_(0),
    queue_platform_for_routing_(queue_platform),
    message_queue_class_debug_level_(MESSAGE_QUEUE_CLASS_DEBUG_LEVEL),
    shut_down_flag_(false),
    q_state_(ok)
{
    if (message_queue_class_debug_level_>0) std::cout << "message queue : constructor called with platform " << queue_platform_for_routing_ << std::endl;
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


bool message_queue_class::enqueue(std::unique_ptr<message_class> message){

    // first check if maybe shutdown is going and messages should be rejected
    bool shutdown_going_on = ( (q_state_ == shutdown_requested_reject_messages) || ( q_state_ == shutdown_notifying_clients) );
    if ( shutdown_going_on == true ) {
        if (message_queue_class_debug_level_>1) std::cout << "message_queue : enqueue : shutdown going on, reject t messsage" << std::endl;
        return false;    
    }

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
    // determine the recipient, check if the message is for a different platform or for a process on the same platform
	if (does_message_need_routing_via_tcp(recipient) == true ) {
		if (message_queue_class_debug_level_>0) std::cout << "message queue : enqueue : route message via tcp to " << recipient << std::endl;
		// replace  with the tcp address
		recipient = get_address_of_tcp_process();
	}

    if (message_queue_class_debug_level_>2) std::cout << "recipient = " << recipient << std::endl;

    // now queue the recipient and the message
    recipient_address_q_.push(recipient);
    message_q_.push(std::move(message));
    if (message_queue_class_debug_level_>1) std::cout << "message recorded" << std::endl;

    // at this point the message and its recepient have been recorded and the queue is consistent, with at least one message
    // hence notify the recipient of the first message in the queue
    // note: the lock on the message queue is still in place
    notify_first();
    return true;
}

bool message_queue_class::is_message_queue_platform_defined() {

    if (message_queue_class_debug_level_>1) {
            std::cout << "message_queue : routing check : message queue address" << std::endl;
            std::cout << queue_platform_for_routing_ << std::endl;
    }

    // assume address is defined and change if it is not
    bool result = true;
    if ( queue_platform_for_routing_ == address_class::platform_type_def::undefined_platform ) {
		if (message_queue_class_debug_level_>0) {
            std::cout << "message_queue : routing check : message queue address undefined!" << std::endl;
        }
        result = false;
    }
    return result;
}

bool message_queue_class::does_message_need_routing_via_tcp(const address_class &recipient) {

	bool result = ( queue_platform_for_routing_ != recipient.get_platform() );
    if (message_queue_class_debug_level_>1) {
		std::cout << "message_queue : result doweneedroutingcheck = " << result << std::endl;
	}
	return result; 
}

// this is called after a message has been queueu
// at this point at least one message is in the queue
// this function notifies the recipeint of the first message in the queue
// if the recipient is on the same platform -> wakeup
// if the recipient is on a different platform -> pass to tcp
        
// !!! important the caller holds the lock on the message queue mutex
void message_queue_class::notify_first(){
    // when notify is called the message queue shouldn't be empty, but check anyway
    if (!message_q_.empty()) {

        // determine the recipient and its index in the process table
        address_class recipient = recipient_address_q_.front();
		int recipient_index = get_process_id(recipient);// recipient.get_process();
        if (message_queue_class_debug_level_>1) std::cout << "message_queue : notify " << recipient << std::endl;

        // get the lock on the commmunication mutex, set the flag, release the lock and then notify
        {
            std::lock_guard<std::mutex> lock_communication_mutex(message_available_mutex[recipient_index]);
            if (message_available_flag[recipient_index] == true) {
                std::cout << "message_queue - data flag still set - has the recipient " << recipient_index << " not picked it up yet" << std::endl;
            }
            message_available_flag[recipient_index] = true;
        }
        message_available_condition[recipient_index].notify_one();
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
        int index = get_process_id(stored_recipient);//(int) stored_recipient.get_process_index();
        message_available_condition[index].notify_one();        
    }
    return std::move(dequeued_message);        
}


void message_queue_class::notify(const address_class &recipient){
    int index = (int) recipient.get_process();
    if (message_queue_class_debug_level_>1) std::cout << "q notifies " << recipient << std::endl;
    message_available_condition[index].notify_one();
}

// this implements the first phase of the shutdown
// enqueuing new messages will be rejected
// and the current queue will be flushed
// after that the system is ready for the shutdown communication via global end flags and condition variables
bool message_queue_class::request_shut_down() {
    if (message_queue_class_debug_level_>0) std::cout << "message queue : request shutdown  called" << std::endl;
    q_state_ = shutdown_requested_reject_messages;
    flush_queue();
    q_state_ = shutdown_notifying_clients;
    if (message_queue_class_debug_level_>0) std::cout << "message queue : queue flush complete" << std::endl;
    return true;
}

// when preparing for the shutdown, empty out all existing messages in the queue
void message_queue_class::flush_queue(){
    // little problem: we don't own the lock, so choose controlled shutdown with waits 
    if (message_queue_class_debug_level_>1) std::cout << "message queue : flushing queue by notifying the receiver of the first message" << std::endl;
    while (!is_empty()) {
        notify_first();
        // give some time for the recipient to pick up the message
        usleep(10000);
    }
}

// this implements the 2nd phase of the shutdown
// at this point the queue is empty and no new messages are being accepted
// also the shutdown requester had time to complete his preparations and has now called for the shutdown to complete
// the queue now informs all registered clients that they shutdown is taking place
// for them it means they should complete their shutdown activiities and deregister
bool message_queue_class::shut_down(){

    // first set the flag so that from now on everybody reading the shutdownflag is reading the new status
    if (message_queue_class_debug_level_>1) std::cout << "message queue : queue is empty, set shutdown flag and wake everybody" << std::endl;

    // this is in brackets so the the lock guard will be released after the section
    {
        std::lock_guard<std::mutex> lck (message_queue_class_mutex_);
        if (message_queue_class_debug_level_>1) std::cout << "message queue :  queue has mutex and sets shutdownflag" << std::endl;
        shut_down_flag_ = true;
    }

    // now informat all registered processes of the shutdown
    while (get_registered_process_count()>0) {
        notify_all_about_shoutdown();
        usleep(10000);
    }
    if (message_queue_class_debug_level_>1) std::cout << "message queue :  all processes have deregistered" << std::endl;
    return true;
}

// this gets a list of all processes in the addressbook and wakes them to signal the shutdown
void message_queue_class::notify_all_about_shoutdown() {
    //get list with ids of registered processes
    std::vector<int> process_list = get_registered_process_list();
    int N = process_list.size();
    if (message_queue_class_debug_level_>1)
        std::cout << "message queue : now waking the " << N << " registered processes to ensure they see the end flag" << std::endl;

    for (int i=0; i<N; i++){
        if (message_queue_class_debug_level_>-1) std::cout << "message queue : shutdown -> check if mutex is required" << std::endl;
        //std::unique_lock<std::mutex> lock_it(message_available_mutex[reader_index]);
        int id = process_list[i];
        if (message_queue_class_debug_level_>1) std::cout << "message queue : notifying process id = " << id << std::endl;
        // set the flag to false, ie.e. data is not avaible, so for receiver this could be a spurious wakeup, however the shutdown flag will be checked
        message_available_flag[id] = false;
        message_available_condition[id].notify_one();
    }
}

/*
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
    for (int i=0; i<G_MAX_PROCESS_COUNT; i++){
        //std::unique_lock<std::mutex> lock_it(message_available_mutex[reader_index]);
        if (message_queue_class_debug_level_>1) std::cout << "notify reader = " << i << std::endl;
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[i].notify_one();
    }
    return true;
}
*/

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
