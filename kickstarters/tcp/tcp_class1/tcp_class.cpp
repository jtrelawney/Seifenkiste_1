#include <tcp_client.h>


#include <iostream>
#include <cstdio>
#include <stdio.h>

// creates a socket at the given port, binds and listens
// sets the internal socketfd for the connect
int tcp_class::TCP_bind(){

    // assume result ok
    int result = 0;

    // prepare the ...
    bzero((char *) &server_socket_info, sizeof(server_socket_info));
    server_socket_info.sin_family = AF_INET;
    server_socket_info.sin_addr.s_addr = INADDR_ANY;
    server_socket_info.sin_port = htons(port_number);

    //std::cout << "binding to socket, ready to listen" << std::endl;
    result = bind(socket_fd, (struct sockaddr *) &server_socket_info, sizeof(server_socket_info));
    if (result < 0) {
        tcp_state = bind_call_failed; 
        if (debug_level>0) std::cout << "tcp server : bind call failed" << std::endl;
        perror("bind");
    }
    else {
        tcp_state = bind_call_successful;
        if (debug_level>0) std::cout << "tcp server : bind call succesful" << std::endl;
    }
    return result;
}

int tcp_class::TCP_listen(){

    // assume result ok
    int result = 0;

    std::cout << "starting to listen to socket" << std::endl;
    result = listen(socket_fd,5);
    if (result < 0) {
        tcp_state = listen_call_failed; 
        if (debug_level>0) std::cout << "tcp server : listen call failed" << std::endl;
    }
    else {
        tcp_state = listen_call_succesful;
        if (debug_level>0) std::cout << "tcp server : listen call succesful" << std::endl;
    }

    return result;
}

int tcp_class::check_buffer(){

    char test_buffer = {0};
    int result = read(accepted_connection_fd, &test_buffer, 0);

    if (result == 0){
        if (debug_level>2) std::cout << "tcp class : test message buffer == 0 , is this EOF" << std::endl;
        return 0;
    }

    if (result < 0){
        std::cout << "tcp class : test message buffer < 0 , what is the error" << std::endl;
        error("error testing message buffer");
        return result;
    }

    return 1;
}

// read the next header message from TCP
// may encounter issues with partiallly transmitted headers, i.e. when the TCP buffer only contains part of the header and the rest is being transmitted
int tcp_class::TCP_receive_message(char **header_buffer){

    /*
    int check = check_buffer();
    if (check == 0){
        if (debug_level>2) std::cout << "tcp class : test buffer - returned = 0 , is this EOF" << std::endl;
        return 0;
    }
    */

    // read header from buffer and validate its length
    bzero(*header_buffer, TCP_MAX_BUFFER_SIZE);
    int header_length = read(accepted_connection_fd, *header_buffer, TCP_MAX_BUFFER_SIZE);
    header_buffer[header_length]=0;

    if (header_length == 0){
        std::cout << "tcp class : receive message - read returned length == 0 , is this EOF" << std::endl;
        return 0;
    }

    if (header_length < 0){
        if (debug_level > 1) std::cout << "tcp class : receive message - message length < 0, some error" << std::endl;
        perror("error receiving tcp");

        exit(0);
        
        return header_length;
    } else {
        if (debug_level > 1) std::cout << "tcp class : received message - length = " << header_length << std::endl;
        //error("ERROR reading message header from connection");
    }

    return header_length+1;
}

// connect to the previously initialized socket and return the fd
int tcp_class::TCP_accept_connection(){

    unsigned int client_info_len = sizeof(client_socket_info);
	int new_connection_fd = accept(socket_fd, (struct sockaddr *) &client_socket_info, &client_info_len);
	if (new_connection_fd < 0) {
        if (debug_level>-1) std::cout << "tcp class : accept connection - failed" << std::endl;
        return -1;
    }

    accepted_connection_fd = new_connection_fd;
    if (debug_level>-1) std::cout << "tcp class : accept connection succeeded - fd = " << accepted_connection_fd << std::endl;

    return new_connection_fd;
}

int tcp_class::TCP_create_socket(int portno){

    // store the portnumber
    port_number = portno;

    // assume failure
    socket_fd = -1;

    // grab
    if (tcp_state == tcp_initialized) {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) tcp_state = socket_error;
        else tcp_state = socket_created;		
    } else {
        if (debug_level>-1) std::cout << "tcp class : create socket attempt on tcpclass object which is not in an initialized state" << std::endl;
        tcp_state = socket_error;
    }
    return socket_fd;
}

