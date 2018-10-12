/*
 * tcp_server.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#include <tcp_server.h>

template <typename T>
T extract(const char *buffer, T variable, const unsigned int n=1){

    unsigned int size = sizeof(variable);
    T result;
    memcpy(&result, buffer, size * n);

    printf("buffer addr %lu\n", (unsigned long) buffer);
    printf("n = %i\n", n);
    printf("size of T = %i\n", size);
    printf("result = %s\n", result);
    return result;
}

// this assumes the full header information is available and not send over several packages
// the length should be fixed, 21 bytes
message_class* tcp_server::extract_header(const char *header_buffer, const int header_length){

    if (header_length!=21){
        printf("header length is %i , not 21 , is something wrong or was the header spit up?\n",header_length);
    }

    message_class *result_message=NULL;

    // set ptr to start of header buffer
    char *seg_ptr = const_cast<char*> (&header_buffer[0]);

    // extract the first segment from the buffer -> control segment "con", then compare to validate the header start
	char comp[]="con";
    char *segment1;
    segment1 = extract(seg_ptr, comp, 3);

    printf("result = %s\n", segment1);

    return result_message;
}

int tcp_server::send_response(int connectionfd, const char *response){
    int count = strlen(response);
    int n = write(connectionfd,response,count);
    return n;
}

// this function is the child process from the fork
// it completely processes the TCP message and either queues a valid message
// or in case of issues requests a retransmision
void tcp_server::process_message(int connectionfd){

    // read message header from the connection
    unsigned int max_header_length = MAX_HEAD_LENGTH;
    char header_buffer[MAX_HEAD_LENGTH];
    bzero(header_buffer,MAX_HEAD_LENGTH);
    int header_length = read(connectionfd,header_buffer,max_header_length-1);
    printf("message header length = %i\n", header_length);

    if (header_length < 0)
        error("ERROR reading message header from connection");
    if (header_length !=21)
        error("ERROR message header too short");
    
    printf("how to determine if this message is actually longer than the a valid control packet? how to reset the connection in this case???\n");

    std::string header_message(header_buffer, header_length);

    // validate the control segments
    std::string con_seg_start = "con";
    int lstart = con_seg_start.length();
    bool start_match = header_message.compare(0,lstart,con_seg_start);
    
    std::string con_seg_end = "end";
    int lend = con_seg_end.length();
    bool end_match = header_message.compare(21-lend,lend,con_seg_end);

    std::cout << "header = " << header_message << std::endl;
    std::cout << "start seg = " << con_seg_start << std::endl;
    std::cout << "lstart = " << lstart << std::endl;
    std::cout << "match = " <<  start_match << std::endl;
    std::cout << "end seg = " << con_seg_end << std::endl;
    std::cout << "lend = " << lend << std::endl;
    std::cout << "match = " <<  end_match << std::endl;

    if (start_match != 0){
        std::cout << "control segment start doesn't match" << std::endl;
        exit(0);
    }

    if (end_match != 0){
        std::cout << "control segment end doesn't match" << std::endl;
        exit(0);
    }
    
    std::cout << "control segments match" << std::endl;
     
    // extract the message header info from the message

    // sender
    sender_type_def sender;
    sender = static_cast<sender_type_def> ( (header_message[3])-char('0') );
    std::cout << "sender = " << sender << "     " << header_message[3] << std::endl;
    if ( sender == sender_type_def::rpi) std::cout << "ok" << std::endl;

    // time sent
    time_format sender_time;
    memcpy(&sender_time, &header_buffer[4],4);
    std::cout << "sender_time = " << sender_time << std::endl;

    // sensor platform
    sender_type_def sensor_platform;
    sensor_platform = static_cast<sender_type_def> ( (header_message[8])-char('0') );
    std::cout << "sensor_platform = " << sensor_platform << std::endl;

    // sensor_type
    sensor_type_def sensor_type;
    sensor_type = static_cast<sensor_type_def> ( (header_message[9])-char('0') );
    std::cout << "sensor_type = " << sensor_type << std::endl;

    // sensor time
    time_format sensor_time;
    memcpy(&sensor_time, &header_buffer[10],4);
    std::cout << "sensor_time = " << sensor_time << std::endl;

    // time sent
    time_format data_length;
    memcpy(&data_length, &header_buffer[14],4);
    std::cout << "data_length = " << data_length << std::endl;

    message_class new_message(sender, sender_time, sensor_platform,sensor_type,sensor_time,data_length);
    new_message.print_meta_data();

    char *data_buffer = new_message.get_data_buffer_ptr();

    int result = read_TCP_data(&data_buffer,data_length);

    new_message.write_to_file();


    // get the state of the message and follow up accordingly
    //message_class::message_state_def message_state = new_message.get_state();

    // if state is messed up then drop the message and request a retransmission
    /*
    if (message_state ==  message_class::message_state_def::initialized) {
        printf("message status undefined - something is wrong !\n");
        printf("drop message and request restransmission\n");

        new_message->~message_class();
        char response[]="retransmit";
        int count = strlen(response);
        int n = write(connectionfd,response,count);
        if (n < 0)
            error("ERROR writing retransmission response to socket");

        return;
        exit(0);
    }

    //if state is in waiting, then it is a long message and the remaining packets will be read into the data buffer of the message
    if (message_state ==  message_class::message_state_def::waiting_for_buffer_data) {

        printf("message is waiting for data ... extract from buffer\n");

        // determine how long the message is going to be and ask for the pointer to the memory the message object has reserved
        unsigned long message_length = new_message->get_data_length();
        unsigned long remaining_message_length = message_length;
        char *message_buffer = new_message->get_data_buffer_ptr();
        printf("fetch %lu bytes from TCP and store in image buffer addr %lu\n",message_length,(unsigned long)message_buffer);

        if(message_length<1) {
            error("ERROR waiting for data but expecting data of length <1 !");
        }

        // previoulsy a handshake after receiving hte header, now removed
        //int s = send_response(connectionfd,"gotheader");
        //if (s < 0) error("ERROR writing header response to socket");

        // keep reading from the connection until the required number of bytes is achieved
        // count up the memory pointer accordingly
        if(message_length>0) {
            unsigned long count_bytes = 0;
            char *data_ptr = message_buffer;
            char *new_data_ptr;
            int n;

            while (count_bytes<remaining_message_length){
                n = read(connectionfd,data_ptr,remaining_message_length);
                if (n < 0) {
                   error("ERROR reading data from socket");
                } else if (n==0) {
                    //printf("message received completely\n");
                    //return;
                    //exit(0);
                } else {
                    printf("received %i bytes, written from %lu  to %lu\n",n,count_bytes,count_bytes+n);
                    new_data_ptr = data_ptr + n;
                    printf("increasing buffer from %lu to %lu\n",(unsigned long)data_ptr,(unsigned long)new_data_ptr);
                    count_bytes+=n;
                    data_ptr = new_data_ptr;
                }
           }

           printf("data bytes received %lu\n",count_bytes);
            int s = send_response(connectionfd,"okokokok");
            if (s < 0) error("ERROR writing confirmation response to socket");
           int result = new_message -> write_to_file();
        }
    }

    if (message_state ==  message_class::message_state_def::complete) {
        printf("message is complete, add to queue and be done\n");
    }
    */

    exit(0);
}

