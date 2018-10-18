/*
 * defs.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <defs.h>

#include <sys/time.h>		//gettimeofday
#include <unistd.h> 		//usleep


#include <iostream>


int extract4ints(char *buffer,unsigned int *int1,unsigned int *int2,unsigned int *int3){
    memcpy(int1,&buffer[0],4);
    memcpy(int2,&buffer[4],4);
    memcpy(int3,&buffer[8],4);
    return 0;
}

void print_buffer_content(char *buffer, int length){
    std::cout << "buffer content = " << std::endl;
    if (length>50) {
        std::cout << "max size is 50, length is " << length << std::endl;
        return;
    }
        
    for (int i=0; i<length; i++){
        char c = buffer[i];
        unsigned char uc = (unsigned char)(c);
        char sc = (char)(c);
        int ci = int(c);
        //std::cout << i << "   " << c << "   " << uc << "   " << sc << "   " << ci << std::endl;
        printf("%i   %c   %u   %c   %i\n",i,c,uc,sc,ci);
    }
}

unsigned long get_timestamp(){
	
    struct timeval start;
    long mtime;
    

    gettimeofday(&start, NULL);
    mtime = ((start.tv_sec) * 1000 + start.tv_usec/1000.0) + 0.5;

	//struct timeval start, end;
    //long mtime, seconds, useconds;    

    //usleep(20000);
    //gettimeofday(&end, NULL);

    //seconds  = end.tv_sec  - start.tv_sec;
    //useconds = end.tv_usec - start.tv_usec;

    //mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    //printf("Elapsed time: %ld milliseconds\n", mtime);

    return (unsigned long) mtime;	
}

time_format convert_string_to_time(std::string from_string){
    // convert string to unsigned long, param pos=0, base =10
    std::cout << "converting string " << from_string << " into unsigned long" << std::endl;
    //unsigned long long_result = std::stoul(from_string,nullptr,10);
    //int long_result = std::stoi(from_string);

    int long_result = atoi(from_string.c_str());
    time_format to_time = (time_format) long_result;
    return to_time;
}


