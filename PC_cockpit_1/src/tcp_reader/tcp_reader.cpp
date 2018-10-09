
/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
   gcc server2.c -lsocket
*/

// source: http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

#include <stdio.h>
#include <stdlib.h>

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <strings.h>
//#include <string.h>
//#include <unistd.h>

#include <tcp_server.h>
//#include <common/message_class.h>

int main(int argc, char *argv[])
{
    //start the tcp server
	tcp_server tcp;
	int result;

    // keep receiving messages, the processing happens in the function call where
    // the data is received and inserted in a message
    // this message is then inserted into the message queue
	printf("starting message loop\n");
	while(1){
		result = tcp.connect_and_receive();
		if (result<0)
			printf("error connecting to port\n");
//		else
//			printf("received message, length = %lu\n",message_length);
	}

}



/*
#include <common/defs.h>
#include <common/message_class.h>
#include <common/message_queue.h>


void test_message(){

	//message_type_def mtype, unsigned long buffersiz, unsigned long time, origin_type_def orig, unsigned long orig_time

	message_class message(message_class::imu,100,1,arduino,1);
	char *message_ptr, *buffer_ptr;
	buffer_ptr = message.get_buffer();
	message_ptr = message.prep_and_get_message_buffer();
	printf("message pointer = %lu\n",(unsigned long) message_ptr);
	printf("buffer pointer = %lu\n",(unsigned long) buffer_ptr);
}

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
	test_message();

	printf("\n\n\ntest the queue\n\n");
	test_queue();

	exit(0);

	return 0;
}
*/

