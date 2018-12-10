#ifndef DATA_BUFFER_CLASS_H
#define DATA_BUFFER_CLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string.h>
#include <vector>

#include <buffer_base_class.h>

class data_buffer_class: public buffer_base_class {

protected:
    

    // control move/copy constructors / assignement operators
    data_buffer_class& operator=(const data_buffer_class&) = delete;
    data_buffer_class( const data_buffer_class&) = delete;
    data_buffer_class& operator=(data_buffer_class&&) = delete; // std copy is sufficient
    // need those for creating a message object with a previously tcp created buffer
    data_buffer_class( data_buffer_class&&) = default;

    // constructor / destructor
    // avoid 0-terminated mess by privatising the related constructors    
    data_buffer_class();

public:

    //int get_data_size();
    //char* get_data_ptr();

    // constructor / destructor
    data_buffer_class(int size);
    ~data_buffer_class();    


    // use the existing snsordata
    data_buffer_class(sensor_data_class &sensordata);
    

    // output / debugigging
    void print();
    friend std::ostream& operator<<(std::ostream& os, const data_buffer_class& buffer);
    void set_debug_level(int level);

    // retrieve a pointer where to store data
    // make sure when writing to it that the correct length/size is used and use resize() to adjust if necessary
    //char* data();

};


#endif
