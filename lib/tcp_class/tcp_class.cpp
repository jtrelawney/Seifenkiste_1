#include <tcp_client.h>

// tcp_debug_level_ reference
// 0 - no output
// 1 - only errors / warnings
// 2 - high level activity
// 3 - details


// initialize the object, only sets the members, no action (for that call startup()
tcp_class::tcp_class() :
    error_state_(error_class(false,"ok")),
    tcp_state_(tcp_initialized),
    port_number_(-1),
    socket_fd_(-1),
    server_ip_address_(std::string()),
    accepted_connection_fd(-1),
    tcp_debug_level_(TCP_DEBUG_LEVEL)
{
    if (tcp_debug_level_>1) std::cout << "tcp class : constructor call" << std::endl;
}

tcp_class::~tcp_class()
{
    if (tcp_debug_level_>1) std::cout << "\n\ntcp_class : destructor call" << std::endl;
    TCP_close_socket();
    TCP_close_client_session_socket();
    if (tcp_debug_level_>1) std::cout << "tcp_class : destructor call completed" << std::endl;
}

// for both client and server - creates the local tcp end point
// creates a socket at a given portnumber
// if successfull the fd and the portnumber are stored and the status changes to socket_created
// if the state is not "initiliazed then it means that the socket exists already or some other error has occured
// handling of the former is not yet implmeneted, latter leads to status socket_error
// returns the new fd
int tcp_class::TCP_create_socket(const int &portno)
{
    // check if status is acceptable to create a socket, which is the first thing to be done during statup
    bool ready_to_create_socket = ( (tcp_state_ == tcp_initialized) && (socket_fd_ == -1) && (port_number_ == -1) );
    if ( !(ready_to_create_socket) ) {
        if (tcp_debug_level_> 0) {
            std::cout << "tcp class : create socket call : called in wrong state" << std::endl;
            TCP_print_status();
            TCP_print_fd_info();
            std::cout << "tcp class : create socket call : port_number = " << port_number_ << std::endl;
        }
        std::cout << "TODO : MANAGE STATE IN CASE OF EXISTING CONNECTION/ SOCKET" << std::endl;
        tcp_state_ = socket_error;
        return -1;
    }
    
    // state found to be acceptable to create a socket
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        tcp_state_ = socket_error;
        if (tcp_debug_level_> 0) std::cout << "tcp class : create socket : attempt to create a socket failed" << std::endl;
    } else {
        tcp_state_ = socket_created;
        port_number_ = portno;
        if (tcp_debug_level_> 0) std::cout << "tcp class : create socket : attempt to create a socket succesful" << std::endl;
    }
    return socket_fd_;
}

// closes the local socket
// for that the socket_fd needs to exist and an existing connection needs to be shtudown first before the socket can be closed
int tcp_class::TCP_close_socket(){

    // fd < 0 - socket probably has never been opened or has been closed before
    if (socket_fd_< 0) {
        if (tcp_debug_level_> 0) std::cout << "tcp_class : close_socket : current socket fd < 0 -> it is closed already" << std::endl;
        return -2;
    }

    // try to shutdown before closing the socket
    if (tcp_debug_level_> 0) std::cout << "tcp_class : close socket : trying to shutdown socket now" << std::endl;
    int result = shutdown(socket_fd_,1);   // parameter 1 = write functionality
    if (result<0) {
        if (tcp_debug_level_> 0) std::cout << "tcp_class : close socket : tcp.shutdown socket_fd_ failed, result = " << result << std::endl;
        return result;
    } else {
        if (tcp_debug_level_> 0) std::cout << "tcp_class : close socket : shutdown socket_fd_ succesful" << std::endl;
    }

    sleep(1);

    result = close(socket_fd_);
    if (result<0) {
        if (tcp_debug_level_> 0) std::cout << "tcp_class : close socket : close socket_fd_ failed, result = " << result << std::endl;
        return result;
    } else {
        if (tcp_debug_level_> 0) std::cout << "tcp_class : close socket : close socket_fd_ succesful, set status = initialized" << std::endl;
        // reset the status completely
        socket_fd_ = -1;
        port_number_ = -1;
        tcp_state_ = tcp_initialized;
    }

    return result;
}

// this is a client function
// first the ip address is resolved
// then the connection is established
// this returns the result of the connect call, if ok the state is set to tcp_connect_successfull
// failure is either an issue to resolve the ip address or during connect
// this currently doesn't check if the fd has been created
int tcp_class::TCP_connect(std::string server_ip){

    server_ip_address_ = server_ip;
    const char *server_addr = server_ip_address_.c_str();
    if (tcp_debug_level_>0) std::cout << "tcp_class : connect : using server ip address = " << server_ip_address_ << std::endl;

    int result = get_host_info(server_addr);
    if (result<0) {
        if (tcp_debug_level_>0) std::cout << "tcp_class : connect : get_host_info call failed for ip = " << server_ip_address_ << std::endl;
        tcp_state_ = gethost_error;
        return -1;
    }

	result = connect(socket_fd_,(struct sockaddr *)&server_socket_info, sizeof(server_socket_info));
	if (result < 0) {
        if (tcp_debug_level_>0) std::cout << "tcp_class : connect : error connecting to server, addr = " << server_ip_address_ << std::endl;
		tcp_state_ = tcp_connect_error;
		return -1;
	} else {
        if (tcp_debug_level_>0) std::cout << "tcp_class : connect successfully connected to server, addr = " << server_ip_address_ << std::endl;
		tcp_state_ = tcp_connect_successful;
		return result;
    }

	if (tcp_debug_level_>1)  std::cout << "tcp_class : connect : connection ready" << std::endl;
    return result;
}

