/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
   gcc server2.c -lsocket
*/

// source: http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

void dostuff(int); /* function prototype */
int writefile(unsigned long imagesize, char *image);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, pid;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     fprintf(stderr,"waiting to accept message on port %i\n",portno);
     uint32_t clilen_match = (uint32_t) clilen;

     while (1) {
         newsockfd = accept(sockfd,
               (struct sockaddr *) &cli_addr, &clilen_match); //&clilen);
         if (newsockfd < 0)
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else {
        	 fprintf(stderr,"parent here, closing socket and starting over\n");
        	 close(newsockfd);
         }
     } /* end of while */
     return 0; /* we never get here */
}

//unsigned int LitToBigEndian(unsigned int x)
unsigned long LitToBigEndian(unsigned long x)
{
	return (((x>>24) & 0x000000ff) | ((x>>8) & 0x0000ff00) | ((x<<8) & 0x00ff0000) | ((x<<24) & 0xff000000));
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
   int n;
   char buffer[256];

   fprintf(stderr,"child here, closing socket and doing stuff ... \n");

   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading control segment from socket");

   printf("Messagelength = %i\n",n);
   //printf("size long int = %lu\n",sizeof(unsigned int));

   printf("Here is the message: %s\n",buffer);
   for (int i=0; i<n; i++){
	   printf("%c   %i\n",buffer[i],(char)(buffer[i]));
   }

   char command[]="000";
   memcpy(&command[0],&buffer[0],3*sizeof(char));
   printf("command : %s\n",command);

   unsigned int imagesize; //=*(addr);
   memcpy(&imagesize,&buffer[3],sizeof(unsigned int));
   printf("imagesize = %u\n",imagesize);
   //printf("imagesize = %lu\n",LitToBigEndian(imagesize));

   char dtype = buffer[n-1];
   printf("datatype = %c\n",dtype);

   printf("Here is the message: %s\n",buffer);
   for (int i=0; i<n; i++){
	   printf("%c   %i\n",buffer[i],(unsigned char)(buffer[i]));
   }

   n = write(sock,"I got your message",18);
   if (n < 0) error("ERROR writing to socket");

   printf("\n\nsummary\n");
   char comp[]="con";
   if (strncmp(command,comp,3)==0) {
	   printf("control segmenet reicevied\n");
	   printf("datasegment size = %u\n",imagesize);
	   printf("data type is %c\n",dtype);
   }

   char *image=(char *)malloc(imagesize);

   if(imagesize>0) {
	   unsigned long count_bytes = 0;
	   char databuffer[2048];
	   while (count_bytes<imagesize){
		   n = read(sock,databuffer,2048);
		   if (n < 0) {
			   error("ERROR reading data from socket");
		   } else {
			   memcpy(&image[count_bytes],&databuffer,n);
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
	   int result = writefile(imagesize,image);
	   if (result<0) { printf("error writing file\n"); } else {printf("file ok\n"); }
   }
}

int writefile(unsigned long imagesize, char *image){
	FILE *file;
	char *fn="./test.jpeg";
	file = fopen(fn,"w");
	fwrite(image, imagesize,1,file);
	fclose(file);
	return 0;
}

