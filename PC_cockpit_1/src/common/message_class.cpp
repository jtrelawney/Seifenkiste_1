/*
 * message_class.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <common/message_class.h>

message_class::message_origin_def message_class::convert_to_message_origin(const char mtype){

    message_origin_def result;
    switch (mtype){
        case 'a':
            printf("converting %c to mtype imu\n",mtype);
            result = message_origin_def::imu;
            break;
        case 'c':
            printf("converting %c to mtype camera\n",mtype);
            result =  message_origin_def::camera1;
            break;
        default:
            result = message_origin_def::undefined;
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
message_class::message_class(header_info_type info){

    state = message_state_def::not_initialized;

    // long or short l,s -> this determines if the data is copied from the given buffer pointer or if the data is later filled in from TCP
    char message_type = info.message_type;

    // copy info 
    time_sent = info.time_sent;

    sensor_type = info.sensor_type;
    data_origin = convert_to_message_origin(info.origin);
    time_origin = info.time_origin;

    data_length = info.data_length;
    data_buffer = NULL;

    // use to copy data from message header if message is of type s
    char *data_source = info.databuffer;

    switch(message_type){
        case 'l':
            printf("message type is long -> status => waiting, fetch data from TCP\n");
            printf("reserving %lu bytes for data\n",data_length);
            data_buffer = (char *) malloc(data_length);
            printf("use ptr %lu to fill image data\n",(unsigned long) data_buffer);
            state=message_state_def::waiting_for_buffer_data; 
            break;
        case 's':
            printf("message type is short -> copy given data and set status => complete\n");
            printf("reserving %lu bytes for data\n",data_length);
            data_buffer = (char *) malloc(data_length);
            memcpy(&data_buffer[0], data_source, data_length);
            state=message_state_def::complete;
            break;
        default:
            printf("issue message constructor: message type should be l or s, but it is %c\n",message_type);
            state=message_state_def::not_initialized;
            exit(0);
    }
}

void message_class::print_meta_data(){
    printf("\n\nmessage metadata:\n");
	printf("time_sent %lu\n",time_sent);	
    printf("sensor_type %c\n",sensor_type);
    printf("data_origin %c\n",data_origin);
    printf("time_origin %lu\n",time_origin);
	printf("data_length %lu\n",data_length);
    printf("data buffer address %lu\n",(unsigned long) data_buffer);
}

/*
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
*/

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


