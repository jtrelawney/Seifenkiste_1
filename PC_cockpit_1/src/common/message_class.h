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
	enum message_type_def {undefined,imu,usonic,camera,time};
	message_class(message_type_def mtype, unsigned long buffersiz, unsigned long time, origin_type_def orig, unsigned long orig_time);

private:
	message_class();
	message_type_def message_type;
	unsigned long buffer_size;
	unsigned long header_size;
	unsigned long message_size;
	unsigned long timestamp;
	origin_type_def origin;
	unsigned long origin_timestamp;

	char *message_buffer;

public:
	char * get_buffer();
	char * prep_and_get_message_buffer();
	virtual ~message_class();
};

#endif /* COMMON_MESSAGE_CLASS_H_ */