void tcp_class::print_n(char *data, int n)
{
   for (int i=0; i<n; i++){
		fprintf(stderr, "%c   %i\n",data[i], (int)data[i]);
	}
   fprintf(stderr, "\n");
}

void tcp_class::error(const char *msg)
{
    perror(msg);
    exit(0);
}

tcp_class::tcp_class(){
	tcp_state = tcp_initialized;
    socket_fd = -1;
    accepted_connection_fd = -1;
    if (debug_level>0) std::cout << "tcp class : constructor call" << std::endl;
}

void tcp_class::print_status(int type){

    // param 1 = print fd
    if (type == 0 ) {
    
    std::cout << "tcp_class : print status information with param " << type << std::endl;
	switch(tcp_state){
		
		case tcp_initialized:
  			std::cout << "tcp_class : status = TCP object initialized" << std::endl;
            break;
        case socket_created:
  			std::cout << "tcp_class : status = TCP socket created, port = " << port_number << std::endl;
			break;
		case socket_error:
			std::cout << "tcp_class : status = failed to create socket" << std::endl;
			break;
		case gethost_successful:
			std::cout << "tcp_class : status = host information resolved" << std::endl;
			break;
		case gethost_error:
			std::cout << "tcp_class : status = failed to resolve host information" << std::endl;
			break;
		case tcp_connect_successful:
			std::cout << "tcp_class : status = connection established to remote host" << std::endl;
			break;
		case tcp_connect_error:
			std::cout << "tcp_class : status = error connecting to remote host" << std::endl;
			break;
		case bind_call_failed:
			std::cout << "tcp_class : status = bind call failed" << std::endl;
			break;
		case bind_call_successful:
			std::cout << "tcp_class : status = bind call succesful" << std::endl;
			break;
		case listen_call_failed:
			std::cout << "tcp_class : status = listen call failed" << std::endl;
			break;
		case listen_call_succesful:
			std::cout << "tcp_class : status = listen call succesful" << std::endl;
			break;
		default:
			std::cout << "tcp state is ok" << std::endl;
	}
    }

    // param 1 = print fd
    if (type ==1 ) {
			std::cout << "tcp_class : socket fd = " << socket_fd << std::endl;
			std::cout << "tcp_class : accepted_connection_fd = " << accepted_connection_fd << std::endl;
    }
}

int tcp_class::get_host_info(const char *server_addr){

    // assume success	    
    int result = 0;
    if (debug_level>1) printf("tcp_class : gethostname for server address = %s\n",server_addr);

	// use ip address to lookup information about the host
	host_info = gethostbyname(server_addr);
    if (host_info == NULL) {
        if (debug_level>0) std::cout << "tcp_class : error resolving host information for " << server_ip_address << std::endl;
        tcp_state = gethost_error;
        result = -1;
    }   else {
        if (debug_level>0) std::cout << "tcp_class : host information resolved" << std::endl;
        tcp_state = gethost_successful;
    }    
    
    // prepare information about the server socket
	bzero((char *) &server_socket_info, sizeof(server_socket_info));

    //fprintf(stderr,"server info\n");
    //char *h_add = server->h_addr;
    //fprintf(stderr,"server address = %s\n",h_add);

    // define protocol, serveraddress and port number
    // bcopy copies arg1 onto arg2 using length arg3, copy is correct even with overlap
	server_socket_info.sin_family = AF_INET;
	bcopy((char *)host_info->h_addr, (char *)&server_socket_info.sin_addr.s_addr, host_info->h_length);
	server_socket_info.sin_port = htons(port_number);

    if (debug_level>1) printf("tcp_class : socket info    %u\n",server_socket_info.sin_addr.s_addr);
    if (debug_level>1) printf("tcp_class : socket info    %i\n",server_socket_info.sin_port);	
	
    return result;
}

