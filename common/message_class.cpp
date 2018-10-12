/*
 * message_class.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <message_class.h>

message_class::message_class(
        sender_type_def sender, time_format sender_time, 
        sender_type_def sensor_platform, sensor_type_def sensortype, unsigned int sensor_time, 
        unsigned int data_length){

	//state
    this->state = message_state_def::initialized;
    //sender
    this -> sender = sender;
    //sender timestamp
    this -> sender_time = sender_time;
	// sensor platform
	this->sensor_platform = sensor_platform;
	// sensor type
	this->sensor_type = sensortype;
	// sensor time
	this->sensor_time = sensor_time;
	// data length
	this->data_length = data_length;
	// ptr to databuffer
	data_buffer = (char *) malloc(data_length);

}

message_class::message_state_def message_class::get_state(){
    return state;
}

unsigned long message_class::get_data_length(){
    return data_length;
}

char* message_class::get_data_buffer_ptr(){
    return data_buffer;
}

void message_class::print_meta_data(){
    printf("\n\nmessage metadata:\n");
    if (state == initialized) printf("message status = initialized\n");
    if (state == complete) printf("message status = complete\n");

	printf("sender %c\n",sender);	
	printf("sender time %lu\n",sender_time);
	printf("sensor_platform %c\n",sensor_platform);	
    printf("sensor_type %c\n",sensor_type);
    printf("sensor_time %lu\n",sensor_time);
	printf("data_length %lu\n",data_length);
    printf("data buffer address %lu\n",(unsigned long) data_buffer);
}

message_class::~message_class() {
	printf("releasing message buffer!");
	if (data_buffer!=NULL)
			free (data_buffer);
}

int message_class::write_to_file(const char *fn){

    printf("writing %lu bytes from buffer %lu into file\n", data_length, (unsigned long)data_buffer);

	FILE *file;
	file = fopen(fn,"w");
	fwrite(data_buffer, data_length,1,file);
	fclose(file);
	return 0;
}
