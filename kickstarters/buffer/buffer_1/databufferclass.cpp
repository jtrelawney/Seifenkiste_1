#include "databufferclass.h"

data_buffer_class::data_buffer_class(char *buffer, int length){
	if (debug_level>0) std::cout << "data buffer class con-structor call with buffer data" << std::endl;

    // this inserts the buffer data
	int result = insert_data(buffer,length);
	if (result<1) {
		std::cout << "data buffer class : error inserting data into base_class data structures" << std::endl;
		return;
	}

    if (debug_level>3) print_buffer();
}

int data_buffer_class::extract_data(sensor_data_class &sensordata){
	if (debug_level>0) std::cout << "data buffer object : extract data into sensordata" << std::endl;

	// the data is in the buffer and needs to transfer into the sensor object data structure
	// this is a hack becuase normally we would extract the correct values
	// but they were not encoded, so we need to record the datasize as if it was one dim
	sensordata.data.cols = data_length;
	sensordata.data.rows = 1;
	sensordata.data.depth = 1;

	sensordata.data.data = std::move(u_data_ptr);
	return data_length;
}

data_buffer_class::data_buffer_class(sensor_data_class &sensordata){ //:sensor_data(std::move(sdata) ){
    std::cout << "data buffer class constructur call , with given sensordata" << std::endl;

    if (debug_level>0) std::cout << "data buffer class : constructor , saving uptr_data and length" << std::endl;

    // the data exists, fill the buffer members
    data_length = sensordata.get_data_size();
    u_data_ptr = sensordata.get_data_ptr();

    if (debug_level>1) {
    	std::cout << "data length = " << data_length << std::endl;
    	/*
    	buffer_type t;
    	char * s = t.data();
    	buffer_type * u = u_data_ptr.get();
    	char * v = u->data();
    	*/
    	char *addr = u_data_ptr->data();
    	std::cout << "data buffer addr = " << (void *) (addr ) << std::endl;
    }

    std::cout << "done in constructor ... " << std::endl;
}

data_buffer_class::~data_buffer_class(){
    std::cout << "data buffer class : destructor call" << std::endl;
    if (u_data_ptr) u_data_ptr -> clear();
}

/*
data_buffer_class::data_buffer_class(data_buffer_class&& other){
    std::cout << "data buffer class : move constructor call" << std::endl;
    exit(0);
}
*/
/*
char* data_buffer_class::get_data_ptr(){
	std::cout << "data buffer class : call get_data_ptr()" << std::endl;
	return u_data_ptr->data();
	return sensor_data.get_data_ptr()->data();
}

unsigned int data_buffer_class::get_data_size(){
	std::cout << "data buffer class : call get_data_size()" << std::endl;
	return data_length;
	return sensor_data.get_data_size();
}
*/
