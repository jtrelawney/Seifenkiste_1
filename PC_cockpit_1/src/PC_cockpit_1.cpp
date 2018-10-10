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
#include <message_class.h>

#include <defs.h>
#include <message_queue.h>



int test_queue(){
	message_queue &s = message_queue::getInstance();
	cout << s.getData() << endl;
	s.setData(100);
	cout << s.getData() << endl;

	message_queue &t = message_queue::getInstance();
	cout << t.getData() << endl;
	t.setData(33);

	cout << s.getData() << endl;

	return 0;
}


int main(int argc, char *argv[])
{

	printf("\n\n\ntest the queue\n\n");
	test_queue();

	exit(0);

	return 0;
}



