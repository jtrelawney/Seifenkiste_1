/*
 * message_class.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <message_class.h>

int message_class::create_TCP_header(char *header_buffer){

	//avoid all functions which are looking for 0 termination
	char con[] = "con";
	char end[] = "end";
	memcpy(&header_buffer[0],&con,3);
	memcpy(&header_buffer[3],&sender,1);
	memcpy(&header_buffer[4],&sender_time,4);
	memcpy(&header_buffer[8],&message_id,4);
	memcpy(&header_buffer[12],&sensor_platform,1);
	memcpy(&header_buffer[13],&sensor_type,1);
	memcpy(&header_buffer[14],&sensor_time,4);
	memcpy(&header_buffer[18],&data_length,4);
	memcpy(&header_buffer[22],&end,3);
	return 0;
	
	/*
	printf("header\n");
	
	for (int i=0; i<buffer_length; i++){
		char outp = header_buffer[i];
		std::cout << int(outp) << "   " << outp << std::endl;
	}
	std::cout << std::endl;
	*/
	
	//std::string header(header_buffer);
	//std::cout << header << std::endl;
	
	exit(0);
	
	/*
	
	// unsigned int message_id;

	bool debug = true;
	
	// prepare header message using a stream buffer which to fill with the relevant data
	std::stringstream buffer;

	// start segment
	buffer << "con";

	//sender = rpi
	buffer << sender;
	if (debug) std::cout << "sender = " << sender << std::endl;

	//  time sent = local time	
	//buffer << sender_time;
	if (debug) std::cout << "sender time = " << sender_time << std::endl;
	
	char *thebytes = (char *) &sender_time;
	unsigned int length = sizeof(sender_time);
	for(int i=0; i<length; i++) { buffer << *(thebytes+i); }
	if (debug) {
		std::cout << "sender time length = " << length << std::endl;
		for(int i=0; i<length; i++) { printf("%c   %c\n",thebytes[i],*(thebytes+i)); }
	}
	
	// sensor platform
	buffer << sensor_platform;
	if (debug) std::cout << "sensor_platform = " << sensor_platform << std::endl;

	// sensor type
	buffer << sensor_type;
	if (debug) std::cout << "sensor_type = " << sensor_type << std::endl;

	// sensor time
	//buffer << sensor_time;
	
	thebytes = (char *) &sensor_time;
	length = sizeof(sensor_time);
	for(int i=0; i<length; i++) { buffer << *(thebytes+i); }
	if (debug) {
		std::cout << "sensor time length = " << length << std::endl;
		for(int i=0; i<length; i++) { printf("%c   %c\n",thebytes[i],*(thebytes+i)); }
	}
	
	// data length
	//buffer << sensor_time;
	thebytes = (char *) &data_length;
	length = sizeof(data_length);
	for(int i=0; i<length; i++) { buffer << *(thebytes+i); }
	if (debug) {
		std::cout << "data length length = " << length << std::endl;
		for(int i=0; i<length; i++) { printf("%c   %c\n",thebytes[i],*(thebytes+i)); }
	}
	
	// end segment
	buffer << "end";

	
	// buffer completed now stream it into header
	std::string header;
	buffer >> header;
	
	return header;
	* */
}

message_class::message_class(
        sender_type_def sender, time_format sender_time, 
        sender_type_def sensor_platform, sensor_type_def sensor_type, time_format sensor_time, 
        unsigned int data_length){

	//state
    this->state = message_state_def::initialized;
    //id
    this->message_id = 0;
    
    //sender
    this -> sender = sender;
    //sender timestamp
    this -> sender_time = sender_time;
	// sensor platform
	this->sensor_platform = sensor_platform;
	// sensor type
	this->sensor_type = sensor_type;
	// sensor time
	this->sensor_time = sensor_time;
	
	// data length
	this->data_length = data_length;
	// ptr to databuffer
	data_buffer = (char *) malloc(data_length);
}

void message_class::set_id(unsigned int id){
    message_id = id;
}

unsigned int message_class::get_id(){
    return message_id;
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

	printf("message ID %lu\n",message_id);	
	printf("sender %i\n",sender);	
	printf("sender time %lu\n",sender_time);
	printf("sensor_platform %i\n",sensor_platform);	
    printf("sensor_type %i\n",sensor_type);
    printf("sensor_time %lu\n",sensor_time);
	printf("data_length %u\n",data_length);
    printf("data buffer address %lu\n",(unsigned long) data_buffer);
}

message_class::~message_class() {
	std::cout << "message class - releasing message buffer!" << std::endl;
	if (data_buffer!=NULL)
			free (data_buffer);
}

int message_class::write_to_file(const char *fn){

    printf("writing %u bytes from buffer %lu into file\n", data_length, (unsigned long)data_buffer);

	FILE *file;
	file = fopen(fn,"w");
	fwrite(data_buffer, data_length,1,file);
	fclose(file);
	return 0;
}
