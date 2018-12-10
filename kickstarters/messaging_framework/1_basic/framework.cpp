// mutex example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

#include <unistd.h>         // usleep
#include <queue>         // queue


#include <message_class.h>
#include <tcp_class.h>
#include <message_queue_class.h>

#include <unistd.h>         // usleep

// global variable for all processes to access the queue
message_queue_class *global_ptr_message_queue = new message_queue_class();

//global variable to signal when processes should shutdown
bool the_end = false;

// thread runs a countdown and then signals the end via the global variable
void watcher(int n) {
    sleep(n);
    printf("watcher : wer are done\n");
    the_end = true;
}

void run_tests(){
    // create a test entry into the queue
    global_ptr_message_queue -> enqueue(5);
    int s = global_ptr_message_queue -> get_size();
    printf("queue size = %i\n",s);

    // create test message
    message_class *t = new message_class(5);
    t -> set_id(3);
    t->print_meta_data();
    delete t;

    // create test TCP
    tcp_class *tcp = new tcp_class(8192);
    tcp -> set_frequency(2);
    delete tcp;
}

void tcp_thread(int mps){

    // frequency in ms
    int f=1000000/mps;

    // connect to txp
    tcp_class tcp(8192);
    tcp.set_frequency(mps);

    //keep creating data until the end
    while (!the_end){
        message_class data = tcp.receive_message();
        if (global_ptr_message_queue!=NULL) global_ptr_message_queue->enqueue(data);
        usleep(f);
    }
}

void reader(int mps){

    // frequency in ms
    int f=1000000/mps;

    // 3seconds head start for tcp to build queue
    sleep(3);

    //keep creating data until the end
    while (!the_end){
        if (global_ptr_message_queue!=NULL) {
            int s = global_ptr_message_queue -> get_size();
            if (s>0) {
                message_class m = global_ptr_message_queue->dequeue();
                int id = m.get_id();
                int data = m.get_data();
                printf("reader : id = %i     d = %i     queue size = %i\n",id, data,s);
            }
        }
        usleep(f);
    }
}


int main ()
{

    //run_tests();

    printf("running for 10 secs\n");
    printf("a tcp object creates random messages with a given frequency and queues inot the global message queue\n");
    printf("a reader reads messages from teh queue with agiven frequqency\n");
    printf("the reader only starts after 3s when teh queue is filled up a little bit\n");
    printf("a watcher process ends the process after 10s by setting a global flag\n");
    printf("very 1s the global process lists the nubmer of messages in the queue\n");

    std::thread w (watcher,10);
    std::thread tcp (tcp_thread,4);
    std::thread r (reader,10);

    while( !the_end){
        int s = global_ptr_message_queue -> get_size();
        printf("\nglobal : queue size = %i\n\n",s);
        sleep(1);
    }

    w.join();
    tcp.join();
    r.join();

    delete global_ptr_message_queue;
}

