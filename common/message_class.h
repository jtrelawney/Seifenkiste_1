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

    // message states
    // starts with initialized, when the header data is set
    // only if the data message is correctly recieved the state changes to complete
    enum message_state_def {initialized, complete};

    // initialize message obj called with the data in the header
    // only if the data message is successfully received the state changes to complete
	message_class(
            sender_type_def sender, time_format sender_time, 
            sender_type_def sensor_platform, sensor_type_def sensor_type, time_format sensor_time,
            unsigned long data_length);

	// from raw data information create a message
	message_class(sender_type_def sensor_platform, sensor_type_def sensortype, unsigned int sensor_time, unsigned int data_length);
	
private:

    /*------------------------------------------------------------------------------*/

	// keep the own state, desc see above
    message_state_def state;

    sender_type_def sender;
    unsigned long sender_time;

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

	message_class(){exit(0);};

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
