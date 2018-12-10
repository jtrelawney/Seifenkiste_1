#include <iostream>       // std::cout

#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>

#include<common.h>
#include<message_class.h>

// message_queue_class.h imports the coordination variables and mutex
#include<message_queue_class.h>

// define a global queue object for all processes / objects to refer to using extern 
message_queue_class *G_MESSAGE_QUEUE_PTR;
// pull in the global end flag to signal the shutdown to entities that are not communicating through condition variables
extern end_flag_class G_END_FLAG;

void test_address_book(){
    address_class a1(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::cockpit);
    address_class a2(address_class::platform_type_def::pc,address_class::sensor_type_def::time_snapshot,address_class::process_type_def::tcp);
    address_class a3(address_class::platform_type_def::arduino,address_class::sensor_type_def::imu1,address_class::process_type_def::cockpit);
    address_class a4(address_class::platform_type_def::rpi,address_class::sensor_type_def::camera1,address_class::process_type_def::tcp);
    address_class a5(address_class::platform_type_def::rpi,address_class::sensor_type_def::usonic1,address_class::process_type_def::undefined);

    int id;
    bool result;

    std::cout << "registering address = " << a1 << std::endl;
    id = G_MESSAGE_QUEUE_PTR -> register_process(a1);
    std::cout << "received id = " << id << std::endl;

    std::cout << "registering address = " << a2 << std::endl;
    id = G_MESSAGE_QUEUE_PTR -> register_process(a2);
    std::cout << "received id = " << id << std::endl;

    std::cout << "registering address = " << a3 << std::endl;
    id = G_MESSAGE_QUEUE_PTR -> register_process(a3);
    std::cout << "received id = " << id << std::endl;

    std::cout << "de - registering address = " << a1 << std::endl;
    result = G_MESSAGE_QUEUE_PTR -> deregister_process(a1);
    if (result) std::cout << "success" << std::endl;
    else std::cout << "failure" << std::endl;

    std::cout << "registering address = " << a4 << std::endl;
    id = G_MESSAGE_QUEUE_PTR -> register_process(a4);
    std::cout << "received id = " << id << std::endl;

    std::cout << "registering address = " << a5 << std::endl;
    id = G_MESSAGE_QUEUE_PTR -> register_process(a5);
    std::cout << "received id = " << id << std::endl;

    std::cout << "de - registering address = " << a1 << std::endl;
    result = G_MESSAGE_QUEUE_PTR -> deregister_process(a1);
    if (result) std::cout << "success" << std::endl;
    else std::cout << "failure" << std::endl;

    std::cout << "de - registering address = " << a2 << std::endl;
    result = G_MESSAGE_QUEUE_PTR -> deregister_process(a2);
    if (result) std::cout << "success" << std::endl;
    else std::cout << "failure" << std::endl;

    std::cout << "de - registering address = " << a3 << std::endl;
    result = G_MESSAGE_QUEUE_PTR -> deregister_process(a3);
    if (result) std::cout << "success" << std::endl;
    else std::cout << "failure" << std::endl;
    std::cout << "de - registering address = " << a4 << std::endl;
    result = G_MESSAGE_QUEUE_PTR -> deregister_process(a4);
    if (result) std::cout << "success" << std::endl;
    else std::cout << "failure" << std::endl;
    std::cout << "de - registering address = " << a5 << std::endl;
    result = G_MESSAGE_QUEUE_PTR -> deregister_process(a5);
    if (result) std::cout << "success" << std::endl;
    else std::cout << "failure" << std::endl;
}


