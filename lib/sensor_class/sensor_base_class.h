#ifndef SENSOR_BASE_CLASS_H_
#define SENSOR_BASE_CLASS_H_



#include <iostream>
#include <sys/time.h>		//gettimeofday
#include <address_class.h>		//gettimeofday

/*
base class - basic functions

1. constructor init
2. init sensor - read parameter file and startup the device
3. acquire sensor data
4. create sensormessage
5. queue sensor message

*/

const int SENSOR_DEBUG_LEVEL = 1;

typedef unsigned int sensor_time_format;
sensor_time_format get_sensor_timestamp();


enum sensor_state_type {sensor_not_initialized, sensor_init_failed, sensor_ready, invalid_sensor_state};

class sensor_base_class{

protected:
    sensor_state_type sensor_state_;
    address_class sensor_type_;
    //sensor_type_def sensor_type_;
    //platform_type_class::platform_type_def platform_;

    unsigned int data_acquisition_count_;
    sensor_time_format most_recent_acquisition_time_;

    int sensor_debug_level_;

public:

    sensor_base_class();

    virtual int init_sensor();
    virtual int acquire_data();
    virtual int create_sensor_message();
    virtual int queue_sensor_message();
    virtual ~sensor_base_class();

    virtual sensor_state_type get_state();
    virtual void set_sensor_type(const address_class &sensor_type);
};

#endif

