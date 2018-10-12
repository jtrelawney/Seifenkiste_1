/*
 * defs.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <defs.h>

#include <sys/time.h>		//gettimeofday
#include <unistd.h> 		//usleep

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


