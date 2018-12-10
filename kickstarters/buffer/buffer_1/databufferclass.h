#ifndef DATA_BUFFER_CLASS_H
#define DATA_BUFFER_CLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <string.h>
#include <vector>

#include <bufferbaseclass.h>

class data_buffer_class: public buffer_base_class {

private:
    data_buffer_class(data_buffer_class& other) = delete;
    data_buffer_class(const data_buffer_class& other) = delete;
    data_buffer_class& operator=(data_buffer_class& other) = delete;
    data_buffer_class& operator=(const data_buffer_class& other) = delete;

    // move stuff
    //data_buffer_class(data_buffer_class&& other) = delete;
    data_buffer_class(const data_buffer_class&& other) = delete;
    data_buffer_class& operator=(data_buffer_class&& other) = delete;
    data_buffer_class& operator=(const data_buffer_class&& other) = delete;

protected:
	sensor_data_class sensor_data;


    // control move/copy constructors / assignement operators
    //data_buffer_class& operator=(const data_buffer_class&) = delete;
    //data_buffer_class( const data_buffer_class&) = delete;
    //data_buffer_class& operator=(data_buffer_class&&) = delete; // std copy is sufficient
    // need those for creating a message object with a previously tcp created buffer
    //data_buffer_class( data_buffer_class&&) = default;

    // constructor / destructor
    // avoid 0-terminated mess by privatising the related constructors
    data_buffer_class();

public:


    ~data_buffer_class();

    //char* get_data_ptr() override;
    //unsigned int get_data_size() override;

    // create the data buffer using the sensor data
    // essentially the data_length and the data_ptr are the sensor data size and location
    // here custom cvmat object is used but it could be any datastructure.
    // when the time for tcp has come the object needs to deliver the size and the continous memory space used to store the data
    // the constructor basically extracts the buffer info and stores it its members
    // that is not a good idea in case the buffer changes before the tcp can transmitt the buffer
    // it would be better to call the cvmat functiopsn to get the size and ptr
    data_buffer_class(sensor_data_class &sdata);

    // provide the standard move constructor
    data_buffer_class(data_buffer_class&& other);

    // use the tcp_buffer to extract the data
    // the header has been reaceived before and the header has been extracted into a new sensordata object
    // the data is now attached to this existing object
    data_buffer_class(char *buffer, int length);
    int extract_data(sensor_data_class &sensordata);
    //data_buffer_class(char *buffer, int length, sensor_data_class &sensordata);

    // output / debugging
    friend std::ostream& operator<<(std::ostream& os, const data_buffer_class& buffer);
};

#endif
