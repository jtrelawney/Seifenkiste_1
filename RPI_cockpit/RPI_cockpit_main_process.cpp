//#include <sstream>
//#include <string.h>

#include <iostream>       // std::cout
#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>

#include "opencv2/opencv.hpp"
#include <common.h>
#include <message_class.h>
#include <message_queue_class.h>
#include <sensor_camera_class.h>
#include <tcp_client.h>


// message_queue_class.h imports the coordination variables and mutex
#include<message_queue_class.h>

// define a global queue object for all processes / objects to refer to using extern 
message_queue_class *G_MESSAGE_QUEUE_PTR;
extern end_flag_class G_END_FLAG;

void run_tcp_client(){

    std::cout << "\n\n starting TCP client" << std::endl;
    tcp_client client;
    client.set_debug_level(5);
    client.set_queue_ptr(G_MESSAGE_QUEUE_PTR);
    // dont need predicate
    //client.set_termination_predicate( []() { std::cout << "calling termination predicate " << std::endl; return G_END_FLAG.read_flag();});
    int result = client.start_up();

    if ( !(result<0) ) {
        while(G_END_FLAG.read_flag() == false) {
            std::cout << "client listening on port" << std::endl;
            // deep down this blocks until a connection is made, then it creates a receiver process, returns and a new listener blocks until the next connection arrives
            client.connect_and_receive();
            std::cout << "client returning from blocking call" << std::endl;
        }
    }
    client.shut_down();
}

void run_camera_thread(){

    address_class cockpit_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::cockpit);

    cv::namedWindow("usb_camera",1);
    std::time_t ct;

    int process_index = G_MESSAGE_QUEUE_PTR -> register_process(cockpit_addr);

    // get lock then in loop sleep until notified
    std::cout << "cockpit locks its mutex" << std::endl;
    std::unique_lock<std::mutex> lock_communication_mutex(G_QUEUE_COORDINATION_VARS_OBJ.message_available_mutex[process_index]);
    while(true) {
        std::cout << "\n\n===========================================" << std::endl;
        std::cout << "cockpit releases its mutex and sleeps" << std::endl;
        //G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[reader_index].wait(lock_communication_mutex , G_QUEUE_COORDINATION_VARS_OBJ.condition_flags[reader_index] );
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[process_index].wait(lock_communication_mutex);
        std::cout << "cockpit wakes up" << std::endl;
        // after waking up the process holds the lock
        // first check if the shutdown flag was set (message queue only sets shutdown flag if the queue is empty)
        // then check for spuriuos wakeup and process message 
        if (G_MESSAGE_QUEUE_PTR -> get_shut_down_flag() == true) break;

        if ( G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[process_index] == true ) {
            ct = get_time();
            unique_message_ptr message = G_MESSAGE_QUEUE_PTR->dequeue(cockpit_addr);
            std::cout << "\nmessage received " << message->get_id() << std::endl;
            if (message!=nullptr) {
                std::unique_ptr<cv::Mat> frame = message->fetch_data();
                message -> print_meta_data();
                cv::imshow("usb_camera", *frame);
                G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[process_index] = false;
                //test_message -> print_meta_data();
            }
            if( cv::waitKey(1) >= 0) break;
        } else {
            std::cout << "condition flag is false" << ct << std::endl;

        }
    }
    ct = get_time();
    std::cout << "\ncockpit pid = " << cockpit_addr << " verified the shutdown flag and is exiting @ " << ct << std::endl;
    bool result = G_MESSAGE_QUEUE_PTR -> deregister_process(cockpit_addr);
    std::cout << "\nresult deregistering cockpit = " << result << std::endl;
    
    cv::destroyWindow("usb_camera");
}

// start the tcp_server thread (to receive messages)
// start the global message queue (so that tcp can queue the messages)
// start the cockpit thread (so that the queue can pass on the messages)
// in the cockpit process process the messages

// the processess are coordinated through the global endflag
// the cockpit can set this global end flag when:
// - the end condition is met (i.e. run for N seconds)
// - some error has occured (e.g. it receives some error message, to be defined)
// when the endflag is set all processes should enter the shutdown state, ie.
// - they finish processing incoming messages until the messagequeue sets the shutdown flag, then end themselves
// - they commumicate their new state and shutdown themselves if possible (tcp - send a shutdown message to all connected processes and close their connections)
// - they do not create new message anymore and shutdown (sensor processes)


// the "cockpit" for the robot doesn't need a display unit
// rather it consists of sensor and processing threads which all send messages to either the cockpit via tcp or communiate with each other

int main(int argc, char *argv[])
{
    // prepare the end condition
    int N = 10;
    if ( (argc>1) && (argc<3) )  N = atoi(argv[1]);
    std::cout << "running for approx " << N << " seconds" << std::endl;

    // create global message queue and endflag
    std::cout << "\n\n\ncreating global message queue" << std::endl;
    G_MESSAGE_QUEUE_PTR = new message_queue_class();
    std::cout << "pulling in global end flag" << std::endl;
    extern end_flag_class G_END_FLAG;

    // waiting for things to catch up
    sleep(1);

    // start the tcp client to send messages to the cockpit
    std::cout << "\n\n\ncreating tcp client process " << std::endl;
    std::thread tcp_client_thread(run_tcp_client);

    // waiting for things to catch up
    sleep(1);

	// create the participating threads (sensors, processing etc)
	
    // create the camera process
    std::cout << "\n\n\ncreating camera thread" << std::endl;
    std::thread camera_thread(&run_camera_thread);

    // waiting for things to catch up
    sleep(1);

    // run for N secs then set the shutdown
    int counter(0);
    while(counter<N){
        counter++;
        std::cout << "running for " << counter << " s" << std::endl;
        sleep(1);
    }

    // all threads which are communicating via the queue object will be informed by the queue and shut down
    std::cout << "time is up, shutting down" << std::endl;
    std::cout << "request queue shutdown" << std::endl;
    G_MESSAGE_QUEUE_PTR -> request_shut_down();

    // for processes which monitor the end flag signal the shutdown
    std::cout << "set global end flag" << std::endl;
    G_END_FLAG.set_flag();

    // queue is going to inform her clients
    std::cout << "now do the queue shutdown" << std::endl;
    G_MESSAGE_QUEUE_PTR -> shut_down();


    // all other prcesses monitor the message queue state and shut down when the queue shuts down
    // wait for everybody to finish
    // order seems to be important, if waiitng for cockpit first the program doesn't terminate
    std::cout << "waiting for tcp_client_process to end" << std::endl;
    tcp_client_thread.join();
    std::cout << "waiting for camera process to end" << std::endl;
    camera_thread.join();
    
    std::cout << "processes ended" << std::endl;

    // clean up
    std::cout << "deleteting queue object" << std::endl;
    delete G_MESSAGE_QUEUE_PTR;

    std::cout << "main program done" << std::endl;

    return 0;
}
