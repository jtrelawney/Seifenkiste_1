#include <tcp_class.h>
#include <tcp_client.h>

#include <thread>

#include <buffer_class.h>
#include <message_class.h>
#include <tcp_server.h>
#include <tcp_client.h>

std::ostream& operator<<(std::ostream& out, const buffer_class& buffer)  
{  
    buffer_type::const_iterator start = buffer.buffer.begin();
    buffer_type::const_iterator ende = buffer.buffer.end();
    for (auto i=start; i<ende; ++i){
        out << *i;
    }
    //buffer.print();  
    return out;  
}

std::ostream& operator<<(std::ostream& out, const message_class& mess)  
{
    return mess.myostream(out);
}

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

    message_class mess1(2);
    std::cout << mess1 << std::endl;
    mess1.print();

    std::cout << "enjoing the silence for 3s" << std::endl;
    sleep(3);

    buffer_class m1("Hallo Alter Schwede");
    client.send_message(m1);
    client.print_status();

    sleep(3);

    //buffer_class m2("was gibts \0Neues?");
    // do not use strings, but chars with the
    char fuckedupstring[] = "was gibts\0?";
    buffer_class m2(fuckedupstring,12);


    client.send_message(m2);
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
