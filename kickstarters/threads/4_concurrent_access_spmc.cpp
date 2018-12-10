// mutex example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

#include <unistd.h>         // usleep
#include <queue>         // queue



class controller;

static controller *global_ptr_controller;
int global_counter = 10;

std::mutex end_mutex;           // mutex for critical section
bool the_end = false;

class controller{
private:
    int id;
    std::mutex controller_mutex;
    std::mutex id_mutex;
    std::queue<int> message_q;

public:
    controller() : id(0){ };
    int next_id(){
        std::lock_guard<std::mutex> lck (id_mutex);
        id++;
        return id;
    }
    void enqueue(int n) {
        std::lock_guard<std::mutex> lck (controller_mutex);
        message_q.push(n);
    }
    int dequeue() {
        std::lock_guard<std::mutex> lck (controller_mutex);
        if (message_q.empty()) return -1;
        int n=message_q.front();
        message_q.pop();
        return n;        
    }
    int get() {
        std::lock_guard<std::mutex> lck (controller_mutex);
        int counter = message_q.size();
        //printf("get counter = %i\n",counter); 
        return counter;
    };
};

void watcher(int n) {
    sleep(n);
    end_mutex.lock();
    the_end = true;
    printf("\nsetting global end flag\n");
    end_mutex.unlock();
}

void writer(unsigned int t) {

    end_mutex.lock();
    while ( the_end == false ) {
        int id = global_ptr_controller -> next_id();
        global_ptr_controller->enqueue(id);
        end_mutex.unlock();
        usleep(t);
        end_mutex.lock();
    }
}

void reader(int rid, unsigned int t) {
    while (  ( the_end == false ) || ( global_ptr_controller-> get()>0 ) ) {
        int id = global_ptr_controller->dequeue();
        printf("message id processed = %i\n",id);
        //int count = global_ptr_controller -> get();
        //printf("messages left = %i\n",count);
        usleep(t); 
    }
    printf("reader %i saw the end flag\n",rid);
}


// 1 thread produces, seberal consume
// 1 thread watches timer and sets global end variable

int main ()
{
    global_ptr_controller = new controller();

    const int N=10;
    const int s=3;

    printf("the process is running for 10 secs, a watcher thread sets the global the_end flag to true\n");
    printf("a writer gets 3 seconds to fill the queue with increasing and unqiue message ids\n");
    printf("after 3 secs N readers start dequeuing messages, one message per sec\n");
    printf("while the writer keeps adding messages at a reate of N messages/s\n");
    printf("the global process lists the nubmer of messages in the queue\n");
    printf("the #messages should flcutuate slighlty arround the original fill level\n");
    printf("while the message ids will be removed in sequential order\n");
    printf("since the writer adds them in order and the readers remove in FIFO\n");
    printf("however they may not be printed sequentially because the readers don't acquire a lock on cout\n");
    printf("again implemented as a global controler with lockgauard on all operations and in\n");
    printf("addition the global end flag is used (with mutex to avoid new queue activities at the end of the watch)\n");
    printf("the main program prints the current queueu fill when it sees the endflag and since the get\n");
    printf("function is used the rest of the count should match it\n");
    printf("\ndrawback is that the processes wait for the locks function is used the rest of the count should match it\n");

    std::thread w (watcher,10);
    std::thread tcp (writer,1000000/N);
    printf("\nsleeping for %i secs while the buffer is being filled\n",s);
    sleep(s);

    std::thread t[N];

    for (int i=0; i<N;i++) {
        t[i] = std::thread(reader,i, 1000000);
    }

    int count;

    while (the_end == false) {
        usleep(1000000);
        count = global_ptr_controller -> get();
        printf("counter = %i\n",count);
    }

    count = global_ptr_controller -> get();
    printf("final counter = %i\n",count); 
    w.join();
    tcp.join();
    for (int i = 0; i < N; ++i) {
        t[i].join();
    }

    return 0;
}

