#include <sensor_base_class.h>


//typedef enum sensor_state_type={not_initialized, sensor_ready, invalid};


sensor_time_format get_sensor_timestamp(){
	
    struct timeval start;
    gettimeofday(&start, NULL);

    long mtime = ((start.tv_sec) * 1000 + start.tv_usec/1000.0) + 0.5;
    return (sensor_time_format) mtime;	
}

sensor_base_class::sensor_base_class():
            sensor_state_(sensor_state_type::sensor_not_initialized), sensor_type_(address_class()),
            data_acquisition_count_(0), most_recent_acquisition_time_(0), sensor_debug_level_(SENSOR_DEBUG_LEVEL) {

    std::cout << "sensor_base_class : constructor : debuglevel = " << sensor_debug_level_ << "\n"; 

}

void sensor_base_class::set_sensor_type(const address_class &sensor_type){
    std::cout << "sensor_base_class : set_sensor_type " << sensor_type << "\n";
    sensor_type_ = sensor_type;
}

int sensor_base_class::init_sensor(){
    return -1;
}

int sensor_base_class::acquire_data(){
   return -1;
}

int sensor_base_class::create_sensor_message(){
    return -1;
}

int sensor_base_class:: queue_sensor_message(){
    return -1;
}

sensor_base_class::~sensor_base_class(){
   sensor_state_ = sensor_state_type::sensor_not_initialized;
}

sensor_state_type sensor_base_class::get_state(){ return sensor_state_; }





