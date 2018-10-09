/*
 * message_class.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <common/message_class.h>

message_class::message_type_def message_class::convert_to_message_type(const char mtype){

    message_type_def result;
    switch (mtype){
        case 'a':
            printf("converting %c to mtype imu\n",mtype);
            result = message_type_def::imu;
            break;
        case 'c':
            printf("converting %c to mtype camera\n",mtype);
            result =  message_type_def::camera1;
            break;
        default:
            result = message_type_def::undefined;
    }
    return result;
}

message_class::message_state_def message_class::get_state(){
    return state;
}

// the headerinfo contains the message type
// short message with the data in the header of the tcp message will be completely initialized
// longer messages with data that follows in the coming tcp packages the header information is recorded and the memory space for the data is reserved
// the owner then asks for a pointer to the data memory space and gives that pointer to the tcp handler to fill the data
message_class::message_class(header_info_type header_info){

    message_type = convert_to_message_type(header_info.message_type);
    state=message_state_def::not_initialized;

    // in case of issue
    if (message_type == message_type_def::undefined) {
            printf("something rotten is going on. Cannot process message from undefined sensor\n");
            exit(0);
    }

    //message source is known -> use header info

    /*
    currently defined headerinfo
    int result;
    char message_type;
    unsigned int message_length;
    char *databuffer;

    currently defined message info
	unsigned long header_size;
	unsigned long message_size;
	unsigned long timestamp;
	origin_type_def origin;
	unsigned long origin_timestamp;

	char *message_buffer;
    */
    
    header_size = 0;
    message_size = header_info.message_length;
    timestamp = 0;
    origin = origin_type_def::origin_undef;
    origin_timestamp = 0;

    //printf("mtype = %i\n",message_type);
    //printf("mtype = %i\n",message_type_def::camera);

    switch(message_type){
        case message_type_def::camera1:
            printf("creating message from camera\n");
            printf("reserving %lu bytes for image\n",message_size);
            message_buffer = (char *) malloc(message_size);
            printf("use ptr %lu to fill image data\n",(unsigned long) message_buffer);
            state=message_state_def::waiting_for_buffer_data; 
            break;
        default:
            printf("implement the message completion\n");
            message_buffer = NULL;
            state=message_state_def::not_initialized;
    }
}

void message_class::print_meta_data(){
    printf("\n\nmessage metadata:\n");
	printf("header_size %lu\n",header_size);
	printf("message_size %lu\n",message_size);
    printf("timestamp %lu\n",timestamp);
    printf("origin %i\n",origin);
    printf("origin_timestamp %lu\n",origin_timestamp);
    printf("data buffer address %lu\n",(unsigned long) message_buffer);
}

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

unsigned long message_class::get_message_length(){
    return message_size;
}
char* message_class::get_message_buffer_ptr(){
    return message_buffer;
}

char * message_class::get_buffer(){
	return message_buffer+sizeof(char)*header_size;
    printf("remove get_buffer in message_clase\n");
    exit(0);
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

int message_class::write_to_file(const char *fn){

    printf("writing %lu bytes from buffer %lu into file\n", message_size, (unsigned long)message_buffer);

	FILE *file;
	file = fopen(fn,"w");
	fwrite(message_buffer, message_size,1,file);
	fclose(file);
	return 0;
}


