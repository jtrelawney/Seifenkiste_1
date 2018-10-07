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

#include <tcp_server.h>

int main(int argc, char *argv[])
{
	char *message;
	unsigned long message_length;
	int result;

	tcp_server tcp;

	while(1){
		result = tcp.listen_and_receive(&message, &message_length);
		if (result<0)
			printf("error receiving message\n");
		else
			printf("received message, length = %lu\n",message_length);
	}

}



