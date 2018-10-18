/*
 * message_class.cpp
 *
 *  Created on: Oct 8, 2018
 *      Author: ethan
 */

#include <unistd.h> //sleep
#include <message_class.h>
void message_class::set_sender_time(time_format t){
	sender_time = t;
}

int message_class::create_TCP_header(char *header_buffer){
	//avoid all functions which are looking for 0 termination -> memcopy
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
	memcpy(&header_buffer[22],special_param_buffer,SPECIAL_PARAMS_BUFFER_LENGTH);
	memcpy(&header_buffer[34],&end,3);
	return 0;
}

//message_class::message_class(std::string header_message){
// one big mess using char* vs string etc
// so also here use char* array and the most basic memory access functions
message_class::message_class(char *header_message, int header_length){

    // validate the control segments - con
    char con[] = "con";
    int start_match = memcmp(header_message, con,3);

    //bool start_match = (memcmp(&header_message[0], &con[0],3) == 0 ); 
 
    char end[] = "end";
    int end_match = memcmp(&header_message[TCP_HEADER_LENGTH-3], end,3);

    if (start_match != 0){
        std::cout << "control segment start doesn't match" << start_match << std::endl;
        state = message_state_def::invalid;
    }

    if (end_match != 0){
        std::cout << "control segment end doesn't match" << std::endl;
        state = message_state_def::invalid;
    }

    if (state == message_state_def::invalid){    
        std::cout << "header : control segments do not match match" << std::endl;
        std::cout << "invalidating this message" << std::endl;

        print_buffer_content(header_message, TCP_HEADER_LENGTH);

        print_buffer_content(con, 3);

    } else {
        std::cout << "header : control segments match" << std::endl;
 
        std::cout << "now initializing the message including memory allocation for the data -> copy data to complete the message" << std::endl;

        // reverse of header generation
	    memcpy(&(this->sender),&header_message[3],1);
	    memcpy(&(this->sender_time),&header_message[4],4);
	    memcpy(&(this->message_id),&header_message[8],4);
	    memcpy(&(this->sensor_platform),&header_message[12],1);
	    memcpy(&(this->sensor_type),&header_message[13],1);
	    memcpy(&(this->sensor_time),&header_message[14],4);
	    memcpy(&(this->data_length),&header_message[18],4);
        this->data_buffer = (char *) malloc(this->data_length*sizeof(char));
        this->special_param_buffer = (char *) malloc(SPECIAL_PARAMS_BUFFER_LENGTH*sizeof(char));
        //memset((this->special_param_buffer),0,SPECIAL_PARAMS_BUFFER_LENGTH);
	    memcpy((this->special_param_buffer),&header_message[22],SPECIAL_PARAMS_BUFFER_LENGTH);

        //print_buffer_content(&header_message[22],SPECIAL_PARAMS_BUFFER_LENGTH);
        //print_buffer_content(this->special_param_buffer,SPECIAL_PARAMS_BUFFER_LENGTH);
        //exit(0);

        // done initializing, caller needs to add the data
        this->state = message_state_def::initialized;
    }
}

// this function extracts the message parameters from the TCP header and initializes the message members
int message_class::extract_TCP_header_info1(std::string header_message){

    // validate the control segments
    std::string con_seg_start = "con";
    int lstart = con_seg_start.length();
    bool start_match = header_message.compare(0,lstart,con_seg_start);
    
    std::string con_seg_end = "end";
    int lend = con_seg_end.length();
    bool end_match = header_message.compare(TCP_HEADER_LENGTH-lend,lend,con_seg_end);

    /*
    std::cout << "header = " << header_message << std::endl;
    std::cout << "start seg = " << con_seg_start << std::endl;
    std::cout << "lstart = " << lstart << std::endl;
    std::cout << "match = " <<  start_match << std::endl;
    std::cout << "end seg = " << con_seg_end << std::endl;
    std::cout << "lend = " << lend << std::endl;
    std::cout << "match = " <<  end_match << std::endl;
    */

    if (start_match != 0){
        std::cout << "control segment start doesn't match" << std::endl;
        state = message_state_def::invalid;
        return -1;
    }

    if (end_match != 0){
        std::cout << "control segment end doesn't match" << std::endl;
        state = message_state_def::invalid;
        return -2;
    }
    
    std::cout << "header : control segments match" << std::endl;
     
    // extract the message header info from the message
    // tried with string functions, however run into issues because of 0 termination (i.e. stringlength turns out too short)
    // also it depends how the header is created (as stream - subract char0, with memcpy, do not subtract char0

    // sender
    sender_type_def sender;
    sender = static_cast<sender_type_def> ( (header_message[3])); //-char('0') );
    std::cout << "sender = " << sender << "     " << header_message[3] << std::endl;
    if ( sender == sender_type_def::rpi) std::cout << "ok" << std::endl;

    // time sent
    //std::string time_extract = header_message.substr (4,4);
    //std::cout << "extract 4 bytes = " << time_extract << std::endl;
    //time_format sender_time = convert_string_to_time(time_extract);
    time_format sender_time;
    memcpy(&sender_time, &header_message[4],4);
    std::cout << "sender_time = " << sender_time << std::endl;

    // message id
    unsigned int message_id;
    memcpy(&message_id, &header_message[8],4);
    std::cout << "message_id = " << message_id << std::endl;

    // sensor platform
    sender_type_def sensor_platform;
    sensor_platform = static_cast<sender_type_def> ( (header_message[12]));//-char('0') );
    std::cout << "sensor_platform = " << sensor_platform << std::endl;

    // sensor_type
    sensor_type_def sensor_type;
    sensor_type = static_cast<sensor_type_def> ( (header_message[13]));//-char('0') );
    std::cout << "sensor_type = " << sensor_type << std::endl;

    // sensor time
    time_format sensor_time;
    memcpy(&sensor_time, &header_message[14],4);
    //time_extract = header_message.substr (10,4);
    //time_format sensor_time = convert_string_to_time(time_extract);
    std::cout << "sensor_time = " << sensor_time << std::endl;

    // data length
    unsigned int data_length;
    memcpy(&data_length, &header_message[18],4);
    //std::string length_extract = header_message.substr (14,4);
    //unsigned int data_length = std::stol (length_extract,NULL);;
    std::cout << "data_length = " << data_length << std::endl;

    // from the info initialize and return a new message
    // this also assinge the memory for the message according to the length
    //message_class new_message(sender, sender_time, sensor_platform, sensor_type, sensor_time, data_length);
    //new_message.set_id(message_id);

    //return new_message;
    return 0;
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
	this->data_buffer = (char *) malloc(data_length*sizeof(char));
	
	//parambuffer init to 0, can be filled later
	this->special_param_buffer = (char *) malloc(sizeof(char)*SPECIAL_PARAMS_BUFFER_LENGTH);
	memset((this->special_param_buffer),0,SPECIAL_PARAMS_BUFFER_LENGTH);
}

