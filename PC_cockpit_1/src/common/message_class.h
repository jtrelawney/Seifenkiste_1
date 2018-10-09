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
#include <common/defs.h>

class message_class {
public:

    // to organize the meta data
    struct header_info_type{
        int result;
        char message_type;
        unsigned int message_length;
        char *databuffer;

        header_info_type():result(-1),message_type(undefined),message_length(0),databuffer(0){};
        };

    //state in constructor starts with not_initialized, when the header is processed and all data is availble state goes to complete
    // if header is from source which sends more data status is set to waiting_for_buffer_data, and when the buffer has been transfered it is set to complete
    // any state with not initialized means error along the way
    enum message_state_def {not_initialized, waiting_for_buffer_data, complete};

	enum message_type_def {undefined,imu,usonic1,camera1,time};
    message_type_def convert_to_message_type(const char mtype);


    //to be deleted once the infotype works
	message_class(message_type_def mtype, unsigned long buffersiz, unsigned long time, origin_type_def orig, unsigned long orig_time);
	message_class(header_info_type header_info);

private:
	message_class();

    message_state_def state;
	message_type_def message_type;
	unsigned long buffer_size;
	unsigned long header_size;
	unsigned long message_size;
	unsigned long timestamp;
	origin_type_def origin;
	unsigned long origin_timestamp;

	char *message_buffer;

public:
    message_state_def get_state();
    unsigned long get_message_length();
    char *get_message_buffer_ptr();


	char * get_buffer();
	char * prep_and_get_message_buffer();
	virtual ~message_class();

    void print_meta_data();
	//int writefile(unsigned long imagesize, char *image);
	int write_to_file(const char *fn="./test.jpeg");
};

#endif /* COMMON_MESSAGE_CLASS_H_ */
