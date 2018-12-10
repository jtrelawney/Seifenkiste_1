#include <process_communication_class.h> 

// defined in header file, initilize here
// if initialized in header file or in main or anywhere else the program hangs at exit waiting to destroy the condition variables which they don't own
G_QUEUE_COORDINATION_VARS_DEF G_QUEUE_COORDINATION_VARS_OBJ = G_QUEUE_COORDINATION_VARS_DEF();
std::mutex G_QUEUE_COORDINATION_VARS_DEF::message_available_mutex[G_MAX_PROCESS_COUNT];
std::condition_variable G_QUEUE_COORDINATION_VARS_DEF::message_available_condition[G_MAX_PROCESS_COUNT];
bool G_QUEUE_COORDINATION_VARS_DEF::message_available_flag[G_MAX_PROCESS_COUNT];
bool G_QUEUE_COORDINATION_VARS_DEF::G_PROCESS_REGISTERED[G_MAX_PROCESS_COUNT];

// class member to synchronize access to the addressbook
// needs to be static because of global variable status
std::mutex G_QUEUE_COORDINATION_VARS_DEF::process_class_mutex_;

//: id(0), shut_down_flag_(false), message_queue_class_debug_level_(MESSAGE_QUEUE_CLASS_DEBUG_LEVEL)
G_QUEUE_COORDINATION_VARS_DEF::G_QUEUE_COORDINATION_VARS_DEF()
    :   registered_process_count(0),
        coordination_class_debug_level_(COMMUNICATION_CLASS_DEBUG_LEVEL)
{
    if (coordination_class_debug_level_>0) std::cout << "communication class : con - structor call : max process count = " << G_MAX_PROCESS_COUNT << std::endl;
    for (int i=0; i<G_MAX_PROCESS_COUNT; i++) {
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[i]=false;
        G_PROCESS_REGISTERED[i] = false;
    }
}

bool G_QUEUE_COORDINATION_VARS_DEF::register_process(int id) {

    std::lock_guard<std::mutex> lck (process_class_mutex_);

    int check_id = id % G_MAX_PROCESS_COUNT; 
    if (check_id != id) {
        std::cout << "id = " << id << " of of range (max = " << G_MAX_PROCESS_COUNT << " )" << std::endl;
        return false;
    }

    std::cout << "registering process = " << id << std::endl;
    G_PROCESS_REGISTERED[id] = true;
    registered_process_count++;
    return true;
}

bool G_QUEUE_COORDINATION_VARS_DEF::deregister_process(int id) {

    std::lock_guard<std::mutex> lck (process_class_mutex_);

    int check_id = id % G_MAX_PROCESS_COUNT; 
    if (check_id != id) {
        std::cout << "id = " << id << " of of range (max = " << G_MAX_PROCESS_COUNT << " )" << std::endl;
        return false;
    }

    std::cout << "deregistering process = " << id << std::endl;
    G_PROCESS_REGISTERED[id] = false;
    registered_process_count--;
    return true;
}

// register a process in the address book
// assign the unique identifier and enable it in the processlist
// if the address exists already -> return its existing index 
// if the max count has been reached already - > return -1
// else find the first available index, assign it, enable the process, increase processcount, return the new index

int G_QUEUE_COORDINATION_VARS_DEF::register_process(const address_class address){

    std::lock_guard<std::mutex> lck (process_class_mutex_);

    // check if the address has been already registered
    int count = address_book_.count(address);
    //std::cout << "found " << count << " entries" << std::endl;
    if (count>0) {
        int index = address_book_[address];
        std::cout << "coordination object : register process : " << address << " exists, index = " << index << std::endl;
        return index;
    }

    // find the first empty slot and register the process with that index
    //std::cout << "find the first empty slot" << std::endl;
    int index(0);
    while (index<G_MAX_PROCESS_COUNT) {
        if (G_PROCESS_REGISTERED[index] == false) {
            std::cout << "registering process " << address << "   id = " << index << std::endl;
            std::pair<address_class,int> new_entry(address,index);
            //std::cout << "now insert" << std::endl;
            address_book_.insert( std::pair<address_class,int>(address,index) );
            G_PROCESS_REGISTERED[index] = true;
            registered_process_count++;
            return index;
        }
        index++;
    }

    // no slot in reange found -> max is reached already
    std::cout << "coordination object : register process : registered process count already maxed out = (" << G_MAX_PROCESS_COUNT << ")" << std::endl;
    return -1;
}

// find the index of the address to be deregistered and delete the entry
// return false if the address doesn't exist in the address book
// return true if successfully 
bool G_QUEUE_COORDINATION_VARS_DEF::deregister_process(const address_class address){

    std::lock_guard<std::mutex> lck (process_class_mutex_);

    // find the entry 
    //std::cout << "de-registing, find " << address << std::endl;
    std::map<address_class,int>::iterator it = address_book_.find(address);
    //std::cout << "lets see ... " << std::endl;
    if (it!=address_book_.end() ){
        int index =  it->second;
        //std::cout << "found index = " << index << std::endl;
        int count = address_book_.erase(address);
        //std::cout << "count = " << count << std::endl;
        //std::map<address_class,int>::iterator del = address_book_.erase(it);
        //std::cout << "deleted = " << del->second << std::endl;
        if (count > 0) {
            std::cout << "coordination object : de-register process : " << address << " index = " << index << " has been deregistered" << std::endl;
            G_PROCESS_REGISTERED[index] = false;
            registered_process_count--;
            return true;
        } else {
            std::cout << "coordination object : de-register process : " << address << " index = " << index << " exists, but erase failed" << std::endl;
            return false;
        }
    }

    std::cout << "coordination object : de-register process : " << address << " not in address book" << std::endl;
    return false;
}

int G_QUEUE_COORDINATION_VARS_DEF::get_registered_process_count(){
    std::lock_guard<std::mutex> lck (process_class_mutex_);
    return registered_process_count;
}

std::vector<int> G_QUEUE_COORDINATION_VARS_DEF::get_registered_process_list(){

    std::lock_guard<std::mutex> lck (process_class_mutex_);

    if (coordination_class_debug_level_>0) std::cout << "communication class : create list with registered processes" << std::endl;
    std::vector<int> process_list;

	std::map<address_class, int>::iterator it = address_book_.begin();
	while (it != address_book_.end())
	{
		process_list.push_back(it->second);
		it++;
	}
    return process_list;
}

G_QUEUE_COORDINATION_VARS_DEF::~G_QUEUE_COORDINATION_VARS_DEF(){
    if (coordination_class_debug_level_>0) std::cout << "communication class : de - structor call" << std::endl;
}
