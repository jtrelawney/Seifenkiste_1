// mutex example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

#include <unistd.h>         // usleep
#include <queue>         // queue

//std::mutex mtx;           // mutex for critical section


class controller;

static controller *global_ptr_controller;
int global_counter = 10;

class controller{
private:
    std::mutex controller_mutex;
    std::queue<int> message_q;

public:
    bool done;

    controller(){ done = false; };
    void enqueue(int n) {
        std::lock_guard<std::mutex> lck (controller_mutex);
        message_q.push(n);
    }
    int dequeue() {
        std::lock_guard<std::mutex> lck (controller_mutex);
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

void writer(int n, unsigned int t) {
    while ( global_ptr_controller -> done == false ) {
        printf("writing = %i\n",n); 
        global_ptr_controller->enqueue(n);
        usleep(t);
    }
}

void reader(unsigned int t) {
    int current = global_ptr_controller -> get();
    while ( current > 0 ) {
        int i = global_ptr_controller->dequeue();
        printf("message = %i\n",i); 

        usleep(t);
        current = global_ptr_controller -> get();
        printf("messages left = %i\n",current); 
    }
    global_ptr_controller -> done = true;
}


// 2 threads fill pipeline the thrid depletes it over time

int main ()
{

    std::cout << "concurrency example : 2 writers queue message and 1 reader sequentially prints them" << std::endl;
    std::cout << "the writers get a head start to fill the queue and the reader then empties it over time with a slighly higher frequency read" << std::endl;
    std::cout << "implemented with a global controler all queue activities are protected with a lockguard and the reader signals the end of the game with a global flag" << std::endl;
    std::cout << "this may lead to issues at the end when the reader sets the done-flag true right after a writer has read it as false and now queues another element" << std::endl;
    std::cout << "in that case the reader exits, the queue is not empty though, then the writers exit too" << std::endl;
    std::cout << "to improve the done-flag should be guarded as well" << std::endl;

    global_ptr_controller = new controller();

    std::thread th1 (writer,1,1000000);
    std::thread th2 (writer,3,1000000);
    usleep(10000000);
    std::thread th3 (reader,1000000/3);
//    std::thread th4 (reader,1,1500000);

    int current = global_ptr_controller -> get();

    while (global_ptr_controller -> done == false) {
        printf("counter = %i\n",current); 
        usleep(1000000);
        current = global_ptr_controller -> get();
    }

    printf("final counter = %i\n",current); 
    th1.join();
    th2.join();
    th3.join();
    //th4.join();

    return 0;
}

