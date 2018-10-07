/*
 * tcp_server.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ethan
 */

#include <tcp_server.h>

tcp_server::tcp_server() {
	// TODO Auto-generated constructor stub
    portno = 8192;

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

    listen(sockfd,5);
    fprintf(stderr,"waiting to accept message on port %i\n",portno);
}

// this version does not have a loop, the caller runs the loop
// however currently the data cannot be shuffled back
// for that a message board will be implemented in the next version
// then the data which is currently saved to a file can be handed back for processing somewhere else
int tcp_server::listen_and_receive(char **message, unsigned long *message_length){

	newsockfd = accept(sockfd,
			(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");

	pid = fork();
	if (pid < 0)
		error("ERROR on fork");
	if (pid == 0)  {
		close(sockfd);
		extract_message(newsockfd);
		exit(0);
	}
	else {
	 fprintf(stderr,"parent here, closing socket and starting over\n");
	 close(newsockfd);
	}

	return 0;
}

//previous version, the loop is in the routine, the caller enters and the loop does its work forking the new listeners
int tcp_server::listen_and_receive_loop(char **message, unsigned long *message_length){

    while (1) {
        newsockfd = accept(sockfd,
        		(struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)  {
            close(sockfd);
            extract_message(newsockfd);
            exit(0);
        }
        else {
       	 fprintf(stderr,"parent here, closing socket and starting over\n");
       	 close(newsockfd);
        }
    } /* end of while */
    return 0; /* we never get here */
}

int tcp_server::extract_header(const char *buffer, const int length){

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
   fprintf(stderr,"child here, closing socket and doing stuff ... \n");

   char buffer[256];
   bzero(buffer,256);
   int length = read(sock,buffer,255);
   if (length < 0) error("ERROR reading control segment from socket");

   printf("Segment length = %i\n",length);
   unsigned long messagelength = extract_header(buffer, length);

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
