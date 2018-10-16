#include <tcp_client.h>


#include <iostream>
#include <cstdio>
#include <stdio.h>

int tcp_client::send_test_image(char *image, unsigned long imagelen){

	int length;
	char *thebytes;

	// collect data for the test
	
	//sender = rpi
	sender_type_def sender = rpi;
	std::cout << "semder " << sender << std::endl;
	//  time sent = local time	
	unsigned long current_time = get_timestamp();
	printf("message time = %lu\n",current_time);
	// sensor platform
	sender_type_def sensor_platform = rpi;
	// sensor type
	sensor_type_def sensortype= camera1;
	// sensor time
	unsigned long sensor_time = get_timestamp()-1000;
	printf("sensor time = %lu\n",sensor_time);
	// data length
	unsigned int data_length = imagelen;
	// ptr to databuffer
	char *data_buffer = image;

	// initialize a new message - doesnt need the sesnor information and buffer ptr
	message_class test_message(sender, current_time, sensor_platform, sensortype, sensor_time, data_length);
	
	// prepare header message using a stream buffer which to fill with the relevant data
	std::stringstream buffer;

	// start segment
	buffer << "con";

	//sender = rpi
	buffer << sender;

	//  time sent = local time	
	thebytes = (char *) &current_time;
	length = sizeof(current_time);
	for(int i=0; i<length; i++) { buffer << *(thebytes+i); }
	for(int i=0; i<length; i++) { printf("%c   %c\n",thebytes[i],*(thebytes+i)); }

	// sensor platform
	buffer << sensor_platform;

	// sensor type
	buffer << sensortype;

	// sensor time
	thebytes = (char *) &sensor_time;
	length = sizeof(sensor_time);
	for(int i=0; i<length; i++) { buffer << *(thebytes+i); }
	for(int i=0; i<length; i++) { printf("%c   %c\n",thebytes[i],*(thebytes+i)); }
	
	// data length
	thebytes = (char *) &data_length;
	length = sizeof(data_length);
	for(int i=0; i<length; i++) { buffer << *(thebytes+i); }
	for(int i=0; i<length; i++) { printf("%c   %c\n",thebytes[i],*(thebytes+i)); }

	// end segment
	buffer << "end";

	/*
	std::string tester;
	buffer >> tester;
	int test_length = tester.length();
	std::cout << "tester = " <<  tester << "   length = " << test_length << std::endl; // tester.length() << std::endl;
	exit(0);
	*/
	
	// buffer completed now stream it into header
	std::string header;
	buffer >> header;

	int header_length = header.length();
	std::cout << "header = " <<  header << "   length = " << header_length << std::endl;
	if (header_length!=21) {
		std::cout << "message length doesn't match 21, what a worry" << std::endl;
		exit(0);
	}

	//transmit the header messsage
	const char *header_char = header.c_str();
	std::cout << "converted to char = " << header_char << std::endl;

	int send_result;
	send_result = send(header_char,header_length);
	if (send_result<0) {
		std::cout << "error sending buffer" << std::endl;
		return -2;
	}
	
	std::cout << "sent " << send_result << " bytes" << std::endl;

	// next send the image data
	send_result = send(data_buffer,data_length);
	if (send_result<0) {
		std::cout << "error sending buffer" << std::endl;
		return -2;
	}

	std::cout << "sent " << send_result << " bytes" << std::endl;
	
	std::cout << "\n\n\nimplement the process response\n\n\n" << std::endl;
	
	return 0;
}

		
void tcp_client::process_error(){
	
	return;
	
	switch(tcp_state){
		
		case tcp_not_initialized:
			tcp_client::error("ERROR TCP not initialized");
			break;
		case socket_error:
			tcp_client::error("ERROR opening socket");
			break;
		case gethost_error:
			tcp_client::error("ERROR, no such host");
			break;
		case connect_error:
			tcp_client::error("ERROR, can't connect to port");
			break;
		default:
			std::cout << "tcp state is ok" << std::endl;
	}
}

int tcp_client::create_socketfd(){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
		tcp_state = socket_error;
		
	return sockfd;
}

