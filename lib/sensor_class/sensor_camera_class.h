#ifndef SENSOR_CAMERA_CLASS_H_
#define SENSOR_CAMERA_CLASS_H_

#include <iostream>     //cout
#include <memory>       //uiqueptr

#include "opencv2/opencv.hpp"

#include <sensor_base_class.h>

class sensor_camera_class : public sensor_base_class {

private:
    std::unique_ptr<cv::VideoCapture> camera_;
    //VideoCapture test_camera;

    int camera_port_;

    cv::Mat current_frame_;
    bool current_frame_valid_;

public:

    sensor_camera_class();
    ~sensor_camera_class();
    sensor_state_type get_state();

    int init_sensor();
    int acquire_data();
    int fetch_current_frame(cv::Mat &frame);
    int create_sensor_message();
    int queue_sensor_message();

    void set_sensor_type(const address_class &sensor_type);
};

#endif

