// mutex example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

#include <unistd.h>         // usleep
//#include <chrono.h>         // usleep
#include <queue>         // queue

#include <process_communication_class.h> 

const int N=5;

extern G_QUEUE_COORDINATION_VARS_DEF G_QUEUE_COORDINATION_VARS_OBJ;

void proc(int id){
    
    std::cout << "process " << id << " registering" << std::endl;    
    G_QUEUE_COORDINATION_VARS_OBJ.register_process(id);

    std::cout << "process " << id << " locks its mutex and sleeps" << std::endl;
    std::unique_lock<std::mutex> lock_it(G_QUEUE_COORDINATION_VARS_OBJ.message_available_mutex[id]);
    while(G_QUEUE_COORDINATION_VARS_OBJ.get_shutdown_flag() == false) {
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[id].wait(lock_it);
        std::cout << "process = " << id << " wakes up" << std::endl;
        if (G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[id] == false ) {
            std::cout << "spurious wake up? or perhaps the end?" << std::endl;
        } else {
            // do something then remove the flag
            sleep(1);
            int next = id;
            while (next == id) next = random() % N;
            std::cout << "passing on to = " << next << std::endl;
            G_QUEUE_COORDINATION_VARS_OBJ.tip_shoulder(next);

            // done, remove the flag
            G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[id] = false;
        }
    }

    std::cout << "process = " << id << " has seen the end flag" << std::endl;
    sleep(1);
    std::cout << "process = " << id << " is deregistering" << std::endl;
    G_QUEUE_COORDINATION_VARS_OBJ.deregister_process(id);
}

int main ()
{

    std::cout << "creating " << N << "processes which pass a token randomly between each other" << std::endl;    
    std::cout << "after a while set the communication endflag" << std::endl;    
    std::cout << "from that point on no messages will be passed on" << std::endl;    
    std::cout << "the communication module will then wake up all processes until they deregister" << std::endl;    

    std::cout << "createing " << N << " processes" << std::endl;    
    std::thread process_list[N];
    for (int i=0; i<N;i++) {
        process_list[i] = std::thread(proc,i);
    }

    sleep(1);
    std::cout << "calling process 0" << std::endl;    
    bool result = G_QUEUE_COORDINATION_VARS_OBJ.tip_shoulder(0);
    if (result == true) std::cout << "success" << std::endl;     
    else std::cout << "failed" << std::endl;

    int T = 10;
    std::cout << "watching for " << T << "seconds" << std::endl;    
    sleep(T);

    std::cout << "setting the end flag" << std::endl;    
    G_QUEUE_COORDINATION_VARS_OBJ.set_shutdown_flag();

    std::cout << "waiting for processes to terminate" << std::endl;    
    for (int i = 0; i < N; ++i) {
        process_list[i].join();
    }

    std::cout << "main is done" << std::endl;    
}



/*
// global variable for all processes to access the queue
message_queue_class *global_ptr_message_queue = new message_queue_class();

//global variable to signal when processes should shutdown
bool the_end = false;

// thread runs a countdown and then signals the end via the global variable
void watcher(int n) {
    sleep(n);
    printf("\nwatcher : we are done\n");
    the_end = true;
}

void run_tests(){
    // create a test entry into the queue
    //message_class *t = new message_class(5);
    unique_message_ptr test(new message_class(5));
    global_ptr_message_queue -> enqueue(std::move(test));
    //int s = global_ptr_message_queue -> get_size();
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
        unique_message_ptr m = tcp.receive_message();
        if (global_ptr_message_queue!=NULL) global_ptr_message_queue->enqueue(std::move(m));
        //m->print_meta_data();
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
                unique_message_ptr m = global_ptr_message_queue->dequeue();
                int id = m->get_id();
                int data = m->get_data();
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

    std::thread w (watcher,5);
    std::thread tcp (tcp_thread,8);
    std::thread r (reader,10);

    while( !the_end){
        int s = global_ptr_message_queue -> get_size();
        printf("\nglobal : queue size = %i\n\n",s);
        sleep(1);
    }

    w.join();
    tcp.join();
    r.join();

    int s = global_ptr_message_queue -> get_size();
    printf("\nfinal global : queue size = %i\n\n",s);

    delete global_ptr_message_queue;
}
*/