int tcp_client::get_hostname(char *server_addr){
	
	int result = 0;
    printf("gethostname for server address = %s\n",server_addr);

	// fill class member
	server = gethostbyname(server_addr); //gethostbyname(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr));

    //fprintf(stderr,"server info\n");
    //char *h_add = server->h_addr;
    //fprintf(stderr,"server address = %s\n",h_add);

    if (server == NULL) {
        tcp_state = gethost_error;
        result = -1;
    } else {		
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, 
			 (char *)&serv_addr.sin_addr.s_addr,
			 server->h_length);
		serv_addr.sin_port = htons(portno);
	}
	
    return result;
}

int tcp_client::TCP_connect(){

    if (tcp_state != tcp_ready_to_connect) {
		std::cout << "socket not ready to connect" << std::endl;
		return -2;
	}
			
	std::cout << "great, status is ready to connect" << std::endl;
	int result = connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	if (result < 0) {
		
		std::cout << "error connecting " << std::endl;
		tcp_state = connect_error;
		return -1;
	}
		
	std::cout << "connection ready" << std::endl;
		
	tcp_state = connection_ready;		
	return 0;
}

int tcp_client::TCP_disconnect(){
	
	// if open then close and set the status ....
	int result = close(sockfd);
	
	tcp_state = tcp_ready_to_connect;
	
	return result;
}

tcp_client::tcp_client(){
	
	tcp_state = tcp_not_initialized;
	
	sockfd = create_socketfd();
    if (tcp_state==socket_error) process_error(); else {
		std::cout << "socket fd created" << std::endl;
		tcp_state = socket_fd_ready;
	}

	// member, im sure there is a shorter way ...	
	char sadr[] = "192.168.0.15";
	int l = strlen(sadr)+1;
	server_addr = (char *) malloc(l*sizeof(char));
	if (server_addr == NULL) exit(0);
	memcpy(server_addr,sadr,l);
	printf("serveraddress = %s   length = %i\n", sadr,l);
	
    portno = 8192; //atoi(argv[2]);
    get_hostname(server_addr);
    if (tcp_state==gethost_error) process_error(); else {
		std::cout << "hostname resolved" << std::endl;
		tcp_state = tcp_ready_to_connect;
	}
	
	int result = TCP_connect();	
	if (tcp_state==connect_error) {
		process_error();
	}
    
    
    /*
    connect_to_socket();
	if (tcp_state==connect_error) process_error(); else tcp_state = connection_ready;
	
	if (tcp_state==connection_ready)
		std::cout << "connected to " << server_addr << " on port " << portno << " established" << std::endl;
	*/
	
}

int tcp_client::send(const char *buffer, unsigned int buffer_length){
	
    printf("TCP - transmitting buffer, length = %lu\n", buffer_length);
    
	if (tcp_state!=connection_ready) {
		std::cout << "TCP socket not ready for a connection" << std::endl;
		return -1;
	}
	
    std::cout << "connection to " << server_addr << " on port " << portno << " is ready" << std::endl;
    
    int n = write(sockfd,buffer,buffer_length);
    if (n < 0) 
         error("ERROR writing buffer to socket");
         
    return n;
}

tcp_client::~tcp_client(){
	TCP_disconnect();
	close(sockfd);
}

/* delete, this is in message class

//takes pointer to char and creates the necessary memory space
//the caller has to release the space
int tcp_client::create_header(char **sessioninfo, unsigned long imagelen){

    unsigned int infolength = 3*sizeof(char) + sizeof(unsigned long) + sizeof(char);
    fprintf(stderr, "\n\ncreating info message, length = %i\n",infolength);

    char *info = (char *)malloc(infolength);//[infolength];
    info[0]='c';
    info[1]='o';
    info[2]='n';
    memcpy(&info[3],&imagelen,sizeof(imagelen));
    info[infolength-1]='h';

    unsigned long test;
    memcpy(&test,&info[3],sizeof(unsigned long));
    fprintf(stderr, "test = %lu\n",test);
    
    *sessioninfo=&info[0];
    print_n(info,8);
    print_n(*sessioninfo,8);
    //print_n((unsigned char *) &test,4);
    
    return infolength;
}
*/

void tcp_client::print_n(char *data, int n)
{
   for (int i=0; i<n; i++){
		fprintf(stderr, "%c   %i\n",data[i], (int)data[i]);
	}
   fprintf(stderr, "\n");
}

void tcp_client::error(const char *msg)
{
    perror(msg);
    //exit(0);
}

