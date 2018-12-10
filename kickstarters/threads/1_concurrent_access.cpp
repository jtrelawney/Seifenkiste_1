// mutex example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

#include <unistd.h>         // usleep

std::mutex mtx;           // mutex for critical section


class controller;

controller *global_ptr_controller;
int global_counter = 10;

class controller{
private:
    int counter;
public:
    controller(){ counter = 0; };
    void add(int n) {
        counter+=n;
        printf("adding %i   current now %i\n", n,counter);
    }
    int get() { return counter;};
};

void writer(int n, unsigned int t) {
    while (global_ptr_controller -> get() < global_counter) {
        // critical section (exclusive access to std::cout signaled by locking mtx):
        mtx.lock();
        global_ptr_controller->add(n);
        mtx.unlock();
        usleep(t);
    }
}




int main ()
{
    std::cout << "concurrency example : 2 processes increase a global counter at different frequencies and with different values" << std::endl;
    std::cout << "implemented with a global controler with global mutex - lock .." << std::endl;

    global_ptr_controller = new controller();

    std::thread th1 (writer,1,1000000);
    std::thread th2 (writer,2,2000000);

    int current = global_ptr_controller -> get();

    while (current < global_counter) {
        printf("counter = %i\n",current); 
        usleep(1000000);
        current = global_ptr_controller -> get();
    }

    printf("final counter = %i\n",current); 
    th1.join();
    th2.join();

    return 0;
}

