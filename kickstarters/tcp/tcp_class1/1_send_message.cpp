#include <tcp_class.h>
#include <tcp_client.h>

#include <thread>

#include <tcp_server.h>
#include <tcp_client.h>

void receive_messages(){

    std::cout << "\n\n starting TCP server" << std::endl;

    int result;
    tcp_server server;
    result = server.start_up();
    if ( !(result<0) ) {
        while(1) {
            std::cout << "server listening on port" << std::endl;
            server.connect_and_receive();
            std::cout << "server returning from blocking call" << std::endl;
        }
    }

    server.shut_down();
}

void send_message(tcp_client &client){

    std::cout << "enjoing the silence for 3s" << std::endl;
    sleep(3);

    client.send_message("Hallo Alter Schwede");
    client.print_status();

    sleep(3);

    client.send_message("unn tschuess machs gut");
    client.print_status();

    std::cout << "\n\ngoing to shut down in 3s" << std::endl;
    sleep(3);
}


int main()
{

    // start the server, wait and start the client
    std::cout << "\n\n now starting the server" << std::endl;
    std::thread rec (receive_messages);
    rec.detach();
    std::cout << "waitng 3s to start the client" << std::endl;
    sleep(3);
    std::cout << "\n\n now starting the client" << std::endl;
    tcp_client client;
    client.start_up();

    // send messages rournd 1
    send_message(client);

    // send messages round 2
    std::cout << "\n\nand one more round ...." << std::endl;
    send_message(client);

    // shutdown client, server will go through forced shutdown in destructor ...
    client.shut_down();
    //std::cout << "\n\nwaiting for server to shutdown ( and it will not happen in the thread ... " << std::endl;
    //rec.join();
}
