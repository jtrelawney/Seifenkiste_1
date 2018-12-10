#include <address_class.h>

address_class::address_class() : platform_(undefined_platform), sensor_(undefined_sensor), process_(undefined) {}
address_class::address_class(platform_type_def platform, sensor_type_def sensor, process_type_def process) : platform_(platform), sensor_(sensor), process_(process) {}

address_class::platform_type_def address_class::get_platform() const { return platform_; }
address_class::sensor_type_def address_class::get_sensor() const { return sensor_; }
address_class::process_type_def address_class::get_process() const { return process_; }

int address_class::get_process_index() const { return (int) process_; }

bool address_class::operator==(const address_class& other_address) {
    int this_platform = (int) get_platform();
    int this_process = (int) get_process();
    int other_platform = (int) other_address.get_platform();
    int other_process = (int) other_address.get_process();
    bool result = ( (this_platform == other_platform) && (this_process == other_process) );

    //std::cout << "this address = " << *this << "     platform  = " << this_platform << "     process = " << this_process << std::endl;
    //std::cout << "other address = " << other_address << "     platform  = " << other_platform << "     process = " << other_process << std::endl;
    //std::cout << "compare result = " << result << std::endl;
    return result;
}

// for using std:map a strict compare op is required to navigate the search tree
bool address_class::operator<(const address_class& other_address) const {

    int this_platform = (int) get_platform();
    int other_platform = (int) other_address.get_platform();
    if ( this_platform < other_platform ) return true;
    if ( this_platform > other_platform ) return false;

    int this_sensor = (int) get_sensor();
    int other_sensor = (int) other_address.get_sensor();
    if ( this_sensor < other_sensor ) return true;
    if ( this_sensor > other_sensor ) return false;

    int this_process = (int) get_process();
    int other_process = (int) other_address.get_process();
    if ( this_process < other_process ) return true;
    if ( this_process > other_process ) return false;

    //std::cout << "comapring " << *this << " < " << other_address << " and the result is == " << std::endl;
    return false;
}

std::ostream& operator<<(std::ostream& out, const address_class& address)  
{  
    address_class::platform_type_def platform = address.get_platform();
    address_class::sensor_type_def sensor = address.get_sensor();
    address_class::process_type_def process = address.get_process();
    out << "platform = " << platform << "   sensor = " << sensor << "   process = " << process;// << (int) (address.platform_); //dt.mo << '/' << dt.da << '/' << dt.yr;  
    return out;  
}

std::ostream& operator<<(std::ostream& out, const address_class::platform_type_def& platform)  
{  
    switch (platform) {
        case address_class::platform_type_def::undefined_platform:
            out << "undefined ";
            break;    
        case address_class::platform_type_def::arduino:
            out << "arduino ";
            break;    
        case address_class::platform_type_def::rpi:
            out << "rpi ";
            break;    
        case address_class::platform_type_def::pc:
            out << "pc ";
            break;
        default:
            out << "default is undefinded " << std::endl;
            break;
    }
    return out;  
}

std::ostream& operator<<(std::ostream& out, const address_class::sensor_type_def& sensor)
{  
    switch (sensor) {
        case address_class::sensor_type_def::undefined_sensor:
            out << "undefined ";
            break;    
        case address_class::sensor_type_def::camera1:
            out << "camera1 ";
            break;    
        case address_class::sensor_type_def::usonic1:
            out << "usonic1 ";
            break;    
        case address_class::sensor_type_def::imu1:
            out << "imu1 ";
            break;
        case address_class::sensor_type_def::time_snapshot:
            out << "time_snapshot ";
            break;
        default:
            out << "default is undefinded " << std::endl;
            break;
    }
    return out;  
}

std::ostream& operator<<(std::ostream& out, const address_class::process_type_def& process){
    switch (process) {
        case address_class::process_type_def::cockpit:
            out << "cockpit ";
            break;    
        case address_class::process_type_def::tcp:
            out << "tcp ";
            break;    
        case address_class::process_type_def::undefined:
            out << "undefined ";
            break;    
        default:
            out << "default is undefinded " << std::endl;
            break;
    }
    return out;  
}

