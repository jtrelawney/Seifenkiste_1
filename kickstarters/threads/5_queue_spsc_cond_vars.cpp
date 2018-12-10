// mutex example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <condition_variable>
#include <chrono>
#include <unistd.h>         // usleep
#include <queue>         // queue

std::time_t get_time(){

    // this seems to give millis
    //return std::chrono::system_clock::to_time_t( std::chrono::high_resolution_clock::now() );
    // this is nanos
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return nanoseconds.count();
}

class controller;
static controller *global_ptr_controller;

std::condition_variable message_available_condition;
std::mutex message_available_mutex;

bool the_end = false;
//int global_counter = 10;

class controller{
private:
    int id;
    std::mutex id_mutex;
    std::mutex controller_mutex;
    bool shut_down_flag;

    std::queue<int> message_q;

public:
    controller(): id(0),shut_down_flag(false){};
    virtual ~controller(){ std::cout << "q destructor" << std::endl;};
    int next_id(){
        std::lock_guard<std::mutex> lck (id_mutex);
        id++;
        return id;
    }
    void enqueue(int n) {
        std::time_t ct = get_time();
        std::cout << "q ing = " << n << " @ " << ct << std::endl;

        std::lock_guard<std::mutex> lck (controller_mutex);

        std::cout << "q gets message lock and records " << n << std::endl;
        std::unique_lock<std::mutex> message_lock (message_available_mutex);
        message_q.push(n);
        std::cout << "q notifies r" << std::endl;
        message_available_condition.notify_all();
        
    }
    int dequeue() {
        std::lock_guard<std::mutex> lck (controller_mutex);
        if (message_q.empty()) return -1;
        int n = message_q.front();
        message_q.pop();
        return n;        
    }
    int get_size() {
        std::lock_guard<std::mutex> lck (controller_mutex);
        int counter = message_q.size();
        return counter;
    };
    void shut_down() {
        std::time_t ct = get_time();
        std::cout << "q shutdown down @ " << ct << std::endl;
        shut_down_flag = true;
        // reader is asleep, so cant see the flag -> wake r up
        std::unique_lock<std::mutex> message_lock (message_available_mutex);
        message_available_condition.notify_all();
    };
    bool get_shut_down_flag() { return shut_down_flag; };
};

void watcher(int n) {
    std::cout << "createing watcher which stops the processes in " << n << " secs" << std::endl;
    sleep(n);
    the_end = true;
}

void writer(unsigned int t) {
    std::time_t ct;
    while ( the_end == false ) {
        int id = global_ptr_controller -> next_id();
        ct = get_time();
        std::cout << "w queuing message " << id << " @ " << ct << std::endl;
        global_ptr_controller->enqueue(id);
        int time_var = std::rand() % 10000;  
        usleep(t+time_var);
    }
    // global flag has been set, last message has been queued, tell the controller it can shutdown
    global_ptr_controller -> shut_down();
}

        
        

//message_condition.notify_one();
//std::condition_variable message_available_condition;
//std::mutex ;

void reader() {
    std::time_t ct;
    std::unique_lock<std::mutex> message_lock (message_available_mutex);
    //message_available_mutex(message_available_mutex);
    while ( the_end == false ) {
        message_available_condition.wait(message_lock);
        ct = get_time();
        std::cout << "r wakes up @ " << ct << std::endl;
        int current_queue_size = global_ptr_controller -> get_size();
        if (current_queue_size>0){
            for (int c=0; c<current_queue_size; c++){
                int id = global_ptr_controller->dequeue();
                std::cout << "r reads " << id << std::endl;
            }
            //std::cout << "r will unlock" << std::endl;
            //message_lock.unlock();
            //ct = get_time();
            //std::cout << "r done, unlocked @ " << ct << std::endl;
        } else {
            std::cout << "r woken up but queue empty" << std::endl;
        }
    }

    // we only know the that global end flag has been set, but there may still be messages available
    // wait until the controller is ready to shutdown, then empty the queue one last time
    while( global_ptr_controller -> get_shut_down_flag() == false) {}
    ct = get_time();
    std::cout << "r verified the shutdown flag @ " << ct << std::endl;
    int current_queue_size = global_ptr_controller -> get_size();
    for (int c=0; c<current_queue_size; c++){
        int id = global_ptr_controller->dequeue();
        std::cout << "r reads " << id << std::endl;
    }
    ct = get_time();
    std::cout << "r exiting @ " << ct << std::endl;
}

int main ()
{
    global_ptr_controller = new controller();

    const int SEC = 10;
    const int HEADSTART = 3;
    const int N = 2;
    const int writer_freq = 1000000/N;

    printf("the main creates a global message controller queue and a reader and a writer process\n");
    printf("the processes run for SEC secs, a watcher thread sets the global the_end flag to true\n");

    printf("the writer keeps adding messages at a slighlty varying frequency\n");
    printf("the writer frequency is long first to see the debug messages later the frequency can be increased for testing\n");
    printf("the writer process watches the end flag and stops writing new message when true\n");
    printf("the writer then calls the global controller shutdown function to signal that no new messages will be queued\n");

    printf("the global controller manages the message queue, all operations are implemented with a lockguard\n");
    printf("when a new message is queued it uses a condition variable to signal the reader\n");
    printf("the controller is not a process rather a global object - so it doesn't monitor the the_end-flag\n");
    printf("it provides a shtudown function which allows the writer to signal the end is reached\n");
    printf("it then sets a controller end flag, which if it is set to true allows the reader to empty the queue one last time and then shut down itself\n");

    printf("the reader reads messages until the queue size is 0\n");
    printf("it then sleeps and waits for the condition variable to wake it up\n");
    printf("just watching the end_flag is not enough since it could have read the queue size just before the writer queues another message\n");
    printf("hence it watches the global the_end flag and then verifies with the global controller end flag that no new messages will be queued\n");
    printf("it then empties the queue completely and then shuts down\n");
    printf("note: the reader holds the message_available_mutex during work, with several consumers this would not allow other threads to work in parallel\n");

    printf("the main program joins the processes and afterwards deletes the message controller\n");
    printf("since there is one writer and one reader the message should be queued sequentially and printed sequentially\n");
    printf("since the shutdown is synchronized all messages whould be completely printed\n");
    
    // run for a certain amount of time
    std::thread watcher_process(watcher,SEC);

    // create the writer and wait for HEADSTART seconds to fill the queue
    std::thread writer_process(writer,writer_freq);
    printf("sleeping for %i secs\n",HEADSTART);
    sleep(HEADSTART);

    // create the reader process
    std::thread reader_process(reader);
    
    // now continue to give status updates every second
    int current_queue_size;
    while (the_end == false) {
        usleep(1000000);
        current_queue_size = global_ptr_controller -> get_size();
        std::cout << "status counter = " << current_queue_size << std::endl;
    }

    current_queue_size = global_ptr_controller -> get_size();
    std::cout << "final status counter = " << current_queue_size << std::endl;

    watcher_process.join();
    writer_process.join();
    reader_process.join();
    std::cout << "processes terminated" << std::endl;
    delete global_ptr_controller;

    std::cout << "controller done too" << std::endl;

    return 0;
}

