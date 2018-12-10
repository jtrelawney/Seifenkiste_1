#ifndef HEADER_BUFFER_CLASS_H
#define HEADER_BUFFER_CLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string.h>
#include <vector>

#define TCP_HEADER_LENGTH 34


#define <header_base_class.h>

class header_buffer_class : public header_base_class {

protected:

    // control move/copy constructors / assignement operators
    header_buffer_class& operator=(const header_buffer_class&) = delete;
    header_buffer_class( const header_buffer_class&) = delete;
    header_buffer_class& operator=(header_buffer_class&&) = delete; // std copy is sufficient
    // need those for creating a message object with a previously tcp created buffer
    //header_buffer_class( header_buffer_class&&) = delete;

    // constructor / destructor
    // avoid 0-terminated mess by privatising the related constructors
    header_buffer_class();

public:

    header_buffer_class( header_buffer_class&&);

    // constructor / destructor
    //header_buffer_class(int size);

    // create the header from the sensordata
    // this creates a buffer for the parent class byffer_type data - member
    // the owner of the header buffer object cann then call the get-functions to retrieve the buffer parameters
    header_buffer_class(const sensor_data_class &sensor_data);

    // create the sensor data from a given tcp buffer
    header_buffer_class(char *buffer, int length);
    int extract_header(sensor_data_class &sensor_data);

    // destruct the header information
    ~header_buffer_class();

    // output / debugigging
    friend std::ostream& operator<<(std::ostream& os, const header_buffer_class& buffer);

    int insert(int pos, std::string text);
    int insert(int pos, unsigned int value);
    int insert(int pos, int value);

    int extract(int pos, int length, std::string &text);
    int extract(int pos, unsigned int &var);
    int extract(int pos, int &var);
};


#endif
