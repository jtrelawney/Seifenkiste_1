/*
 * tcp_server.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#include <tcp_server.h>


message_class* tcp_server::extract_header(const char *header_buffer, const int header_length){

    message_class *result_message=NULL;
    message_class::header_info_type info;
    char *seg_ptr;
    unsigned int seg_length;
    
    // extract control segment identifier, currently it is "con"
	char comp[]="con";
	char command[]="000";

    seg_ptr = const_cast<char*> (&header_buffer[0]);
    seg_length = 3*sizeof(char);

    //memcpy(&command[0],&header_buffer[0],3*sizeof(char);
	memcpy(&command[0],seg_ptr,seg_length);
    seg_ptr+=seg_length;

	if (strncmp(command,comp,3)==0) {
        printf("control segment reiceived : %s\n",command);

        // extract the message type, can only be either s or l
        char message_type;
        seg_length = sizeof(char);
    	memcpy(&message_type,seg_ptr,seg_length);
	    seg_ptr+=seg_length;

        // extract the time of sent
        unsigned int time_sent;
        seg_length = sizeof(time_sent);
    	memcpy(&time_sent,seg_ptr,seg_length);
        seg_ptr+=seg_length;

        // extract the data type
        char sensor_type;
    	seg_length = sizeof(sensor_type);
    	memcpy(&sensor_type,seg_ptr,seg_length);
        seg_ptr+=seg_length;

        // extract the origin
        char origin;
    	seg_length = sizeof(origin);
    	memcpy(&origin,seg_ptr,seg_length);
        seg_ptr+=seg_length;

        // extract the time at origin
        unsigned int time_origin;
        seg_length = sizeof(time_origin);
    	memcpy(&time_origin,seg_ptr,seg_length);
        seg_ptr+=seg_length;

        //extract header information, carefull with platfrom difference  rpi is 32bit, pc is 64 bit
	    // unsigned long rpi = 4 bytes
	    // unsigned long pc = 8 bytes
	    // -> use int on pc = 4 bytes
	    unsigned int data_length;
        seg_length = sizeof(data_length);
        memcpy(&data_length,seg_ptr,seg_length);
        seg_ptr+=seg_length;

	    printf("messagetype = %c\n",message_type);
        char *databuffer;
        switch(message_type){
            case 's':
                printf("message type %c  = short message -> copy the data from header into message \n",message_type);
                seg_length = data_length;
                databuffer = seg_ptr;
                seg_ptr+=seg_length;
                // store the databuffer pointer into the info struct and call the message constructor where the data is then copied into the message;
                break;
            case 'l':
                printf("message type %c  = long message -> reserve memory and receive the data from TCP \n",message_type);
                databuffer = 0;
                // set databuffer pointer to 0, call the message constructor where the memory is reserved for later data buffer read from TCP
                break;
            default:
                printf("message type %c  = unknown -> retransmit\n",message_type);
                exit(0);
        }

        // extract end segment identifier, currently it is "end"
	    char comp_end[]="end";
	    char seg_end[]="000";
        seg_length = 4*sizeof(char);
	    memcpy(&seg_end[0],seg_ptr,seg_length);
        seg_ptr+=seg_length;
        if (strncmp(seg_end,comp_end,3)==0) {
            printf("end segment received : %s\n",seg_end);
            printf("send confirmation\n");
            exit(0);
        } else {
            printf("end segment not received : %s\n",seg_end);
            printf("ask for retransmission\n");
            exit(0);
        }

        info.result = 0;
        info.message_type = message_type;
        info.time_sent = time_sent;
        info.sensor_type = sensor_type;
        info.origin = origin;
        info.time_origin = time_origin;
        info.data_length = data_length;
        info.databuffer = databuffer;

        result_message = new message_class(info);
        result_message -> print_meta_data();
        exit(0);
	
    } else {
		printf("not a control segment - command : %s\n",command);
	}

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
void tcp_server::process_message(int connectionfd)
{
    // read message header from the connection
    unsigned int max_header_length = MAX_HEAD_LENGTH;
    char header_buffer[MAX_HEAD_LENGTH];
    bzero(header_buffer,MAX_HEAD_LENGTH);
    int header_length = read(connectionfd,header_buffer,max_header_length-1);
    if (header_length < 0)
        error("ERROR reading message header from connection");
    printf("message header length = %i\n", header_length);
    printf("how to determine if this message is actually longer than the a valid control packet? how to reset the connection in this case???\n");

    // extract the message header info from the message
    message_class *new_message = extract_header(header_buffer, header_length);

    new_message->print_meta_data();

    // get the state of the message and follow up accordingly
    message_class::message_state_def message_state = new_message -> get_state();

    // if state is messed up then drop the message and request a retransmission
    if (message_state ==  message_class::message_state_def::not_initialized) {
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

    exit(0);
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
}

void tcp_server::error(const char *msg)
{
    perror(msg);
    exit(1);
}
