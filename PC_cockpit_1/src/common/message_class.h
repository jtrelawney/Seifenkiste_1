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

#include <common/defs.h>

class message_class {
public:

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

    //state in constructor starts with not_initialized, when the header is processed and all data is availble state goes to complete
    // if header is from source which sends more data status is set to waiting_for_buffer_data, and when the buffer has been transfered it is set to complete
    // any state with not initialized means error along the way
    enum message_state_def {not_initialized, waiting_for_buffer_data, complete};

	enum message_origin_def {undefined, imu, usonic1, camera1, time};
    message_origin_def convert_to_message_origin(const char mtype);


    //to be deleted once the infotype works
	message_class(message_origin_def mtype, unsigned long buffersiz, unsigned long time, origin_type_def orig, unsigned long orig_time);
	message_class(header_info_type header_info);

private:

    message_state_def state;

    unsigned long time_sent;
    char sensor_type;
    message_origin_def data_origin;
    unsigned long time_origin;
    unsigned long data_length;
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