// this sends a given buffer over an established connection
// the buffer implements acess to char*, length (and could be any data object e.g. header, sensor data etc)

//int tcp_class::TCP_send_buffer(const buffer_class &buffer){
int tcp_class::TCP_send_buffer(buffer_class &&buffer){
//int tcp_class::TCP_send_buffer(buffer_class buffer){

    int target_length = buffer.get_data_size();
    char *data_ptr = buffer.get_data_ptr();
    if (tcp_debug_level_>1) std::cout << "tcp_class : send_buffer : transmitting buffer_class object, length = " << target_length << std::endl;
    if (target_length <= 0) {
        std::cout << "tcp class : send buffer : message buffer length " << target_length << " too small to be sent as a message" << std::endl;
        return -1;
    }

	if (tcp_state_!=tcp_connect_successful) {
		if (tcp_debug_level_>0) std::cout << "tcp_class : send_buffer : TCP socket not ready for a connection" << std::endl;
		return -1;
	}
	
    if (tcp_debug_level_>1) std::cout << "tcp_class : send buffer : connection to " << server_ip_address_ << " on port " << port_number_ << " is ready" << std::endl;
   
    int n = write(socket_fd_, data_ptr, target_length);
    if (n < 0) {
        if (tcp_debug_level_>0) std::cout << "tcp_class : send buffer : error writing buffer to socket" << std::endl;
        return n;
    }

    if (n < target_length) {
        if (tcp_debug_level_>0) std::cout << "tcp_class : send buffer : of " << target_length << " bytes only written " << n << std::endl;
        std::cout << "tcp_class : send buffer : implement case when fewer bytes are written than requested" << std::endl;
        return -2;
    }
    return n;
}

