/*
 * defs.h
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

#include <string>
#include <string.h>



#include "opencv2/opencv.hpp"

// define a global varialbe which can be used in the tcp server to create a new image and then display it in the main program
// remove this hack with the implementation of the message queue
extern cv::Mat global_test_frame;

typedef unsigned int time_format;

time_format convert_string_to_time(std::string from_string);

unsigned long get_timestamp();


void print_buffer_content(char *buffer, int length);

//from char buffer of length 12 extract 4 uints
int extract4ints(char *buffer, unsigned int *int1,unsigned int *int2,unsigned int *int3);


// enums
enum sender_type_def {undefined_platform, arduino, rpi, jetson, pc};
enum sensor_type_def {undefined_sensor, imu, usonic1, camera1, time_snapshot};

#endif /* COMMON_DEFS_H_ */
