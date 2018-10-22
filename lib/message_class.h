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
#include <iostream>

#include <string>
#include <sstream> // for stringstream

#include <defs.h>

// to keep object specific params, tailored to 3*int
#define SPECIAL_PARAMS_BUFFER_LENGTH 12

// length of buffer to be transfered over TCP as header
#define TCP_HEADER_LENGTH 37

class message_class {
public:

    // message states
    // starts with initialized, when the header parameters are set
    // changes to complete when the message data has been set
    // is invalid when during the constructor call invalid parameters are detected
    enum message_state_def {initialized, complete, invalid};


    // 2 constructors
    // one takes information, the other extracts the information from the TCP header
    // both end in status initialized
    // to get to complete the data buffer needs to be filled
    // also set the message id and the additional param buffer
    // before TCP send set the sender time


    // constructor 1
    // initialize message obj with given message parameters
    // to get to status complete the data buffer needs to be filled
	message_class(
            sender_type_def sender, time_format sender_time, 
            sender_type_def sensor_platform, sensor_type_def sensor_type, time_format sensor_time,
            unsigned int data_length);

    // constructor 2
	// from raw TCP header data extract the message parameters and initialize the message
	// to get to status complete the data buffer needs to be filled
	//message_class(std::string header_message);
    message_class(char *header_message);
	
private:

    /*------------------------------------------------------------------------------*/

	// keep the own state, desc see above
    message_state_def state;
	unsigned int message_id;

    sender_type_def sender;
    time_format sender_time;

	// sensor platform
	sender_type_def sensor_platform;
	// sensor type
	sensor_type_def sensor_type;
	// sensor time
	time_format sensor_time;
	// data length
	unsigned int data_length;
	// ptr to databuffer
	char *data_buffer;
	// ptr to param field of size 3 * int
	char *special_param_buffer;

	message_class(){std::cout << "can't use standard constructor for class message_class();\n" << std::endl; exit(0);};

    // extract the message parameters from the TCP header
    int extract_TCP_header_info1(std::string header_message);
    //int extract_TCP_header_info(char *header_message);

public:

    // to set message parameters
	void set_id(unsigned int id);
	unsigned int get_id();
	
    void set_sender_time(time_format t);
	int set_special_params(char *param_buffer, int length);
    
    message_state_def get_state();
    unsigned int get_data_length();
    char *get_data_buffer_ptr();

    // from the message parameters create a TCP header
    int create_TCP_header(char *buffer);


    int extract_special_param_buffer(unsigned int *int1, unsigned int *int2, unsigned int *int3);

    // debuging
    void print_meta_data();
	int write_to_file(const char *fn="./test.jpeg");

	virtual ~message_class();
};

#endif /* COMMON_MESSAGE_CLASS_H_ */
