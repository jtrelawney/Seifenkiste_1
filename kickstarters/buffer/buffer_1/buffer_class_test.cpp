#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#include <string.h>
#include <unistd.h> // sleep


#include <bufferbaseclass.h>
#include <databufferclass.h>
#include <headerbufferclass.h>

buffer_type header_sent, data_sent;
int header_sent_size, data_sent_size;

void print_buffer(buffer_type buffer, int size){
	char *d = buffer.data();
	std::cout << "buffer content (size = " << size << ")" << std::endl;
	for (int i=0; i<size; i++){
		char content = *(d);
		std::cout << int(content) << ",";
		d++;
	}
	std::cout << std::endl;
}

// send the buffer and hand back a control buffer for later use, latter is only for test purposes
int TCPsend(buffer_base_class *buffer, buffer_type &char_buffer){

	std::cout << std::endl << "simulating a tcp send with:" << std::endl;
	std::cout << "buffer size = " << buffer->get_data_size() << std::endl;
	std::cout << "buffer addr = " << (void *)buffer -> get_data_ptr() << std::endl;
	//buffer -> print_buffer();
	sleep(1);

    int s = buffer->get_data_size();
    char *pos = buffer->get_data_ptr();
    char_buffer.resize(s);

    for (int i=0; i<s; i++){
        int inhalt = *(pos);
        char_buffer[i] = inhalt;
        std::cout << "i=" << i << "     " << int(inhalt) << "       " << inhalt << std::endl; //(unsigned int) (inhalt) << "       " << inhalt << std::endl;
        pos++;

    }
	return s;
}

// "receive a message" and return the base buffer with the data and size
header_buffer_class TCP_read_header(){
	//buffer_base_class buffer(header_sent.data(),header_sent_size);
	header_buffer_class header_buffer(header_sent.data(),header_sent_size);
	return header_buffer;
}

data_buffer_class TCP_read_data(){
	//buffer_base_class buffer(header_sent.data(),header_sent_size);
	//buffer_type , ;
	//int , ;
	data_buffer_class data_buffer(data_sent.data(),data_sent_size);
	data_buffer.print_buffer();
	return data_buffer;
}



void send_sensordata(sensor_data_class &sensor_data){

	// step one : crate the header and send it via tcp
	std::cout << "\n\nin tcp send, first process the header\n" << std::endl;
	sensor_data.print();
	std::cout << "the header buffer class creates its own buffer with fixed buffer size and then fills it with sensor metadata" << std::endl;
	std::cout << "hence the buffer address and the data address are different" << std::endl;
	std::cout << "this means the sensor data object is unchanged and can later be used to send the data" << std::endl;
	header_buffer_class *hb = new header_buffer_class(sensor_data);
	std::cout << "header buffer size = " << hb -> get_data_size() << std::endl;
	std::cout << "header buffer addr = " << (void *)hb -> get_data_ptr() << std::endl;

	// look at it as a char buffer and use it to send the header via tcp
	buffer_base_class *tcp_buffer = hb;
	header_sent_size = TCPsend(tcp_buffer,header_sent);

	// do not delete the tcp_buffer, it is a pointer to the header buffer object
	tcp_buffer = nullptr;
	// delete the header buffer
	delete hb;

	// step two : send the data
	std::cout << "\n\nnext process the data" << std::endl;
	std::cout << "current sensor data" << std::endl;
	sensor_data.print();

	data_buffer_class *db = new data_buffer_class(sensor_data);
	std::cout << "\n\nsensordata after data_buffer was created" << std::endl;
	sensor_data.print();

	std::cout << "data buffer size = " << db -> get_data_size() << std::endl;
	std::cout << "data buffer addr = " << (void *)db -> get_data_ptr() << std::endl;

	tcp_buffer = db;
	data_sent_size = TCPsend(tcp_buffer,data_sent);
	tcp_buffer = nullptr;
	delete db;
}

// this takes the global test data and creates the sensor data from it
sensor_data_class receive_sensordata(){

	// result to be filled with data from buffer
	sensor_data_class sensordata;

	// read tcp and create the headerbuffer from it, then extract the header info into the sensor_data
	header_buffer_class header_buffer = TCP_read_header();
	//header_buffer.print_buffer();

	int result  = header_buffer.extract_header(sensordata);

	if (result<0){
	    std::cout << std::endl << "receive sensordata : error extracting header info" << std::endl;
	}

	// read tcp and create the data buffer from it, then extract the data into the sensor data
	data_buffer_class data_buffer = TCP_read_data();
	result  = data_buffer.extract_data(sensordata);

	if (result<0){
	    std::cout << std::endl << "receive sensordata : error extracting header info" << std::endl;
	}

	sensordata.print();

	return sensordata;
}

int main()
{
	/*
    std::cout << "test the buffer_class" << std::endl;

    std::cout << std::endl << "create and delete base class object " << std::endl;
    buffer_base_class *bbc1 = new buffer_base_class();
    std::cout << "buffer base class size = " << bbc1 -> get_data_size() << std::endl;
    //conversion from base to dervied doesnt fly
    //data_buffer_class *dnw1 = bbc1;
    delete bbc1;
	*/

    // simulate a tcp send activity
    // first create a test message
    // pass it to the tcp send command
    // within tcp send the sensor object is first processed as a header buffer
    // after sending the header buffer the sensor object is processed as a data header

    std::cout << std::endl << "simulate a tcp send" << std::endl;
    std::cout << "first create sensor test data" << std::endl;
    unsigned int message_id = 4711;
    char platform = 2;
    char value = 69;
    sensor_data_class sd(message_id, platform, value);
    // this will be implemented as a uniqueptr and hence teh ownerhsip will pass to tcpsend
    send_sensordata(sd);

    std::cout << std::endl << "========================================================" << std::endl;
    std::cout << std::endl << "after tcp send we have transmitted the following" << std::endl;

	if ( header_sent_size > 0 ) {
		std::cout << "header successfully sent" << std::endl << std::endl;
		print_buffer(header_sent, header_sent_size);
	}

    if (data_sent_size > 0) {
		std::cout << "data successfully sent" << std::endl << std::endl;
		print_buffer(data_sent, data_sent_size);
	}

	std::cout << std::endl << "========================================================" << std::endl;

    std::cout << std::endl << "wait a moment and reverse process to see if we can receive" << std::endl;
    sleep(1);

    // this is an assignment operator, but the compiler wants to invoke the asssignment constructor
    sensor_data_class sd_rec = receive_sensordata();
    std::cout << std::endl << "========================================================" << std::endl;
    std::cout << std::endl << "the sensor data result after receiving from tcp" << std::endl;
    sd_rec.print();

    std::cout << std::endl << "the end" << std::endl;
}
