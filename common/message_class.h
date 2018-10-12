/*
 * message_class.h
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#ifndef COMMON_MESSAGE_CLASS_H_
#define COMMON_MESSAGE_CLASS_H_

#include <stdio.h>	//printf
#include <stdlib.h>	//malloc,free
#include <string.h> //memcpy

#include <defs.h>

class message_class {
public:

	/*
    // to organize the meta data
    struct header_info_type{
        int result;
        char message_type;              // either l or s   (long or short)
        unsigned int time_sent;         // time when message was sent
        char sensor_type;                 // camera, usensor, imu, etc
        char origin;                    // source of message / data, arduino, jetson, rpi, etc
        unsigned int time_origin;       // time at origin when data was collected
        unsigned int data_length;    // length of data segment
        char *databuffer;               // reserved memory for the data

        header_info_type() : result(-1), message_type('u'), time_sent(0), sensor_type('u'), origin('u'), time_origin(0), data_length(0), databuffer(0){};
        void print_data(){ printf("header info data:\n"); };
        };
     */

    //state in constructor starts with not_initialized, when the header is processed and all data is availble state goes to complete
    // if header is from source which sends more data status is set to waiting_for_buffer_data, and when the buffer has been transfered it is set to complete
    // any state with not initialized means error along the way
    enum message_state_def {initialized, complete};
	
	// from raw data information create a message
	message_class(sender_type_def sensor_platform, sensor_type_def sensortype, unsigned int sensor_time, unsigned int data_length);
	
private:

	// keep the own state, desc see above
    message_state_def state;

	// sensor platform
	sender_type_def sensor_platform;
	// sensor type
	sensor_type_def sensor_type;
	// sensor time
	unsigned long sensor_time;
	// data length
	unsigned int data_length;
	// ptr to databuffer
	char *data_buffer;

	message_class();

public:
    message_state_def get_state();
    unsigned long get_data_length();
    char *get_data_buffer_ptr();

	virtual ~message_class();

    void print_meta_data();
	//int writefile(unsigned long imagesize, char *image);
	int write_to_file(const char *fn="./test.jpeg");
};

#endif /* COMMON_MESSAGE_CLASS_H_ */
