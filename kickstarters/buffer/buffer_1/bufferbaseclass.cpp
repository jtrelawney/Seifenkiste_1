/*
 * bufferbaseclass.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: ethan
 */

#include "bufferbaseclass.h"

buffer_base_class::buffer_base_class():u_data_ptr(nullptr),data_length(0), debug_level(buffer_debug_level){
	if (debug_level>0) std::cout << "\n\nbuffer base class con-structor call" << std::endl;
}

char* buffer_base_class::get_data_ptr(){
	if (debug_level>0) std::cout << "buffer base class get_data_ptr() call" << std::endl;
    return u_data_ptr->data();
}

unsigned int buffer_base_class::get_data_size(){
	if (debug_level>0) std::cout << "buffer base class get_data_size() call" << std::endl;
    if (u_data_ptr==nullptr) return -1;
    return u_data_ptr->size();
}

void buffer_base_class::print_buffer(){
	if (debug_level>0) std::cout << "buffer base class - print_buffer()" << std::endl;
    int s = get_data_size();
    char *pos = get_data_ptr();

    if (s>50) s = 50;
    for (int i=0; i<s; i++){
        char inhalt = *(pos);
        std::cout << "i=" << i << "     " << int(inhalt) << "       " << inhalt << std::endl; //(unsigned int) (inhalt) << "       " << inhalt << std::endl;
        pos++;
    }
}

buffer_base_class::~buffer_base_class(){
	if (debug_level>0) std::cout << "buffer base class de-structor call" << std::endl;
	if (u_data_ptr == nullptr){ if (debug_level>0) std::cout << "buffer_base_class : destructor - u_data_ptr == nullptr (derived class cleaned up)" << std::endl; }
    if (u_data_ptr) {
    	if (debug_level>0) std::cout << "buffer_base_class : destructor - releasing u_data_ptr" << std::endl;
    	u_data_ptr.release();
    }
}

int buffer_base_class::insert_data(char * buffer, int buffer_length){

	std::cout << std::endl << "buffer_base_class : constructor, simulating message receive with given buffer,length" << std::endl;
	data_length = buffer_length;
	if (u_data_ptr) {
		std::cout << "buffer base class : data buffer exists -> resize to " <<  data_length << std::endl;
		u_data_ptr->resize(data_length);
	} else {
		std::cout << "buffer base class : data buffer created, size to " <<  data_length << std::endl;
		u_data_ptr = std::unique_ptr<buffer_type>( new buffer_type(data_length) );
	}

	char *pos = u_data_ptr->data();
    for (int i=0; i<buffer_length; i++){
        int inhalt = *(buffer);
        *pos = inhalt;
        if (debug_level>1) std::cout << "i=" << i << "     " << int(inhalt) << "       " << inhalt << std::endl; //(unsigned int) (inhalt) << "       " << inhalt << std::endl;
        pos++;
        buffer++;
    }

    return buffer_length;
}
