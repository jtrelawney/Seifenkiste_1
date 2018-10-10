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

#include <tcp_client.h>

// the caller hands over an image pointer, this function creates the memory, the caller is reponsible to release the memory
int read_image( char **image, unsigned long *imagelen)
{
        char *buffer;
        unsigned long fileLen;

        //Open file
        FILE *file;
        const char *fn="./test.jpeg";
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
        fprintf(stderr, "buffer addr %lu\n", (unsigned long)&buffer[0]);
        fprintf(stderr, "img addr %lu\n", (unsigned long)*image);
	
	//print_n(buffer,40);
	//print_n(*image,40);

	return 0;
}

int main(int argc, char *argv[])
{
	
	tcp_client tcp;
	
    char *image;
    unsigned long imagelen;
    int res = read_image(&image,&imagelen);

    fprintf(stderr, "return from data fetch\n");
    fprintf(stderr, "image len %lu\n", imagelen);
    fprintf(stderr, "img addr %lu\n", (unsigned long)image);
    //print_n(image,40);

    if (res>0) return 1;


	int result = tcp.send(image,imagelen);
	if (result<0)
		printf("error sending image %i\n",result);
    
    return 0;
}
