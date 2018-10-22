/*
 * tcp_server.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#include <tcp_server.h>



int tcp_server::send_response(int connectionfd, const char *response, int length){
    int n = write(connectionfd,response,length);
    return n;
}

// read the next header message from TCP
// may encounter issues with partiallly transmitted headers, i.e. when the TCP buffer only contains part of the header and the rest is being transmitted
int tcp_server::read_TCP_header(char **header_buffer){

    // read header from buffer and validate its length
    bzero(*header_buffer, TCP_HEADER_LENGTH);
    int header_length = read(connectionfd, *header_buffer, TCP_HEADER_LENGTH);

    printf("message header length = %i\n", header_length);

    if (header_length < 0)
        error("ERROR reading message header from connection");
    if (header_length !=TCP_HEADER_LENGTH)
        error("ERROR message header too short");

    return header_length;
}

int tcp_server::read_TCP_data(char **data_buffer, unsigned int data_length){

        printf("waiting for data message ... extract from buffer\n");

        // determine how long the message is going to be and ask for the pointer to the memory the message object has reserved
        unsigned long message_length = data_length;
        unsigned long remaining_message_length = message_length;
        char *message_buffer = *data_buffer;
        printf("fetch %lu bytes from TCP and store in image buffer addr %lu\n",message_length,(unsigned long)message_buffer);

        if(message_length<1) {
            error("ERROR waiting for data but expecting data of length <1 !");
            return -1;
        }

        bool output = false;

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
                    if (output) printf("received %i bytes, written from %lu  to %lu\n",n,count_bytes,count_bytes+n);
                    new_data_ptr = data_ptr + n;
                    if (output) printf("increasing buffer from %lu to %lu\n",(unsigned long)data_ptr,(unsigned long)new_data_ptr);
                    count_bytes+=n;
                    data_ptr = new_data_ptr;
                }
            }

        printf("data bytes received %lu\n",count_bytes);
        }
    return 0;
}


// this function is the child process from the fork
// it completely processes the TCP message and either queues a valid message
// or in case of issues requests a retransmision
void tcp_server::process_messages(int connectionfd){

    int result=-1;

    // prepare buffer
    char buffer[MAX_HEAD_LENGTH];
    char *header_buffer = &buffer[0];
   
    // keep reading messages, which should be alternating between a header message and a data message
    printf("starting receiver loop ...\n");

    int N=5;
    while(1){

        std::cout << "\n\nreceiver loop - messages left to receive = " << N << "\n\n" << std::endl;

        // read header from TCP and validate correct length
        result = read_TCP_header(&header_buffer);
        if (result<0) {
            std::cout << "error processing header message" << std::endl;
            perror("quitting receiver loop with error in header message length");
        }
        
    /*
        // process header, i.e. extract its segments and initialize a new message accordingly
        std::string header_message(header_buffer, header_length);
        std::cout << "creating header message string = " << header_message << std::endl;

        //ptr_message test; //new_message = process_header_message(header_message);
        //message_class new_message = process_header_message(header_message);
        message_class new_message(header_message);
    */
        message_class new_message(header_buffer);
        new_message.print_meta_data();        


        // from the special parameter section determine the image dimensions
        unsigned int rows, cols, depth;
        new_message.extract_special_param_buffer(&rows,&cols,&depth);
        std::cout << "formated to : " << rows << " , " << cols << " , " << depth << std::endl;

        // read data message and copy the data into the message
        char *data_buffer = new_message.get_data_buffer_ptr();
        unsigned int data_length = new_message.get_data_length();
        std::cout << "now waiting to receive " << data_length << " data bytes " << std::endl;

        result = read_TCP_data(&data_buffer,data_length);
        if (result<0) {
            std::cout << "error processing data message" << std::endl;
            perror("quitting receiver loop with error in data message");
        }

        //namedWindow("rpi_camera",1);
        // global variable defined in defs.h
        // create the image mat here, the main loop picks it up and displays it in a window
        // in the future the message class will be used to pass the image on

        printf("creating global display image\n");
        global_test_frame = cv::Mat(rows, cols, CV_8UC3,data_buffer);
        display_object.display(global_test_frame);
        //imshow("rpi_camera", frame);
        //waitKey(20);

        bool response_ok = true;

        char ok[] = "okokok";
        char resend[] = "resend";
        unsigned int message_id = new_message.get_id();

        char *response = (char *) malloc( 10*sizeof(char));
        if (response_ok == true)
            memcpy(&response[0],&ok,6);
        else
            memcpy(&response[0],&resend,6);
        
        memcpy(&response[6],&message_id,4);

        int s = send_response(connectionfd,response,10);
        if (s < 0) {
            std::cout << "ERROR writing confirmation response to socket" << std::endl;
        }

        // queue message, for now just write the file
        std::string fname = "./test_"+std::to_string(new_message.get_id())+".jpeg";
        //new_message.write_to_file(fname.c_str());
        N--;
    }

    std::cout << "the endless TCP message processing loop was broken, we shouldn't be here ..." << std::endl;
    //exit(0);
}

/*
ptr_message tcp_server::process_header_message1(std::string header_message){

    message_class new_message = process_header_message(header_message);
    ptr_message message=NULL;

    return message;

    //std::shared_ptr<int> p7 (std::move(p6));
}
*/

