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

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void print_n(unsigned char *data, unsigned long n)
{
   for (int i=0; i<n; i++){
		fprintf(stderr, "%c   %i\n",data[i], (int)data[i]);
	}
   fprintf(stderr, "\n");
}

// the caller hands over an image pointer, this function creates the memory, the caller is reponsible to release the memory
int read_image( unsigned char **image, unsigned long *imagelen)
{
        unsigned char *buffer;
        unsigned long fileLen;

        //Open file
        FILE *file;
        char *fn="./test.jpeg";
        fprintf(stderr, "opening file %s\n", fn);

        file = fopen(fn, "rb");
        if (!file)
        {
                fprintf(stderr, "Unable to open file %s", fn);
                return 2;
        }

        //Get file length
        fseek(file, 0, SEEK_END);
        fileLen=ftell(file);
        fseek(file, 0, SEEK_SET);

        //Allocate memory
        buffer=(char *)malloc(fileLen);
        if (!buffer)
        {
                fprintf(stderr, "Memory error!");
                fclose(file);
                return 1;
        }

	// print some test data
	*imagelen=fileLen;
        fprintf(stderr, "file length %lu\n", fileLen);
        fprintf(stderr, "image len %lu\n", *imagelen);

        fread(buffer,fileLen,sizeof(unsigned char),file);
        fclose(file);
        fprintf(stderr, "done reading file\n");

        *image = buffer;
        fprintf(stderr, "buffer addr %lu\n", buffer);
        fprintf(stderr, "img addr %lu\n", *image);
	
	//print_n(buffer,40);
	//print_n(*image,40);

	return 0;
}

//takes pointer to char and creates the necessary memory space
//the caller has to release the space
int create_info(char **sessioninfo, unsigned long imagelen){

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

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);

    fprintf(stderr,"server info\n");
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
    printf("Please enter the message: ");

    unsigned char *image;
    unsigned long imagelen;
    int res = read_image(&image,&imagelen);

    fprintf(stderr, "return from data fetch\n");
    fprintf(stderr, "image len %lu\n", imagelen);
    fprintf(stderr, "img addr %lu\n", image);
    //print_n(image,40);

    if (res>0) return 1;

    //bzero(buffer,256);
    //fgets(buffer,255,stdin);
	char *sessioninfo;
	unsigned long infolength;
	infolength = (unsigned long)create_info(&sessioninfo,imagelen);

    fprintf(stderr,"buffer to transmit, length = %lu\n", infolength);
    print_n(sessioninfo,5);
    
    fprintf(stderr,"unsigned long length = %i\n",sizeof(unsigned long));

    
    n = write(sockfd,sessioninfo,infolength);
    //n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);

    n = write(sockfd,image,imagelen);
    return 0;
}