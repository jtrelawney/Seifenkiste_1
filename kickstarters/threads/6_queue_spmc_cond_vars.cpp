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

const int N_readers(6);

// mutexes for getting the locks
std::mutex message_available_mutex[N_readers];
// cond vars to signal the wakeups
std::condition_variable message_available_condition[N_readers];
// locks cannot be global, this leads to issues at shutdown if an unlock happens on an already unlocked lock

// implement a global end flag
class end_flag_class {
private:
    bool end_flag;
    std::mutex flag_mutex;
public:
    end_flag_class():end_flag(false){ std::cout << "constructing end flag , set to false" << std::endl; };
    ~end_flag_class(){ std::cout << "destructing end flag" << std::endl; };
    void set_flag(){
        std::cout << "setting end_flag to true" << std::endl;
        std::lock_guard<std::mutex> lck (flag_mutex);
        end_flag = true;
   };
   bool read_flag(){
        std::lock_guard<std::mutex> lck (flag_mutex);
        std::cout << "reading end_flag" << end_flag << std::endl;
        return end_flag;
   };
};

end_flag_class the_end;

class controller{
private:
    int id;
    std::mutex id_mutex;
    std::mutex controller_mutex;
    bool shut_down_flag;

    std::queue<int> target_ids_q;
    std::queue<int> message_q;

    // only call when the data lock has been obtained
    void notify_first(){
        if (!message_q.empty()) {
            int next_id = target_ids_q.front();
            std::cout << "q notifies r = " << next_id << std::endl;
            message_available_condition[next_id].notify_one();
        }
    }

public:
    controller(): id(0),shut_down_flag(false){};
    virtual ~controller(){ std::cout << "q destructor" << std::endl;};
    int next_id(){
        std::lock_guard<std::mutex> lck (id_mutex);
        id++;
        return id;
    }
    void enqueue(int n, int target_id) {
        std::time_t ct = get_time();
        std::cout << "q ing = " << n << " for " << target_id << " @ " << ct << std::endl;

        // get a lock on the data
        std::lock_guard<std::mutex> lck (controller_mutex);
        std::cout << "q gets message lock and records " << n << std::endl;
        message_q.push(n);
        target_ids_q.push(target_id);
        notify_first();
    }
    int dequeue(int target_id) {
        std::time_t ct = get_time();
        std::cout << "de - q ing for " << target_id << " @ " << ct << std::endl;

        // get a lock on the data
        std::lock_guard<std::mutex> lck (controller_mutex);
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
        std::lock_guard<std::mutex> lck (controller_mutex);
        int counter = message_q.size();
        return counter;
    };
    bool is_empty() {
        std::lock_guard<std::mutex> lck (controller_mutex);
        return message_q.empty();
    };

    void shut_down() {
        std::time_t ct = get_time();
        std::cout << "q shutdown down @ " << ct << std::endl;
        
        // the writer will not send new messages, so first make sure the queue is empty
        // little problem: we don't own the lock, so choose controlled shutdown with waits 
        while (!is_empty()) {
            notify_first();
            usleep(10000);
        }

        // once the queue is empty set the flag and wake all sleeping processes so that they can see it and exit
        std::cout << "queue is empty, set flag and wake everybody" << std::endl;
        {
            std::lock_guard<std::mutex> lck (controller_mutex);
            shut_down_flag = true;
        }

        std::cout << "shutdown waking all process to ensure they see the end flag" << std::endl;
        for (int i=0; i<N_readers; i++){
            //std::unique_lock<std::mutex> lock_it(message_available_mutex[reader_index]);
            std::cout << "notify reader = " << i << std::endl;
            message_available_condition[i].notify_one();
        }
    };
    bool get_shut_down_flag() {
            std::lock_guard<std::mutex> lck (controller_mutex);
            return shut_down_flag;
    };
};

void watcher(int n) {
    std::cout << "createing watcher which stops the processes in " << n << " secs" << std::endl;
    sleep(n);
    std::cout << "watcher signals the end" << std::endl;
    the_end.set_flag();
}

void writer(unsigned int t) {
    std::time_t ct;
    while ( the_end.read_flag() == false ) {
        int id = global_ptr_controller -> next_id();
        ct = get_time();
        int target_id = std::rand() % N_readers;  

        std::cout << "w queuing message " << id << " for p = " << target_id << " @ " << ct << std::endl;
        global_ptr_controller->enqueue(id,target_id);

        int time_var = std::rand() % 10000;
        usleep(t+time_var);
    }
    // global flag has been set, last message has been queued, tell the controller it can shutdown
    ct = get_time();
    std::cout << "w calls q for shutdown @ " << ct << std::endl;
    global_ptr_controller -> shut_down();
}

