/*
 * defs.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <common.h>

// initialize the global flag and set the static members
end_flag_class G_END_FLAG = end_flag_class();
std::mutex end_flag_class::flag_mutex;
int end_flag_class::self_counter_ = 0;
end_flag_class::end_flag_class():end_flag(false){ self_counter_++; std::cout << "constructing end flag, object count = " << self_counter_ << " , set to false" << std::endl; }
end_flag_class::~end_flag_class(){ self_counter_--; std::cout << "destructing end flag, objects left = " << self_counter_ << std::endl; }
void end_flag_class::set_flag(){ std::cout << "setting end_flag to true" << std::endl; std::lock_guard<std::mutex> lck (flag_mutex); end_flag = true; }
bool end_flag_class::read_flag(){ std::lock_guard<std::mutex> lck (flag_mutex); /*std::cout << "reading end_flag " << end_flag << std::endl;*/ return end_flag; }



std::time_t get_time(){
    // this seems to give millis
    //return std::chrono::system_clock::to_time_t( std::chrono::high_resolution_clock::now() );
    // this is nanos
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return nanoseconds.count();
}


time_format get_time_stamp(){
	
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