// this function extracts the header information from the TCP header message (i.e. the buffer) and 
// initializes the message
message_class tcp_server::process_header_message(std::string header_message){

    // validate the control segments
    std::string con_seg_start = "con";
    int lstart = con_seg_start.length();
    bool start_match = header_message.compare(0,lstart,con_seg_start);
    
    std::string con_seg_end = "end";
    int lend = con_seg_end.length();
    bool end_match = header_message.compare(TCP_HEADER_LENGTH-lend,lend,con_seg_end);

    /*
    std::cout << "header = " << header_message << std::endl;
    std::cout << "start seg = " << con_seg_start << std::endl;
    std::cout << "lstart = " << lstart << std::endl;
    std::cout << "match = " <<  start_match << std::endl;
    std::cout << "end seg = " << con_seg_end << std::endl;
    std::cout << "lend = " << lend << std::endl;
    std::cout << "match = " <<  end_match << std::endl;
    */

    if (start_match != 0){
        std::cout << "control segment start doesn't match" << std::endl;
        exit(0);
    }

    if (end_match != 0){
        std::cout << "control segment end doesn't match" << std::endl;
        exit(0);
    }
    
    std::cout << "header : control segments match" << std::endl;
     
    // extract the message header info from the message
    // tried with string functions, however run into issues because of 0 termination (i.e. stringlength turns out too short)

    // sender
    sender_type_def sender;
    sender = static_cast<sender_type_def> ( (header_message[3])); //-char('0') );
    std::cout << "sender = " << sender << "     " << header_message[3] << std::endl;
    if ( sender == sender_type_def::rpi) std::cout << "ok" << std::endl;

    // time sent
    //std::string time_extract = header_message.substr (4,4);
    //std::cout << "extract 4 bytes = " << time_extract << std::endl;
    //time_format sender_time = convert_string_to_time(time_extract);
    time_format sender_time;
    memcpy(&sender_time, &header_message[4],4);
    std::cout << "sender_time = " << sender_time << std::endl;

    // message id
    unsigned int message_id;
    memcpy(&message_id, &header_message[8],4);
    std::cout << "message_id = " << message_id << std::endl;

    // sensor platform
    sender_type_def sensor_platform;
    sensor_platform = static_cast<sender_type_def> ( (header_message[12]));//-char('0') );
    std::cout << "sensor_platform = " << sensor_platform << std::endl;

    // sensor_type
    sensor_type_def sensor_type;
    sensor_type = static_cast<sensor_type_def> ( (header_message[13]));//-char('0') );
    std::cout << "sensor_type = " << sensor_type << std::endl;

    // sensor time
    time_format sensor_time;
    memcpy(&sensor_time, &header_message[14],4);
    //time_extract = header_message.substr (10,4);
    //time_format sensor_time = convert_string_to_time(time_extract);
    std::cout << "sensor_time = " << sensor_time << std::endl;

    // data length
    unsigned int data_length;
    memcpy(&data_length, &header_message[18],4);
    //std::string length_extract = header_message.substr (14,4);
    //unsigned int data_length = std::stol (length_extract,NULL);;
    std::cout << "data_length = " << data_length << std::endl;

    // from the info initialize and return a new message
    // this also assinge the memory for the message according to the length
    message_class new_message(sender, sender_time, sensor_platform, sensor_type, sensor_time, data_length);
    new_message.set_id(message_id);

    return new_message;
}


// this routine accepts a connection on the listening port (i.e. gets a filedescriptor) and then forks a child 
// the parent closes the connection and returns to continue the calling thread
// the child processes the message and then terminates
// this call is blocking until a connection is requested (i.e. a message is being sent by the client) but then returns right away 
int tcp_server::connect_and_receive(){

	fprintf(stderr,"parent here, blocking to wait for incoming connection ...\n");
    connectionfd = connect_to_socket();
	fprintf(stderr," ok, connected\n");

	int pid = fork();
	if (pid < 0)
		error("ERROR on fork");
	if (pid == 0)  {
        fprintf(stderr,"child here, closing socket and starting process to receive message\n");
		close(sockfd);
		process_messages(connectionfd);
        fprintf(stderr,"child here, done and closing the connection\n"); //processing the message\n");
		exit(0);
	} else {
        //fprintf(stderr,"parent here, closing connection and returning to caller\n");
        std::cout << "child process created, pid = " << pid << std::endl;
        std::cout << "closing parent connection and returning to callier" << std::endl;
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

    std::cout << "creating socket fd" << std::endl;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       error("ERROR opening server socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    clilen = sizeof(cli_addr);

    std::cout << "binding to socket, ready to listen" << std::endl;
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
             error("ERROR on binding");

    std::cout << "starting to listen to socket" << std::endl;
    errorcode=listen(sockfd,5);

    if (errorcode<0) {
        std::cout << "error listening to socket..." << std::endl;
        error("ERROR on listening");
    }

    return errorcode;
}

// 2 constructors
tcp_server::tcp_server():portno(8192){
    error("tcp server constructor");

    //int errorcode = init_socket();
    //fprintf(stderr,"initializing port %i\n",portno);
}

tcp_server::tcp_server(int portnumber): portno(portnumber){

    std::cout << "tcp server constructor, port " << portno << " to be intialized" << std::endl;
    
    int errorcode = init_socket();
    if (errorcode<0) {
        fprintf(stderr,"error %i initializing port %i to receive messages\n",errorcode,portno);
        perror("error initializing port");
    }
    else
        fprintf(stderr,"port %i ready to receive messages\n",portno);
}

tcp_server::~tcp_server() {
	// TODO Auto-generated destructor stub
    std::cout << "tcp server destructor called, does it need an update?" << std::endl;
    //close(sockfd);
}

void tcp_server::error(const char *msg)
{
    perror(msg);
    exit(1);
}

