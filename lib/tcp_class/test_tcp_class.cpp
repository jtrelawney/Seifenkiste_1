#include <thread>

#include <buffer_class.h>
#include <message_class.h>

#include <tcp_class.h>
#include <tcp_client.h>
#include <tcp_server.h>

// endflag will be set right after sending the message
// the tcp server gets a predicate which checks for the_end
// if the server receives the message and checks the predicate before the test function can set the flag the tcp server will block waiting for another connection
// in receive messages the flag is also checked
// this concept only works if the endflag will be set before the server receives the message
// in real life the predicate will be set to test the message queue status and then the message queue can safely shutdown all processes
bool the_end=false;

void receive_messages(){

    std::cout << "\n\n starting TCP server" << std::endl;

    int result;
    tcp_server server;
    server.set_debug_level(5);    
    server.set_termination_predicate( []() { std::cout << "test test " << std::endl; return the_end;});
    result = server.start_up();

    if ( !(result<0) ) {
        while(the_end == false) {
            std::cout << "server listening on port" << std::endl;
            server.connect_and_receive();
            std::cout << "server returning from blocking call" << std::endl;
        }
    }
    server.shut_down();
}

void send_message(tcp_client &client){

    // set addresses, but they are not going to be routed
    address_class::platform_type_def platform;
    address_class::sensor_type_def sensor;
    address_class::process_type_def process;

    platform = address_class::platform_type_def::pc;
    sensor = address_class::sensor_type_def::undefined_sensor;
    process = address_class::process_type_def::cockpit;
    address_class cockpit_addr(platform,sensor,process);

    platform = address_class::platform_type_def::pc;
    sensor = address_class::sensor_type_def::undefined_sensor;
    process = address_class::process_type_def::tcp_client;
    address_class tcp_addr(platform,sensor,process);

    // create the message
    std::cout << "create a test message" << std::endl;
    unique_message_ptr test_message = message_class_unit_test_create_test_message(20,50,3,100);
    test_message -> set_recipient_address(cockpit_addr);
    std::cout << "in tcp _test : send message we have a test message : " << std::endl;
    test_message -> print_meta_data();

    // send the message
    std::cout << "send the test message" << std::endl;
    client.send_message(std::move(test_message));
    std::cout << "tcp: message sent, set the end-flag, this may not reach the server if setting the flag happens after receive" << std::endl;
    the_end = true;
    client.print_status();
}

int main()
{
    // start the server, wait and start the client
    std::cout << "\n\n now starting the server" << std::endl;
    std::thread rec (receive_messages);
    //rec.detach();
    std::cout << "waiting 3s to start the client" << std::endl;
    sleep(3);
    std::cout << "\n\n now starting the client" << std::endl;
    tcp_client client;
    client.start_up();

    sleep(1);    
    send_message(client);

    std::cout << "waiting 3s and then shut down" << std::endl;
    sleep(3);

    // send messages round 2
    //std::cout << "\n\nand one more round ...." << std::endl;
    //send_message(client);

    // shutdown client, server will go through forced shutdown in destructor ...
    client.shut_down();
    rec.join();
    //std::cout << "\n\nwaiting for server to shutdown ( and it will not happen in the thread ... " << std::endl;
    //rec.join();
}
