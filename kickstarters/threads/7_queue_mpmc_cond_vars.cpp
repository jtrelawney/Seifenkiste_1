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

const int N_readers(10);
//const int N_readers(6);

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
        //std::cout << "reading end_flag " << end_flag << std::endl;
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

int get_random_target(int reader_index){ int result=0; while( (result == 0) || (result == reader_index) ) result = std::rand() % (N_readers); return result;  }

class reader_class{
private:
    int reader_index;
    int write_freq;
    bool writer;
public:
    reader_class():reader_index(-1){ std::cout << "simple reader constructor" << std::endl; };
    virtual ~reader_class(){ std::cout << "reader de-structor " << reader_index << std::endl; };
    reader_class(int index): reader_index(index),write_freq(0),writer(false){
        std::cout << "reader constructor " << reader_index << std::endl;
        if (index == 0) { writer=true; write_freq = 100000/N_readers; std::cout << "I'm the troublemaker ! write_freq = " << write_freq << std::endl;   };
    };

    void read(){
        std::time_t ct;
        if (writer == true){
            while (the_end.read_flag() == false) {
                int id = global_ptr_controller -> next_id();
                int target_id = get_random_target(reader_index);
                global_ptr_controller->enqueue(id,target_id);
                usleep(write_freq);
            }
            // global flag has been set, last message has been queued, tell the controller it can shutdown
            ct = get_time();
            std::cout << "writer calls q for shutdown @ " << ct << std::endl;
            global_ptr_controller -> shut_down();
        } else {
            // get lock then in loop sleep until notified
            std::cout << "r = " << reader_index << " locks its mutex" << std::endl;
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

                // randomly respond with some result
                int rand_response = std::rand() % 10;
                if (rand_response>2) {
                    int time_var = std::rand() % 1000+500;
                    std::cout << "r = " << reader_index << " is busy for " << time_var << std::endl;
                    usleep(time_var);
                    int id = global_ptr_controller -> next_id();
                    int target_id = get_random_target(reader_index);
                    std::cout << "r = " << reader_index << " responds with " << id << " to " << target_id << std::endl;
                    global_ptr_controller->enqueue(id,target_id);
                    usleep(write_freq);
                }
            }

            ct = get_time();
            std::cout << "r = " << reader_index << " verified the shutdown flag and is exiting @ " << ct << std::endl;
        }
    };
};

int main ()
{
    printf("the main creates a global message controller queue and several readers one of which is a 'writer' process\n");
    printf("the 'writer process' regularly queues a message to a random target reader and does not receive message itself\n");
    printf("all other reader proocess sometimes create a response to a random other reader\n");
    printf("the processes run for SEC secs, a watcher thread sets the global the_end flag to true\n");

    printf("the main watches the global end flag and once it occurs it calls the message queue shutdown function\n");
    printf("it then joins the terminating processes and afterwards deletes the message controller\n");

    printf("the global controller manages the message queue, all operations are implemented with a lockguard\n");
    printf("new messages are queued and then for the intended receipient of the first messgage in the queue the condition variable is notified\n");
    printf("each reader has its own condition variable (array)  and the target id is used as the index for the condition variable array\n");
    printf("when a reader tries to read the first element in the queue and it is not for the reader, -1 is returned\n");
    printf("and the condition variable of the target id is called\n");
    printf("the controller is not a process rather a global object - so it doesn't monitor the the_end-flag\n");
    printf("it provides a shutdown function which the main calls to end the process\n");
    printf("in the shutdown function the controller notifies the reader for the first message in the qeueu, until\n");
    printf("the queue is empty. Then the controller sets teh shutdonw flag and notifies all readers so that they can see the shutdown flag is set and terminate\n");

    printf("message ids are created sequentially by the controller, so they are unique but they maybe queued out of order or displayed out of order\n");
    printf("since the shutdown is synchronized all messages should be completely printed\n");

    printf("each reader has a unique integer id which is used to identify its condition variable and allows to select the correct messages\n");
    printf("when the readers gets notified and holds the lock, it first checks the shutdown flag and starts the exit process if that is the case\n");
    printf("in its loop the reader dequeues the next massage, it can assume it is for him since it was notified. if this is not the case -1 is returned\n");
    printf("if this element is for the reader the value is reutrned and the element is removed from the queue\n");
    printf("the controller ensures that the queue will be emptied at shutdown, so it will notify the process to read their remaining message\n");
    printf("if during this process a reader wants to send a response it will be rejected (at this point the shutdown flag is not yet set)\n");
    printf("when the shutdown flag is set the queue will have been emptied so when the reader sees the flag it can exit\n");

    printf("compilation with the debug and threadsnaitizer, if the interprocess communication is wrong warnings will be issued\n");
    printf("run with ./7_queue_mpmc_cond_vars > test.txt 2>&1 /  to catch the warnings");
    printf("to test run with ./7_queue_mpmc_cond_vars > test.txt    then compare\n");
    printf("cat test.txt | grep q ing \n");
    printf("cat test.txt | grep reads | grep -v -1\n");

    global_ptr_controller = new controller();

    const int SEC = 60;
    
    // run for a certain amount of time
    std::thread watcher_process(watcher,SEC);
   
    std::vector<std::thread> reader_process;
    for (int i=0; i<N_readers; i++) {
        std::cout << "starting reader " << i << std::endl;
        // this init creates some weird destructor calls probably one for the intiializing the vector entry, then destroy, then one for this and oen destroy ....
        reader_process.push_back(std::thread(&reader_class::read,reader_class(i)) );
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
    for (int i = 0; i < N_readers; ++i) {
        reader_process[i].join();
    }
    std::cout << "processes terminated" << std::endl;
    delete global_ptr_controller;

    std::cout << "controller done too" << std::endl;
    return 0;
}

