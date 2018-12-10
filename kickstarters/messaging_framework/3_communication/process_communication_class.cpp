
#include <process_communication_class.h> 


// end flag - set by main
extern bool the_end;

// defined in header file, initilize here
// if initialized in header file or in main or anywhere else the program hangs at exit waiting to destroy the condition variables which they don't own
G_QUEUE_COORDINATION_VARS_DEF G_QUEUE_COORDINATION_VARS_OBJ = G_QUEUE_COORDINATION_VARS_DEF();
std::mutex G_QUEUE_COORDINATION_VARS_DEF::message_available_mutex[G_MAX_PROCESS_COUNT];
std::condition_variable G_QUEUE_COORDINATION_VARS_DEF::message_available_condition[G_MAX_PROCESS_COUNT];
bool G_QUEUE_COORDINATION_VARS_DEF::message_available_flag[G_MAX_PROCESS_COUNT];
bool G_QUEUE_COORDINATION_VARS_DEF::G_PROCESS_REGISTERED[G_MAX_PROCESS_COUNT];

//: id(0), shut_down_flag_(false), message_queue_class_debug_level_(MESSAGE_QUEUE_CLASS_DEBUG_LEVEL)
G_QUEUE_COORDINATION_VARS_DEF::G_QUEUE_COORDINATION_VARS_DEF() : registered_process_count(0), coordination_class_debug_level_(COMMUNICATION_CLASS_DEBUG_LEVEL), the_end(false)
{
    if (coordination_class_debug_level_>0) std::cout << "communication class : con - structor call : max process count = " << G_MAX_PROCESS_COUNT << std::endl;
    for (int i=0; i<G_MAX_PROCESS_COUNT; i++) {
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[i]=false;
        G_PROCESS_REGISTERED[i] = false;
    }
}

void G_QUEUE_COORDINATION_VARS_DEF::register_process(int id) { 
    G_PROCESS_REGISTERED[id] = true;
    registered_process_count++;
    std::cout << "registering process = " << id << std::endl;
}

void G_QUEUE_COORDINATION_VARS_DEF::deregister_process(int id) {
    G_PROCESS_REGISTERED[id] = false;
    registered_process_count--;
    std::cout << "deregistering process = " << id << std::endl;
}

G_QUEUE_COORDINATION_VARS_DEF::~G_QUEUE_COORDINATION_VARS_DEF(){ if (coordination_class_debug_level_>0) std::cout << "communication class : de - structor call" << std::endl; }

bool G_QUEUE_COORDINATION_VARS_DEF::tip_shoulder(int id){

    if (the_end == true) {
        if (coordination_class_debug_level_>1) std::cout << "end flag is set , no more messages" << std::endl;
        return false;
    }

    if (G_PROCESS_REGISTERED[id] == true) {
        if (coordination_class_debug_level_>1) std::cout << "process " << id << " is registered, get lock on mutex for process " << std::endl;
        // get the lock on the commmunication mutex, set the flag, release the lock and then notify
        {
            std::lock_guard<std::mutex> lock_communication_mutex(G_QUEUE_COORDINATION_VARS_OBJ.message_available_mutex[id]);
            if (G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[id] == true) {
                std::cout << "data flag still set - receiver is busy" << std::endl;
            }
            G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[id] = true;
        }
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[id].notify_one();
        return true;
    } else {
        if (coordination_class_debug_level_>1) std::cout << "process " << id << " is not registered" << std::endl;
        return false;
    }
}

//only the main can set the flag - no mutex
bool G_QUEUE_COORDINATION_VARS_DEF::get_shutdown_flag() { return the_end; }
void G_QUEUE_COORDINATION_VARS_DEF::set_shutdown_flag() {
    the_end = true;
    while (registered_process_count > 0) {
        notify_to_deregister();
        sleep(1);
    }
}

void G_QUEUE_COORDINATION_VARS_DEF::notify_to_deregister(){
    if (coordination_class_debug_level_>1) std::cout << "notifying the processes" << std::endl;
    for (int id=0; id<G_MAX_PROCESS_COUNT; id++) {
        if (G_PROCESS_REGISTERED[id] == true) {
            if (coordination_class_debug_level_>1) std::cout << "process " << id << " is still registered" << std::endl;
            // notify without setting the data flag
            G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[id].notify_one();
        } else if (coordination_class_debug_level_>1) std::cout << "process " << id << " is not registered" << std::endl;
    }
}

std::time_t get_time(){
    // this seems to give millis
    //return std::chrono::system_clock::to_time_t( std::chrono::high_resolution_clock::now() );
    // this is nanos
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return nanoseconds.count();
}                
