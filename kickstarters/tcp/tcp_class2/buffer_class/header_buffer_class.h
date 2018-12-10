#ifndef HEADER_BUFFER_CLASS_H
#define HEADER_BUFFER_CLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string.h>
#include <vector>

#include <buffer_base_class.h>

#define TCP_HEADER_LENGTH 34



class header_buffer_class: public buffer_base_class {

protected:

    // control move/copy constructors / assignement operators
    header_buffer_class& operator=(const header_buffer_class&) = delete;
    header_buffer_class( const header_buffer_class&) = delete;
    header_buffer_class& operator=(header_buffer_class&&) = delete; // std copy is sufficient
    // need those for creating a message object with a previously tcp created buffer
    header_buffer_class( header_buffer_class&&) = delete;

    // constructor / destructor
    // avoid 0-terminated mess by privatising the related constructors    
    header_buffer_class();

public:

    //int get_data_size();
    //char* get_data_ptr();

    // constructor / destructor
    //header_buffer_class(int size);
    header_buffer_class(const sensor_data_class &sensor_data);
    ~header_buffer_class();    

    
    //data_buffer_class(char *buffer, int length);
    //data_buffer_class(std::string input);

    // output / debugigging
    void print();
    friend std::ostream& operator<<(std::ostream& os, const header_buffer_class& buffer);
    void set_debug_level(int level);

    // retrieve a pointer where to store data
    // make sure when writing to it that the correct length/size is used and use resize() to adjust if necessary
    //char* data();

    // get and change the size of the buffer    
    //int resize(int new_size);
    //int fill(int startpos, int end_pos, unsigned int value);

    int insert(int pos, std::string text);
    int insert(int pos, unsigned int value);

    unsigned int extract_ui(int pos);
};


#endif
