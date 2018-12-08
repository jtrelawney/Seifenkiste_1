#include <sensor_camera_class.h>

sensor_state_type sensor_camera_class::get_state(){ return sensor_state_; }

sensor_camera_class::sensor_camera_class(): sensor_base_class() , camera_(nullptr), camera_port_(0), current_frame_valid_(false) {
    std::cout << "sensor_camera_class : constructor : debuglevel = " << sensor_debug_level_ << "\n"; 
}

int sensor_camera_class::init_sensor(){

    //cv::VideoCapture test = cv::VideoCapture(camera_port_);
    //cv::VideoCapture *test = new cv::VideoCapture(camera_port_);
    camera_ = std::unique_ptr<cv::VideoCapture> ( new cv::VideoCapture(camera_port_) );
    //test_camera = = cv::VideoCapture(camera_port_);

    // decide if attempt to open camera was successful and set state accordingly
    bool isopen = camera_ -> isOpened();

    if ( !( isopen ) ) {
        sensor_state_ = sensor_state_type::sensor_init_failed;
        if (sensor_debug_level_>0) std::cout << "camera_sensor_class : init_sensor : issue opening the camera" << "\n"; 
        return -1;
    }

    sensor_state_ = sensor_state_type::sensor_ready;
    return 0;
}

void sensor_camera_class::set_sensor_type(const address_class &sensor_type){
    sensor_base_class::set_sensor_type(sensor_type);
}

int sensor_camera_class::fetch_current_frame(cv::Mat &frame){
    if (current_frame_valid_ == false) return -1;
    frame = current_frame_;
    return 0;
}

int sensor_camera_class::acquire_data(){

    // to maintain the frame state, assume failure and clear when success
    current_frame_valid_ = false;
    bool result = false;
    
    // grab a frame from the camera and get a timeetamp immediately
    result = camera_ -> grab();
    most_recent_acquisition_time_ = get_sensor_timestamp();

    if (result == false) {
        if (sensor_debug_level_>1) std::cout << "camera_sensor_class : acquire_data : issue grabing a frame" << "\n"; 
        return -1;
    }

    // retrieve the frame from the camera
    result = camera_ -> retrieve(current_frame_);
    if (result == false) {
        if (sensor_debug_level_>1) std::cout << "camera_sensor_class : acquire_data : issue restrieving the frame" << "\n"; 
        return -1;
    }

    // at this point the frame is valid
    data_acquisition_count_ ++;
    current_frame_valid_ = true;
    if (sensor_debug_level_>2) std::cout << "camera_sensor_class : acquire_data : frame ready" << "\n"; 

    return 0;
}

int sensor_camera_class::create_sensor_message(){
    return -1;
}

int sensor_camera_class:: queue_sensor_message(){
    return -1;
}

sensor_camera_class::~sensor_camera_class(){
}