void reader(const address_class &whoami){
    std::time_t ct;
    std::cout << "registering reader with address " << whoami << std::endl;
    int myid = G_MESSAGE_QUEUE_PTR -> register_process(whoami);
    std::cout << "received id = " << myid << std::endl;

    // get lock then in loop sleep until notified
    std::cout << "reader " << myid << " locks its mutex" << std::endl;
    std::unique_lock<std::mutex> lock_it(G_QUEUE_COORDINATION_VARS_OBJ.message_available_mutex[myid]);
    while(true) {
        std::cout << "\n\n===========================================" << std::endl;
        std::cout << "reader " << myid << " releases its mutex and sleeps" << std::endl;
        G_QUEUE_COORDINATION_VARS_OBJ.message_available_condition[myid].wait(lock_it);

        ct = get_time();
        std::cout << "reader " << myid << " wakes up @ " << ct << std::endl;

        if ( G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[myid] == true) {
            unique_message_ptr test_message = G_MESSAGE_QUEUE_PTR->dequeue(whoami);
            std::cout << "\nmessage received " << std::endl;
            G_QUEUE_COORDINATION_VARS_OBJ.message_available_flag[myid] = false;
            //test_message -> print_meta_data();
        } else {
            // when woken up read data, unless shutdown flag is set, then break
            if (G_MESSAGE_QUEUE_PTR -> get_shut_down_flag() == true) {
                std::cout << "reader " << myid << " verified the shutdown flag" << std::endl;
                break;
            }
        }
    }
    ct = get_time();
    std::cout << "reader " << myid << " verified the shutdown flag and is deregistering @ " << ct << std::endl;
    bool result = G_MESSAGE_QUEUE_PTR -> deregister_process(whoami);
    if (result) std::cout << "success" << std::endl;
    else std::cout << "failure" << std::endl;
    
}

// create and queue 2 test messages  / 1. and 2. receiver given) 
void writer(const address_class &cockpit_addr, const address_class &tcp_addr){

    // test message 1
    std::cout << "writer waits for 1s and then queues message for cockpit" << std::endl;
    sleep(1);
    unique_message_ptr test_message = message_class_unit_test_create_test_message();
    test_message -> set_recipient_address(cockpit_addr);
    std::cout << "======================================================================" << std::endl;
    test_message -> print_meta_data();
    G_MESSAGE_QUEUE_PTR->enqueue(std::move(test_message));

    // test message 2
    std::cout << "writer waits for 1s and then queues message for tcp" << std::endl;
    sleep(1);
    test_message = message_class_unit_test_create_test_message();
    test_message -> set_recipient_address(tcp_addr);
    std::cout << "======================================================================" << std::endl;
    test_message -> print_meta_data();
    G_MESSAGE_QUEUE_PTR->enqueue(std::move(test_message));
    std::cout << "writer done" << std::endl;
}

int main(){

    // create global message queue
    std::cout << "creating global message queue" << std::endl;
    G_MESSAGE_QUEUE_PTR = new message_queue_class();
    G_MESSAGE_QUEUE_PTR -> set_debug_level(5);

    test_address_book();
    std::cout << "\n\n================================================================" << std::endl;

    address_class cockpit_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::cockpit);
    address_class tcp_addr(address_class::platform_type_def::pc,address_class::sensor_type_def::undefined_sensor,address_class::process_type_def::tcp);
    
    std::cout << "creating reader processs for cockpit and tcp" << std::endl;
    std::thread reader0_process(reader, cockpit_addr);
    sleep(1);
    std::thread reader1_process(reader, tcp_addr);
    sleep(1);

    std::cout << "starting writer function" << std::endl;
    writer(cockpit_addr, tcp_addr);
    
    std::cout << "waiting 2s for messages to be delivered" << std::endl;
    sleep(3);
    
    std::cout << "request queue shutdown" << std::endl;
    G_MESSAGE_QUEUE_PTR -> request_shut_down();

    std::cout << "set global end flag, not really required here, because all processes listen to the condition variables of the message queue" << std::endl;
    G_END_FLAG.set_flag();

    std::cout << "now do the queue shutdown" << std::endl;
    G_MESSAGE_QUEUE_PTR -> shut_down();
    
    reader0_process.join();
    reader1_process.join();
    std::cout << "processes ended" << std::endl;

    std::cout << "deleteting queue object" << std::endl;
    delete G_MESSAGE_QUEUE_PTR;

    std::cout << "main program done" << std::endl;

    return 0;
}