int tcp_server::read_TCP_data(char **data_buffer, unsigned int data_length){

        printf("message is waiting for data ... extract from buffer\n");

        // determine how long the message is going to be and ask for the pointer to the memory the message object has reserved
        unsigned long message_length = data_length;
        unsigned long remaining_message_length = message_length;
        char *message_buffer = *data_buffer;
        printf("fetch %lu bytes from TCP and store in image buffer addr %lu\n",message_length,(unsigned long)message_buffer);

        if(message_length<1) {
            error("ERROR waiting for data but expecting data of length <1 !");
        }

        // previoulsy a handshake after receiving hte header, now removed
        //int s = send_response(connectionfd,"gotheader");
        //if (s < 0) error("ERROR writing header response to socket");

        // keep reading from the connection until the required number of bytes is achieved
        // count up the memory pointer accordingly
        if(message_length>0) {
            unsigned long count_bytes = 0;
            char *data_ptr = message_buffer;
            char *new_data_ptr;
            int n;

            while (count_bytes<remaining_message_length){
                n = read(connectionfd,data_ptr,remaining_message_length);
                if (n < 0) {
                   error("ERROR reading data from socket");
                } else if (n==0) {
                    //printf("message received completely\n");
                    //return;
                    //exit(0);
                } else {
                    printf("received %i bytes, written from %lu  to %lu\n",n,count_bytes,count_bytes+n);
                    new_data_ptr = data_ptr + n;
                    printf("increasing buffer from %lu to %lu\n",(unsigned long)data_ptr,(unsigned long)new_data_ptr);
                    count_bytes+=n;
                    data_ptr = new_data_ptr;
                }
           }

           printf("data bytes received %lu\n",count_bytes);
            int s = send_response(connectionfd,"okokokok");
            if (s < 0) error("ERROR writing confirmation response to socket");
           //int result = new_message -> write_to_file();
        }
}

// this routine accepts a connection on the listening port (i.e. gets a filedescriptor) and then forks a child 
// the parent closes the connection and returns to continue the calling thread
// the child processes the message and then terminates
// this call is blocking until a connection is requested (i.e. a message is being sent by the client) but then returns right away 
int tcp_server::connect_and_receive(){

	fprintf(stderr,"parent here, waiting for incoming connection ...\n");
    connectionfd = connect_to_socket();
	fprintf(stderr," ok, connected\n");

	int pid = fork();
	if (pid < 0)
		error("ERROR on fork");
	if (pid == 0)  {
        fprintf(stderr,"child here, closing socket and starting process to receive message\n");
		close(sockfd);
		process_message(connectionfd);
        fprintf(stderr,"child here, done processing the message\n");
		exit(0);
	} else {
        fprintf(stderr,"parent here, closing connection and returning to caller\n");
        close(connectionfd);
	}

	return 0;
}

// connect to the previously initialized socket and return the fd
int tcp_server::connect_to_socket(){
	int newsockfd = accept(sockfd,
			(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("\nERROR on accept");
    return newsockfd;
}

void tcp_server::print_buffer(const char *buffer, const int n){
	   printf("buffer content: %s\n",buffer);
	   for (int i=0; i<n; i++){
		   printf("%c   %i\n",buffer[i],(char)(buffer[i]));
	   }
}

// creates a socket at the given port, binds and listens
// sets the internal socketfd for the connect
int tcp_server::init_socket(){

    int errorcode=0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    clilen = sizeof(cli_addr);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
             error("ERROR on binding");

    errorcode=listen(sockfd,5);

    return errorcode;
}

// 2 constructors
tcp_server::tcp_server():portno(8192){
    int errorcode = init_socket();
    fprintf(stderr,"port %i ready to receive messages\n",portno);
}

tcp_server::tcp_server(int portnumber): portno(portnumber){
    int errorcode = init_socket();
    fprintf(stderr,"port %i ready to receive messages\n",portno);
}

tcp_server::~tcp_server() {
	// TODO Auto-generated destructor stub
    close(sockfd);
}

void tcp_server::error(const char *msg)
{
    perror(msg);
    exit(1);
}

