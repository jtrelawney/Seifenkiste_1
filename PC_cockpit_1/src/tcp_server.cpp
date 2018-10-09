/*
 * tcp_server.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#include <tcp_server.h>

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

tcp_server::tcp_server():portno(8192){
    int errorcode = init_socket();
    fprintf(stderr,"port %i ready to receive messages\n",portno);
}

tcp_server::tcp_server(int portnumber): portno(portnumber){
    int errorcode = init_socket();
    fprintf(stderr,"port %i ready to receive messages\n",portno);
}

// this routine accepts a connection on the listening port (i.e. gets a filedescriptor) and then forks a child 
// the parent closes the connection and returns to continue the calling thread
// the child processes the message and then terminates
// this call is blocking until a connection opens (i.e. a message is being sent by the client) but then returns right away 
int tcp_server::connect_and_receive(){

	fprintf(stderr,"parent here, waiting for incoming connection ...\n");
    connectionfd = connect_to_socket();
	fprintf(stderr," ok, connected\n");

	pid = fork();
	if (pid < 0)
		error("ERROR on fork");
	if (pid == 0)  {
       fprintf(stderr,"child here, closing socket and starting process to receive message\n");
		close(sockfd);
		process_message(connectionfd);
		exit(0);
	}
	else {
	 fprintf(stderr,"parent here, closing connection and returning to caller\n");
	 close(connectionfd);
	}

	return 0;
}

message_class* tcp_server::extract_header(const char *header_buffer, const int header_length){

    message_class *result_message=NULL;
    message_class::header_info_type info;

    // extract control segment identifier, currently it is "con"
	char comp[]="con";
	char command[]="000";
	memcpy(&command[0],&header_buffer[0],3*sizeof(char));
	if (strncmp(command,comp,3)==0) {
        printf("control segment reiceived : %s\n",command);
        
        //extract header information, carefull with platfrom difference  rpi is 32bit, pc is 64 bit
	    // unsigned long rpi = 4 bytes
	    // unsigned long pc = 8 bytes
	    // -> use int on pc = 4 bytes
	    unsigned int message_length;
	    memcpy(&message_length,&header_buffer[3],sizeof(unsigned int));
	    printf("messagelength = %u\n",message_length);

	    char message_type = header_buffer[header_length-1];
	    printf("messagetype = %c\n",message_type);

        info.result = 0;
        info.message_type = message_type;
        info.message_length = message_length;

        result_message = new message_class(info);
        //result_message -> print_meta_data();

	} else {
		printf("not a control segment - command : %s\n",command);
	}


    return result_message;
}

void tcp_server::process_message(int connectionfd)
{
    // read message from the connection
    unsigned int max_header_length = MAX_HEAD_LENGTH;
    char header_buffer[MAX_HEAD_LENGTH];
    bzero(header_buffer,MAX_HEAD_LENGTH);
    int header_length = read(connectionfd,header_buffer,max_header_length-1);
    if (header_length < 0) error("ERROR reading header from connection");
    printf("how to determine if message is actually longer than the a valid header? how to reset the connection in this case???\n");

    printf("Segment length = %i\n",header_length);
    message_class *new_message = extract_header(header_buffer, header_length);
    new_message->print_meta_data();

    message_class::message_state_def message_state = new_message -> get_state();
    if (message_state ==  message_class::message_state_def::not_initialized) {
        printf("message status undefined - something is wrong !\n");
        exit(0);
    }

    if (message_state ==  message_class::message_state_def::waiting_for_buffer_data) {
        printf("message is waiting for data ... extract from buffer\n");

        int n = write(connectionfd,"I got your message",18);
        if (n < 0) error("ERROR writing to socket");

        //char *message=(char *)malloc(messagelength);

        unsigned long message_length = new_message->get_message_length();
        unsigned long remaining_message_length = message_length;
        char *message_buffer = new_message->get_message_buffer_ptr();

        printf("fetch %lu bytes from TCP and store in image buffer addr %lu\n",message_length,(unsigned long)message_buffer);

        if(message_length>0) {
            unsigned long count_bytes = 0;
            //char databuffer[2048];
            char *data_ptr = message_buffer;
            char *new_data_ptr;

            while (count_bytes<remaining_message_length){
                //n = read(connectionfd,databuffer,);
                n = read(connectionfd,data_ptr,remaining_message_length);
                if (n < 0) {
                   error("ERROR reading data from socket");
                } else if (n==0) {
                    //printf("message received completely\n");
                    //exit(0);
                } else {
                    printf("received %i bytes, written from %lu  to %lu\n",n,count_bytes,count_bytes+n);
                    new_data_ptr = data_ptr + n;
                    printf("increasing buffer from %lu to %lu\n",(unsigned long)data_ptr,(unsigned long)new_data_ptr);

                   //memcpy(&message[count_bytes],&databuffer,n);
                   count_bytes+=n;
                    data_ptr = new_data_ptr;
                }
           }
           printf("data bytes received %lu\n",count_bytes);
            int result = writefile(message_length, new_message->get_message_buffer_ptr());
        }
    }

    if (message_state ==  message_class::message_state_def::complete) {
        printf("message is complete, add to queue and be done\n");
    }

    exit(0);

    /*
    unsigned long messagelength = extract_header(buffer, length);

    if (messagelength<0)
       error("ERROR extracting message header");

    if (messagelength==0)
       error("issue, messagelength = 0");


    //print_buffer(buffer,length);

    int n = write(connectionfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");

    char *message=(char *)malloc(messagelength);

    if(messagelength>0) {
       unsigned long count_bytes = 0;
       char databuffer[2048];
       while (count_bytes<messagelength){
	       n = read(connectionfd,databuffer,2048);
	       if (n < 0) {
		       error("ERROR reading data from socket");
	       } else {
		       memcpy(&message[count_bytes],&databuffer,n);
		       printf("received %i bytes, written from %lu  to %lu\n",n,count_bytes,count_bytes+n);
		       count_bytes+=n;
	       }
       }
       printf("data bytes received %lu\n",count_bytes);

       /*
       n = read(sock,databuffer,2048);
       char closingmessage[10];
       if (n < 0) {
	       error("ERROR reading closing message from socket");
       } else {
	       memcpy(&closingmessage,&databuffer,n);
	       printf("received %i bytes     %s\n",n,closingmessage);
       }
       */

    /*
       int result = writefile(messagelength,message);
       if (result<0) { printf("error writing file\n"); } else {printf("file ok\n"); }
    }
    */
}

