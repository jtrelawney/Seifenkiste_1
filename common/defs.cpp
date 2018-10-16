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

unsigned long get_timestamp(){
	
	struct timeval start, end;
    long mtime, seconds, useconds;    

    gettimeofday(&start, NULL);
    mtime = ((start.tv_sec) * 1000 + start.tv_usec/1000.0) + 0.5;

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


