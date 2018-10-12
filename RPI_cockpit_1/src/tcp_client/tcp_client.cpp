#include <tcp_client.h>

/*
 	int n;


    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
*/

#include <iostream>
#include <cstdio>
#include <stdio.h>

void tcp_client::send_test_image(char *image, unsigned long imagelen){

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
	message_class test_message(sensor_platform, sensortype, sensor_time, data_length);
	
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
	std::cout << "sent " << send_result << " bytes" << std::endl;

	// next send the image data
	send_result = send(data_buffer,data_length);
	std::cout << "sent " << send_result << " bytes" << std::endl;
	
	std::cout << "process response" << std::endl;
}


tcp_client::tcp_client(){
	
	portno = 8192; //atoi(argv[2]);
	char addr[] = "192.168.0.15";
    server_addr = addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        tcp_client::error("ERROR opening socket");
    
    server = gethostbyname(server_addr); //gethostbyname(argv[1]);

    //fprintf(stderr,"server info\n");
    //char *h_add = server->h_addr;
    //fprintf(stderr,"server address = %s\n",h_add);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
        
    printf("connected to %s on port %i\n",server_addr,portno);
    
}

int tcp_client::send(const char *buffer,unsigned int buffer_length){
	
	//char *headerdata;
	//unsigned long headerlength = (unsigned long)create_header(&headerdata, bufferlength);

    printf("buffer to transmit, length = %lu\n", buffer_length);
    
    int n = write(sockfd,buffer,buffer_length);
    if (n < 0) 
         error("ERROR writing header to socket");
    
    return n;
}
/*
         
    char answerbuffer[256];
    bzero(answerbuffer,256);
    n = read(sockfd,answerbuffer,255);
    if (n < 0) 
         error("ERROR reading answer from socket");
    
    printf("%s\n",answerbuffer);

    n = write(sockfd,buffer,bufferlength);
    if (n<0)
		error("ERROR writing buffer to socket");
         
    return 0;    
}
* */


tcp_client::~tcp_client(){
}

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
    exit(0);
}