void reader(int reader_index) {
    std::cout << "constructor for reader = " << reader_index << std::endl;
    std::time_t ct;
    // get lock then in loop sleep until notified
    std::cout << "r = " << reader_index << " locks its mutex" << std::endl;
    // this can't be a global lock because in the end it may be released by a different process
    std::unique_lock<std::mutex> lock_it(message_available_mutex[reader_index]);
    while(true) {
        std::cout << "r = " << reader_index << " releases its mutex and sleeps" << std::endl;
        message_available_condition[reader_index].wait(lock_it);
        // when woken up read data, unless shutdown flag is set, then break
        if (global_ptr_controller -> get_shut_down_flag() == true) break;
        ct = get_time();
        std::cout << "r = " << reader_index << " wakes up @ " << ct << std::endl;
        int id = global_ptr_controller->dequeue(reader_index);
        std::cout << "r = " << reader_index << " reads " << id << std::endl;
    }

    ct = get_time();
    std::cout << "r = " << reader_index << " verified the shutdown flag and is exiting @ " << ct << std::endl;
}

int main ()
{
    global_ptr_controller = new controller();

    const int SEC = 10;
    const int HEADSTART = 2;
    const int N = 2;
    const int writer_freq = 1000000/N;

    printf("the main creates a global message controller queue and several reader and a writer process\n");
    printf("the processes runs for SEC secs, a watcher thread sets the global the_end flag to true\n");

    printf("the writer keeps adding messages with a target_id for a specific reader at a slighlty varying frequency\n");
    printf("the writer frequency is long first to see the debug messages later the frequency can be increased for testing\n");
    printf("the writer process watches the end flag and stops writing new message when true\n");
    printf("the writer then calls the global controller shutdown function to signal that no new messages will be queued\n");

    printf("the global controller manages the message queue, all operations are implemented with a lockguard\n");
    printf("new messages are queued and then for the intended receipient of the first messgage in the queue the condition variable is notified\n");
    printf("each reader has its own condition variable (array)  and the target id is used as the index for the condition variable array\n");
    printf("when a reader tries to read the first element in the queue and it is not for the reader, -1 is returned\n");
    printf("and the condition variable of the target id is called\n");
    printf("the controller is not a process rather a global object - so it doesn't monitor the the_end-flag\n");
    printf("it provides a shutdown function which allows the writer to signal when the last message has been queued\n");
    printf("in the shutdown function the controller notifies the reader for the first message in the qeueu, until\n");
    printf("the queue is empty. Then the controller sets teh shutdonw flag and notifies all readers so that they can see the shutdown flag is set and terminate\n");

    printf("each reader has a unique integer id which is used to identify its condition variable and allows to select the correct messages\n");
    printf("when the readers gets notified and holds the lock, it first checks the shutdown flag and exits if that is the case\n");
    printf("in the loop the reader dequeues the next massage, it can assume it is for him since it was notified. if this is not the case -1 is returned\n");
    printf("if this element is for the reader the value is reutrned and the element is removed from the queue\n");

    printf("the main program joins the processes and afterwards deletes the message controller\n");
    printf("since there is one writer and one reader the message should be queued sequentially and printed sequentially\n");
    printf("since the shutdown is synchronized all messages whould be completely printed\n");

    printf("compilation with the debug and threadsnaitizer, if the interprocess communication is wrong warnings will be issued\n");
    printf("run with ./6_queue_spmc_cond_vars > test.txt 2>&1 /  to catch the warnings");
    printf("to test run with ./6_queue_spmc_cond_vars > test.txt    then compare\n");
    printf("cat test.txt | grep q ing \n");
    printf("cat test.txt | grep reads | grep -v -1\n");
    
    // run for a certain amount of time
    std::thread watcher_process(watcher,SEC);

    // create the writer and wait for HEADSTART seconds to fill the queue
    std::thread writer_process(writer,writer_freq);
    printf("sleeping for %i secs while the queue starts filling up\n",HEADSTART);
    sleep(HEADSTART);
    
    std::cout << "done sleeping starting the reader processes" << std::endl;
    std::thread reader_process[N_readers];
    for (int i=0; i<N_readers;i++) {
        reader_process[i] = std::thread(reader,i);
    }

    // now continue to give status updates every second
    int current_queue_size;
    while (the_end.read_flag() == false) {
        usleep(1000000);
        current_queue_size = global_ptr_controller -> get_size();
        std::cout << "status counter = " << current_queue_size << std::endl;
    }

    current_queue_size = global_ptr_controller -> get_size();
    std::cout << "final status counter = " << current_queue_size << std::endl;

    watcher_process.join();
    writer_process.join();
    for (int i = 0; i < N_readers; ++i) {
        reader_process[i].join();
    }

    std::cout << "processes terminated" << std::endl;
    delete global_ptr_controller;

    std::cout << "controller done too" << std::endl;

    return 0;
}

