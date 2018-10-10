#include <tcp_client.h>

/*
 	int n;


    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
*/

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

int tcp_client::send(char *buffer,unsigned long bufferlength){
	
	char *headerdata;
	unsigned long headerlength = (unsigned long)create_header(&headerdata, bufferlength);

    fprintf(stderr,"buffer to transmit, length = %lu\n", headerlength);
    print_n(headerdata,5);
    
    int n = write(sockfd,headerdata,headerlength);
    if (n < 0) 
         error("ERROR writing header to socket");
         
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