int message_class::set_special_params(char *param_buffer, int length){
	//printf("param_buffer ptr     address = %lu   value = %lu\n", (unsigned int)&param_buffer, (unsigned int)param_buffer);
	//printf("special_param_buffer ptr     address = %lu   value = %lu\n", (unsigned int)&special_param_buffer, (unsigned int)special_param_buffer);
	if (length>SPECIAL_PARAMS_BUFFER_LENGTH) return -1;
	//print_buffer_content(special_param_buffer, SPECIAL_PARAMS_BUFFER_LENGTH);
	memcpy(special_param_buffer,param_buffer,length);
	//print_buffer_content(special_param_buffer, SPECIAL_PARAMS_BUFFER_LENGTH);
	return 0;
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

unsigned int message_class::get_data_length(){
    return data_length;
}

char* message_class::get_data_buffer_ptr(){
    return data_buffer;
}

int message_class::extract_special_param_buffer(unsigned int *int1, unsigned int *int2, unsigned int *int3){
    if (special_param_buffer==NULL) return -1;
    //std::cout << "converting special param buffer to 3 unsinged ints" << std::endl;
    //print_buffer_content(this->special_param_buffer,SPECIAL_PARAMS_BUFFER_LENGTH);
    memcpy(int1,&special_param_buffer[0],4);
    memcpy(int2,&special_param_buffer[4],4);
    memcpy(int3,&special_param_buffer[8],4);
    return 0;
}

void message_class::print_meta_data(){
	printf("\n-------------------------\n");
    printf("\n\nmessage metadata:\n");
    printf("message ptr %lu\n",(unsigned long)this);	
    if (state == initialized) printf("message status = initialized\n");
    if (state == complete) printf("message status = complete\n");

	printf("message ID %u\n",(unsigned int)message_id);	
	printf("sender %c\n",sender);	
	printf("sender time %u\n",sender_time);
	printf("sensor_platform %c\n",sensor_platform);	
    printf("sensor_type %c\n",sensor_type);
    printf("sensor_time %u\n",sensor_time);
	printf("data_length %u\n",data_length);
    printf("data buffer address %lu\n",(unsigned long) data_buffer);
    
    printf("special param buffer = \n ");
    printf("achtung : display int(unsigned char) conversion , what a mess\n ");
    //print_buffer_content(this->special_param_buffer,SPECIAL_PARAMS_BUFFER_LENGTH);

    for (int i=0; i<SPECIAL_PARAMS_BUFFER_LENGTH; i++){
        unsigned char uc = (unsigned char) special_param_buffer[i];
		//char c = (unsigned char) special_param_buffer[i];
	    //int o = int(c);
        //int p = o & 0xFF;
        
        std::cout << int(uc) << " , ";
		//printf("%u,",o);
	}

	printf("\n\n\n-------------------------\n");
}

message_class::~message_class() {
	std::cout << "\n\nmessage class - releasing message buffer!" << std::endl;
    //printf("message ptr %lu\n",(unsigned int)this);	
	free(special_param_buffer);
	//std::cout << "after freeing special params, now freeing data buffer!" << std::endl;
	//std::cout << "data ptr = " << (unsigned int) data_buffer << std::endl;
	if (data_buffer!=NULL) free (data_buffer);
	//printf("done ... \n");
}

int message_class::write_to_file(const char *fn){

    printf("writing %u bytes from buffer %lu into file\n", data_length, (unsigned long)data_buffer);

	FILE *file;
	file = fopen(fn,"w");
	fwrite(data_buffer, data_length,1,file);
	fclose(file);
	return 0;
}
