/*
 * message_class.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <common/message_class.h>


message_class::message_class()
:message_type(undefined), buffer_size(0), header_size(0), message_size(0), timestamp(0), origin(origin_undef), origin_timestamp(0),message_buffer(NULL){}

message_class::message_class(message_type_def mtype, unsigned long buffersiz, unsigned long time, origin_type_def orig, unsigned long orig_time)
:message_type(mtype), buffer_size(buffersiz), timestamp(time), origin(orig), origin_timestamp(orig_time){

	header_size = sizeof(message_type_def) + sizeof(message_size);
	printf("implement proper message_size!\n");

	message_size = header_size + buffer_size;
	message_buffer = (char *) malloc(message_size);

	printf("message buffer ptr = %lu\n",(unsigned long)message_buffer);
	printf("header_size = %lu\n",(unsigned long)header_size);
	printf("data buffer ptr = %lu\n",(unsigned long)message_buffer+header_size*sizeof(char));

	printf("message length = %lu\n",(unsigned long)message_size);

}

char * message_class::get_buffer(){
	return message_buffer+sizeof(char)*header_size;
}

char * message_class::prep_and_get_message_buffer(){
	printf("implement message prep !\n");
	return message_buffer;
}

message_class::~message_class() {
	printf("releasing message buffer!");
	if (message_buffer!=NULL)
			free (message_buffer);
}

