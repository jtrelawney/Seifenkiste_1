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
    server.set_termination_predicate( []() { std::cout << "receive messages : check end flag " << std::endl; return the_end;});
    result = server.start_up();

    if ( result != 0 ) {
		std::cout << "server startup failed, stopping operatoin" << std::endl;
		server.shut_down();
		return;
	}

	// end flag set by main after N sec
	while(the_end == false) {
		std::cout << "server listening on port" << std::endl;
		server.connect_and_receive();
		std::cout << "server returning from blocking call" << std::endl;
	}

    server.shut_down();
}

int main()
{
    // start the server, wait and start the client
    std::cout << "\n\n now starting the server" << std::endl;
    std::thread rec (receive_messages);
    std::cout << "running for 10 s" << std::endl;
    sleep(10);
    the_end = true;
    rec.join();
}