int tcp_class::TCP_connect(std::string server_ip){

    // assume success
    int result = 0;

    server_ip_address = server_ip;
    const char *server_addr = server_ip_address.c_str();
    if (debug_level>0) std::cout << "tcp_class : using server ip address = " << server_addr << std::endl;

    result = get_host_info(server_addr);
    if (result<0) return -1;

	result = connect(socket_fd,(struct sockaddr *)&server_socket_info,sizeof(server_socket_info));
	if (result < 0) {
        if (debug_level>0) std::cout << "tcp_class : error connecting to server, addr = " << server_ip_address << std::endl;
		tcp_state = tcp_connect_error;
        perror("connection");
		return -1;
	} else {
        if (debug_level>0) std::cout << "tcp_class : successfully connected to server, addr = " << server_ip_address << std::endl;
		tcp_state = tcp_connect_successful;
		return -1;
    }

	std::cout << "connection ready" << std::endl;
		
   return result;

    /*
	int result = TCP_connect();	
	if (tcp_state==connect_error) {
		process_error();
	}
    */
    
    /*
    connect_to_socket();
	if (tcp_state==connect_error) process_error(); else tcp_state = connection_ready;
	
	if (tcp_state==connection_ready)
		std::cout << "connected to " << server_addr << " on port " << portno << " established" << std::endl;
	*/

}

void tcp_class::process_error(){
	
	return;
	
	switch(tcp_state){
		
		case tcp_initialized:
			tcp_class::error("ERROR TCP not initialized");
			break;
		case socket_error:
			tcp_class::error("ERROR opening socket");
			break;
		case gethost_error:
			tcp_class::error("ERROR, no such host");
			break;
		case tcp_connect_error:
			tcp_class::error("ERROR, can't connect to port");
			break;
		default:
			std::cout << "tcp state is ok" << std::endl;
	}
}


int tcp_class::TCP_send_message(const char *buffer, int buffer_length){
	
    //printf("TCP - transmitting buffer, length = %lu\n", buffer_length);
    std::cout << "tcp_class : TCP - transmitting buffer, length = " << buffer_length << std::endl;

	if (tcp_state!=tcp_connect_successful) {
		std::cout << "TCP socket not ready for a connection" << std::endl;
		return -1;
	}
	
    std::cout << "connection to " << server_ip_address << " on port " << port_number << " is ready" << std::endl;
    
    int n = write(socket_fd,buffer,buffer_length);
    if (n < 0) 
         error("ERROR writing buffer to socket");
         
    return n;
}

int tcp_class::TCP_close_socket(){

    if (socket_fd<0) {
        // fd < 0 - probably has been closed before -> we are done
        std::cout << "tcp_class : shutdown socket_fd , socket fd < 0" << std::endl;
        return -2;
    }

    int result = shutdown(socket_fd,1);   // parameter 1 = write functionality
    if (result<0) {
        std::cout << "tcp_class : shutdown socket_fd failed, result = " << result << std::endl;
        error("tcp shutdown failed");
        return result;
    } else {
        std::cout << "tcp_class : shutdown socket_fd succesful" << std::endl;
    }

    sleep(1);

    result = close(socket_fd);
    if (result<0) {
        std::cout << "tcp_class : close socket_fd failed, result = " << result << std::endl;
        return result;
    } else {
        std::cout << "tcp_class : close socket_fd succesful" << std::endl;
        socket_fd = -1;
    }

    return result;
}

int tcp_class::TCP_close_client_session_socket(){

    if (accepted_connection_fd<0) {
        // fd < 0 - probably has been closed before -> we are done
        std::cout << "tcp_class : shutdown accepted_connection_fd , accepted_connection_fd < 0" << std::endl;
        return -2;
    }

    int result = shutdown(accepted_connection_fd,1);   // parameter 1 = write functionality
    if (result<0) {
        std::cout << "tcp_class : shutdown accepted_connection_fd failed, result = " << result << std::endl;
        error("tcp shutdown failed");
        return result;
    } else {
        std::cout << "tcp_class : shutdown accepted_connection_fd succesful" << std::endl;
    }

    sleep(1);

    result = close(accepted_connection_fd);
    if (result<0) {
        std::cout << "tcp_class : close accepted_connection_fd failed, result = " << result << std::endl;
        return result;
    } else {
        std::cout << "tcp_class : close accepted_connection_fd succesful" << std::endl;
        accepted_connection_fd = -1;
    }

    return result;
}

/*
int tcp_class::TCP_close(){

    //assume failure
    int result=-1;
    
    result = close_accepted_connection_socket();
    if (result<0) {
        std::cout << "tcp_class : accepted connection socket - shutdown/close failed" << std::endl;
        tcp_state = TCP_close_failed;
        return result;
    }
    
    result = close_socket();
    if (result<0) {
        std::cout << "tcp_class : socket - shutdown/close failed" << std::endl;
        tcp_state = TCP_close_failed;
        return result;
    }

    tcp_state = TCP_close_successful;
    return result;
}
*/