// connect to the previously initialized socket and return the fd
int tcp_server::connect_to_socket(){
	int newsockfd = accept(sockfd,
			(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("\nERROR on accept");
    return newsockfd;
}

int tcp_server::extract_header1(const char *buffer, const int length){

	char command[]="000";
	memcpy(&command[0],&buffer[0],3*sizeof(char));
	printf("command : %s\n",command);

	// unsigned long rpi = 4 bytes
	// unsigned long pc = 8 bytes
	// -> use int on pc = 4 bytes
	unsigned int imagesize;
	memcpy(&imagesize,&buffer[3],sizeof(unsigned int));
	printf("imagesize = %u\n",imagesize);

	char dtype = buffer[length-1];
	printf("datatype = %c\n",dtype);

	printf("\n\nsummary\n");
	char comp[]="con";
	if (strncmp(command,comp,3)==0) {
	   printf("control segment reiceived\n");
	   printf("datasegment size = %u\n",imagesize);
	   printf("data type is %c\n",dtype);
	} else {
		printf("not a control segment");
	}

	return imagesize;
}

void tcp_server::print_buffer(const char *buffer, const int n){
	   printf("buffer content: %s\n",buffer);
	   for (int i=0; i<n; i++){
		   printf("%c   %i\n",buffer[i],(char)(buffer[i]));
	   }
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void tcp_server::extract_message(int sock)
{
   char buffer[256];
   bzero(buffer,256);
   int length = read(sock,buffer,255);
   if (length < 0) error("ERROR reading control segment from socket");

   printf("Segment length = %i\n",length);
   unsigned long messagelength = extract_header1(buffer, length);

   if (messagelength<0)
	   error("ERROR extracting message header");

   if (messagelength==0)
	   error("issue, messagelength = 0");


   //print_buffer(buffer,length);

   int n = write(sock,"I got your message",18);
   if (n < 0) error("ERROR writing to socket");

   char *message=(char *)malloc(messagelength);

   if(messagelength>0) {
	   unsigned long count_bytes = 0;
	   char databuffer[2048];
	   while (count_bytes<messagelength){
		   n = read(sock,databuffer,2048);
		   if (n < 0) {
			   error("ERROR reading data from socket");
		   } else {
			   memcpy(&message[count_bytes],&databuffer,n);
			   printf("received %i bytes, written from %lu  to %lu\n",n,count_bytes,count_bytes+n);
			   count_bytes+=n;
		   }
	   }
	   printf("data bytes received %lu\n",count_bytes);

	   /*
	   n = read(sock,databuffer,2048);
	   char closingmessage[10];
	   if (n < 0) {
		   error("ERROR reading closing message from socket");
	   } else {
		   memcpy(&closingmessage,&databuffer,n);
		   printf("received %i bytes     %s\n",n,closingmessage);
	   }
	   */

	   int result = writefile(messagelength,message);
	   if (result<0) { printf("error writing file\n"); } else {printf("file ok\n"); }
   }
}

int tcp_server::writefile(unsigned long imagesize, char *image){

    printf("writing %lu bytes from buffer %lu into file\n",imagesize, (unsigned long)image);
	FILE *file;
	const char *fn="./test.jpeg";
	file = fopen(fn,"w");
	fwrite(image, imagesize,1,file);
	fclose(file);
	return 0;
}

tcp_server::~tcp_server() {
	// TODO Auto-generated destructor stub
}

void tcp_server::error(const char *msg)
{
    perror(msg);
    exit(1);
}

// this version does not have a loop, the caller runs the loop
// however currently the data cannot be shuffled back
// for that a message board will be implemented in the next version
// then the data which is currently saved to a file can be handed back for processing somewhere else
int tcp_server::listen_and_receive(char **message, unsigned long *message_length){

    connectionfd = connect_to_socket();
	pid = fork();
	if (pid < 0)
		error("ERROR on fork");
	if (pid == 0)  {
		close(sockfd);
		extract_message(connectionfd);
		exit(0);
	}
	else {
	 fprintf(stderr,"parent here, closing socket and starting over\n");
	 close(connectionfd);
	}

	return 0;
}

//previous version, the loop is in the routine, the caller enters and the loop does its work forking the new listeners
int tcp_server::listen_and_receive_loop(char **message, unsigned long *message_length){

    while (1) {

        connectionfd = connect_to_socket();
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)  {
            close(sockfd);
            extract_message(connectionfd);
            exit(0);
        }
        else {
       	 fprintf(stderr,"parent here, closing socket and starting over\n");
       	 close(connectionfd);
        }
    } /* end of while */
    return 0; /* we never get here */
}


