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


	//sensor_type_def convert_to_message_origin(const char mtype);
    //to be deleted once the infotype works
	//message_class(sensor_type_def mtype, unsigned long buffersiz, unsigned long time, origin_type_def orig, unsigned long orig_time);
	//message_class(header_info_type header_info);

private:

    /*------------------------------------------------------------------------------*/
    message_state_def state;

    sender_type_def sender;
    unsigned long sender_time;

    sender_type_def sensor_platform;
    sensor_type_def sensor_type;
    unsigned long sensor_time;

    unsigned long data_length;
    char *data_buffer;


    /*
    char sensor_type;
    message_origin_def data_origin;
    unsigned long time_origin;
    unsigned long data_length;
    */

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
