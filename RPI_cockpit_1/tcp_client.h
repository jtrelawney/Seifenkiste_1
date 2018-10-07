// this is from
// http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

// it sends a short message to the server and gets one back
// call with the server ip address and the port
// ./tcp.exe 192.168.0.15 8192

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//added those because of warnings
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

class tcp_client{
	
private:
	int sockfd, portno;
	char *server_addr;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	void error(const char *msg);
	void print_n(char *data, int n);
	int create_header(char **sessioninfo, unsigned long imagelen);

public:
	tcp_client();
	~tcp_client();
	
	int send(char *buffer,unsigned long length);
};
