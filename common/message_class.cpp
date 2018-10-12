/*
 * message_class.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <message_class.h>

message_class::message_class(
            sender_type_def sender, time_format sender_time, 
            sender_type_def platform, sensor_type_def s_type, time_format orig_time,
            unsigned long message_length){

    state = message_state_def::initialized;

    this -> sender = sender;
    this -> sender_time = sender_time;

    this -> sensor_platform = platform;
    this -> sensor_type = s_type;
    this -> sensor_time = orig_time;

    this -> data_length = message_length;
    this -> data_buffer = (char *) malloc(message_length);
}


message_class::message_state_def message_class::get_state(){
    return state;
}

void message_class::print_meta_data(){
    printf("\n\nmessage metadata:\n");
    if (state == initialized) printf("message status = initialized\n");
    if (state == complete) printf("message status = complete\n");

	printf("sender %c\n",sender);	
	printf("sender time %lu\n",sender_time);
	
    printf("sensor platform %c\n",sensor_platform);
    printf("sensor type %c\n",sensor_type);
    printf("sensor time %lu\n",sensor_time);

	printf("data_length %lu\n",data_length);
    printf("data buffer address %lu\n",(unsigned long) data_buffer);
}

unsigned long message_class::get_data_length(){
    return data_length;
}
char* message_class::get_data_buffer_ptr(){
    return data_buffer;
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