int tcp_class::TCP_close_client_session_socket(){

    if (accepted_connection_fd<0) {
        // fd < 0 - probably has been closed before -> we are done
        std::cout << "tcp_class : shutdown accepted_connection_fd , accepted_connection_fd < 0, probably shutdown already" << std::endl;
        return -2;
    }

    int result = shutdown(accepted_connection_fd,1);   // parameter 1 = write functionality
    if (result<0) {
        std::cout << "tcp_class : shutdown accepted_connection_fd failed, result = " << result << std::endl;
        return result;
    } else {
        std::cout << "tcp_class : shutdown accepted_connection_fd successful" << std::endl;
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

// creates a socket at the given port, binds and listens
// sets the internal socketfd for the connect
int tcp_class::TCP_bind(){

    // assume result ok
    int result = 0;

    // prepare the ...
    bzero((char *) &server_socket_info, sizeof(server_socket_info));
    server_socket_info.sin_family = AF_INET;
    server_socket_info.sin_addr.s_addr = INADDR_ANY;
    server_socket_info.sin_port = htons(port_number_);

    //std::cout << "binding to socket, ready to listen" << std::endl;
    result = bind(socket_fd_, (struct sockaddr *) &server_socket_info, sizeof(server_socket_info));
    if (result < 0) {
        tcp_state_ = bind_call_failed; 
        if (tcp_debug_level_>0) std::cout << "tcp server : bind call failed" << std::endl;
    }
    else {
        tcp_state_ = bind_call_successful;
        if (tcp_debug_level_>0) std::cout << "tcp server : bind call succesful" << std::endl;
    }
    return result;
}

int tcp_class::TCP_listen(){

    // assume result ok
    int result = 0;

    std::cout << "starting to listen to socket" << std::endl;
    result = listen(socket_fd_,5);
    if (result < 0) {
        tcp_state_ = listen_call_failed; 
        if (tcp_debug_level_>0) std::cout << "tcp server : listen call failed" << std::endl;
    }
    else {
        tcp_state_ = listen_call_succesful;
        if (tcp_debug_level_>0) std::cout << "tcp server : listen call succesful" << std::endl;
    }

    return result;
}

// receive a tcp message with a buffer provided, i.e. the length and the data ptr is given
// return codes
// -1 : server not in listening status
// -2 : requested buffer data length too small (<0)
// -3 : message length received < 0 : tcp error
// -4 : message length received = 0 : has client terminated the connection?
// > 0: number of bytes received

int tcp_class::TCP_receive_buffer(buffer_class &message_buffer){

    if ( tcp_state_ != listen_call_succesful) {
        if (tcp_debug_level_>0)  std::cout << "tcp class : receive_buffer : status is not 'listen_call_successful'" << std::endl;
		error_state_.set_error_state(false, "tcp class : receive_buffer : status is not 'listen_call_successful'");
        return -1;
    } else {
        if (tcp_debug_level_>0)  std::cout << "tcp class : receive_buffer : ready to read, accepted_connection_fd = " << accepted_connection_fd << std::endl;
    }

    int data_size = message_buffer.get_data_size();
    char *data_ptr = message_buffer.get_data_ptr();
    if (tcp_debug_level_>2)  std::cout << "tcp class : receive_buffer : target message length = " << data_size << std::endl;
    if (data_size <= 0) {
        if (tcp_debug_level_>0) std::cout << "tcp class : receive_buffer : message buffer length " << data_size << " too small to receive message" << std::endl;
		std::string em = "tcp class : receive_buffer : message buffer length " + std::to_string(data_size) + " too small to receive message";
		error_state_.set_error_state(false,em);
        return -2;
    }

    int message_length_to_receive = data_size;
    while ( message_length_to_receive > 0 ) {
        int received_message_length = read(accepted_connection_fd, data_ptr, message_length_to_receive);

        // check for error
        if ( received_message_length<0) {
                std::cout << "tcp class : receive_buffer : error during tcp read, result = " << received_message_length << std::endl;
				std::string em = "tcp class : receive_buffer : error during tcp read, result = " + std::to_string(received_message_length);
				error_state_.set_error_state(false,em);
                return -3;
        }

        if ( received_message_length==0) {
                std::cout << "tcp class : receive_buffer : recieved message length 0 , is client shutting down ???" << received_message_length << std::endl;
				std::string em = "tcp class : receive_buffer : recieved message length 0 , is client shutting down ???" + std::to_string(received_message_length);
				error_state_.set_error_state(false,em);
                return -4;
        }

        // no errors, keep counting
        message_length_to_receive-=received_message_length;
        if (tcp_debug_level_>3) {
            std::cout << "received message length = " << received_message_length;
            std::cout << "      left to receive " << message_length_to_receive << "     from a total of " << data_size << std::endl;
        }
    }

    if (tcp_debug_level_>0)  std::cout << "tcp class : receive_buffer : buffer complete" << data_size << std::endl;
    if (tcp_debug_level_>2) message_buffer.print_buffer_content();
    return data_size;
}


// connect to the previously initialized socket and return the fd
int tcp_class::TCP_accept_connection(){

    unsigned int client_info_len = sizeof(client_socket_info);
	int new_connection_fd = accept(socket_fd_, (struct sockaddr *) &client_socket_info, &client_info_len);
	if (new_connection_fd < 0) {
        if (tcp_debug_level_>-1) std::cout << "tcp class : accept connection - failed" << std::endl;
        return -1;
    }

    accepted_connection_fd = new_connection_fd;
    if (tcp_debug_level_>-1) std::cout << "tcp class : accept connection succeeded - fd = " << accepted_connection_fd << std::endl;

    return new_connection_fd;
}

void tcp_class::TCP_print_status(){

    std::cout << "tcp_class : print status information " << std::endl;
	switch(tcp_state_){
		
		case tcp_initialized:
  			std::cout << "tcp_class : status = TCP object initialized" << std::endl;
            break;
        case socket_created:
  			std::cout << "tcp_class : status = TCP socket created, port = " << port_number_ << std::endl;
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

// prints the fd descriptor information
void tcp_class::TCP_print_fd_info(){
    std::cout << "tcp_class : socket fd = " << socket_fd_ << std::endl;
    std::cout << "tcp_class : accepted_connection_fd = " << accepted_connection_fd << std::endl;
}

int tcp_class::get_host_info(const char *server_addr){

    // assume success	    
    int result = 0;
    if (tcp_debug_level_>1) printf("tcp_class : gethostname for server address = %s\n",server_addr);

	// use ip address to lookup information about the host
	host_info = gethostbyname(server_addr);
    if (host_info == NULL) {
        if (tcp_debug_level_>0) std::cout << "tcp_class : error resolving host information for " << server_ip_address_ << std::endl;
        tcp_state_ = gethost_error;
        result = -1;
    }   else {
        if (tcp_debug_level_>0) std::cout << "tcp_class : host information resolved" << std::endl;
        tcp_state_ = gethost_successful;
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
	server_socket_info.sin_port = htons(port_number_);

    if (tcp_debug_level_>1) printf("tcp_class : socket info    %u\n",server_socket_info.sin_addr.s_addr);
    if (tcp_debug_level_>1) printf("tcp_class : socket info    %i\n",server_socket_info.sin_port);	
	
    return result;
}






/*
int tcp_class::TCP_close(){

    //assume failure
    int result=-1;
    
    result = close_accepted_connection_socket();
    if (result<0) {
        std::cout << "tcp_class : accepted connection socket - shutdown/close failed" << std::endl;
        tcp_state_ = TCP_close_failed;
        return result;
    }
    
    result = close_socket();
    if (result<0) {
        std::cout << "tcp_class : socket - shutdown/close failed" << std::endl;
        tcp_state_ = TCP_close_failed;
        return result;
    }

    tcp_state_ = TCP_close_successful;
    return result;
}
*/



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