tcp_class::~tcp_class(){
    std::cout << "\n\ntcp_class : destructor call" << std::endl;
    TCP_close_socket();
    TCP_close_client_session_socket();
}

/*

int tcp_client::send_message(message_class &message){
	
	//create header
	char *header = (char *) malloc(sizeof(char) * TCP_HEADER_LENGTH);
	int result = message.create_TCP_header(&header[0]);
	
	for (int i=0; i<TCP_HEADER_LENGTH; i++){
		char c = header[i];
		int ci = int(c);
		char sc = char(c);
		unsigned char uc = (unsigned char) (c);
		printf("%i   %c   %u   %c   %i\n",i, c, uc, sc, ci);
	}
	
	int send_result;
	send_result = send(header,TCP_HEADER_LENGTH);
	if (send_result<0) {
		std::cout << "error sending buffer" << std::endl;
		return -2;
	}
	
	std::cout << "sent header , " << send_result << " bytes" << std::endl;
	
	
	// next send the image data
	unsigned int data_length = message.get_data_length();
	std::cout << "going to send data , " << data_length << " bytes" << std::endl;
	char* data_buffer = message.get_data_buffer_ptr();
	send_result = send(data_buffer,data_length);
	if (send_result<0) {
		std::cout << "error sending buffer" << std::endl;
		return -2;
	}

	std::cout << "sent data , " << send_result << " bytes" << std::endl;
	
	std::cout << "\n\n\nimplement the process response\n\n\n" << std::endl;
	
	return 0;
}

*/

/*
int tcp_client::send_test_image(char *image, unsigned long imagelen){

	int length;
	//char *thebytes;

	// collect data for the test
	
	//sender = rpi
	sender_type_def sender = rpi;
	std::cout << "semder " << sender << std::endl;
	//  time sent = local time	
	unsigned long current_time = get_timestamp();
	printf("message time = %lu\n",current_time);
	// sensor platform
	sender_type_def sensor_platform = rpi;
	// sensor type
	sensor_type_def sensortype= camera1;
	// sensor time
	unsigned long sensor_time = get_timestamp()-1000;
	printf("sensor time = %lu\n",sensor_time);
	// data length
	unsigned int data_length = imagelen;
	// ptr to databuffer
	char *data_buffer = image;

	// initialize a new message
	message_class test_message(sender, current_time, sensor_platform, sensortype, sensor_time, data_length);
	test_message.set_id(256);
	test_message.print_meta_data();
	
	//create header
	int header_length = 25;
	char *header = (char *) malloc(sizeof(char) * 26);
	memset(&header[0],0,26);
	int result = test_message.create_TCP_header(&header[0]);
	
	for (int i=0; i<26; i++){
		char c = header[i];
		int ci = int(c);
		printf("%i   %i   %c\n",i,ci,c);
	}
	
	int send_result;
	send_result = send(header,header_length);
	if (send_result<0) {
		std::cout << "error sending buffer" << std::endl;
		return -2;
	}
	
	std::cout << "sent " << send_result << " bytes" << std::endl;

	// next send the image data
	send_result = send(data_buffer,data_length);
	if (send_result<0) {
		std::cout << "error sending buffer" << std::endl;
		return -2;
	}

	std::cout << "sent " << send_result << " bytes" << std::endl;
	
	std::cout << "\n\n\nimplement the process response\n\n\n" << std::endl;
	
	return 0;
}
*/
		







/* delete, this is in message class

//takes pointer to char and creates the necessary memory space
//the caller has to release the space
int tcp_client::create_header(char **sessioninfo, unsigned long imagelen){

    unsigned int infolength = 3*sizeof(char) + sizeof(unsigned long) + sizeof(char);
    fprintf(stderr, "\n\ncreating info message, length = %i\n",infolength);

    char *info = (char *)malloc(infolength);//[infolength];
    info[0]='c';
    info[1]='o';
    info[2]='n';
    memcpy(&info[3],&imagelen,sizeof(imagelen));
    info[infolength-1]='h';

    unsigned long test;
    memcpy(&test,&info[3],sizeof(unsigned long));
    fprintf(stderr, "test = %lu\n",test);
    
    *sessioninfo=&info[0];
    print_n(info,8);
    print_n(*sessioninfo,8);
    //print_n((unsigned char *) &test,4);
    
    return infolength;
}
*/


